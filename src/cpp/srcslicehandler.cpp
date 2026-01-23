#include <srcslicehandler.hpp>

// Create custom buffers with callbacks
CallbackBuf cbOut(coutHandler);
CallbackBuf cbErr(cerrHandler);

// Number of CPU cores
int CPUCount() {
#ifdef _WIN64
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (int) sysinfo.dwNumberOfProcessors;
#elif __APPLE__
    int count = 0;
    size_t len = sizeof(count);
    sysctlbyname("machdep.cpu.core_count", &count, &len, NULL, 0);
    return count;
#else
    cpu_set_t cs;
    CPU_ZERO(&cs);
    sched_getaffinity(0, sizeof(cs), &cs);
    return CPU_COUNT(&cs);
#endif
}

// Use literal string filename ctor of srcSAXController (srcslice cpp main)
SrcSliceHandler::SrcSliceHandler(const char* filename, bool v, bool p, bool ce, int threads) : verboseMode(v), progressMode(p), calculateControlEdges(ce) {
    // if an invalid number is passed default to 5 threads
    threadCount = (threads > 0) ? threads : 5;

    unitsScaned.store(false, std::memory_order_release);

    std::thread t = std::thread(&SrcSliceHandler::ManageThreads, this);

    srcSAXController control(filename);
    srcDispatch::srcDispatcher<srcDispatch::UnitPolicy> handler(this);
    
    // p -> progress display mode
    if (p) {
        // Save original buffers and Redirect cout and cerr
        IdleBar idlebar;
        std::cout << "[Slicing Started]" << std::endl;

        control.parse(&handler); // Start parsing
        unitsScaned = true;

        if (t.joinable())
            t.join();

        // Restore original buffers
        idlebar.Finish("[srcSAXController Parse]");
        idlebar.Reset();

        Finalize();

        // Restore original buffers
        idlebar.Finish("[Second-Pass]");
        std::cout << "[Slicing Finished]" << std::endl;
    } else {
        control.parse(&handler); // Start parsing

        unitsScaned.store(true, std::memory_order_release);
        if (t.joinable()) t.join();

        Finalize();
    }
}

// Use string srcml buffer ctor of srcSAXController
SrcSliceHandler::SrcSliceHandler(std::string& sourceCodeStr, bool ce) : verboseMode(false), calculateControlEdges(ce) {
    // test-suite contains single-unit tests => multiple threads would be wasteful
    threadCount = 1;

    srcSAXController control(sourceCodeStr);
    srcDispatch::srcDispatcher<srcDispatch::UnitPolicy> handler(this);

    unitsScaned.store(false, std::memory_order_release);

    std::thread t = std::thread(&SrcSliceHandler::ManageThreads, this);

    control.parse(&handler); // Start parsing

    unitsScaned.store(true, std::memory_order_release);
    if (t.joinable()) t.join();

    Finalize();
}

SrcSliceHandler::~SrcSliceHandler() {
    while (!backlog.empty()) {
        if (backlog.front() != nullptr) {
            delete backlog.front();
        }
        backlog.pop();
    }
}

void SrcSliceHandler::Notify(const srcDispatch::PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) {
    std::shared_ptr<srcDispatch::UnitData> unit = policy->Data<srcDispatch::UnitData>();
    if (unit) {
        // push worker into queue
        backlog.push(
            new SrcSliceWorker(unit, ctx, verboseMode, calculateControlEdges)
        );
    }
}

void SrcSliceHandler::ManageThreads() {
    auto MergeStructures = [this](SrcSliceWorker* worker) {
        if (worker == nullptr) return;

        std::lock_guard<std::mutex> lock(dataMutex);
        Blob& data = worker->data;

        profileMap.merge(data.profileMap);
        functionSigMap.merge(data.functionSigMap);
        
        // make_move_iterator prevents making copies
        loopdata.insert(loopdata.end(),
            std::make_move_iterator(data.loopdata.begin()),
            std::make_move_iterator(data.loopdata.end())
        );
        forloopdata.insert(forloopdata.end(),
            std::make_move_iterator(data.forloopdata.begin()),
            std::make_move_iterator(data.forloopdata.end())
        );
        whileloopdata.insert(whileloopdata.end(),
            std::make_move_iterator(data.whileloopdata.begin()),
            std::make_move_iterator(data.whileloopdata.end())
        );
        dowhileloopdata.insert(dowhileloopdata.end(),
            std::make_move_iterator(data.dowhileloopdata.begin()),
            std::make_move_iterator(data.dowhileloopdata.end())
        );
        
        ifdata.insert(ifdata.end(),
            std::make_move_iterator(data.ifdata.begin()),
            std::make_move_iterator(data.ifdata.end())
        );
        elsedata.insert(elsedata.end(),
            std::make_move_iterator(data.elsedata.begin()),
            std::make_move_iterator(data.elsedata.end())
        );
        elseifdata.insert(elseifdata.end(),
            std::make_move_iterator(data.elseifdata.begin()),
            std::make_move_iterator(data.elseifdata.end())
        );

        /*
            while under the mutex lock:
                - perform interprocedural against the shared structure
                  after merging it with a sub-structure

                - gather partial slices that later are dealt with via
                  the second pass (finalize)
        */

        // Handles Collecting Control-Edges
        if (calculateControlEdges) ComputeControlPaths();
        
        // populates Aliases attribute in slice profiles and
        // performs crude interprocedural to connect use/def data
        ComputeAliasInterprocedural();
        ComputeInterprocedural();
    };

    auto replaceWorker = [this](SrcSliceWorker** worker) {
        if (!backlog.empty()) {
            if (backlog.front() != nullptr) {
                // start worker
                *worker = backlog.front();
                (*worker)->Start();
            }

            // remove from backlog
            backlog.pop();
        }
    };
    
    std::vector<SrcSliceWorker*> workers(threadCount, nullptr);
    bool runningJobs = true;

    while (runningJobs) {
        bool allDispatched = backlog.empty() && unitsScaned.load(std::memory_order_acquire);
        int activeJobs = 0;

        for (int i = 0; i < threadCount; ++i) {
            if (workers[i] != nullptr) {
                ++activeJobs;
                if (workers[i]->Finished()) {
                    workers[i]->WaitForJob();

                    // Handles Collecting Control-Edges
                    if (calculateControlEdges) ComputeControlPaths();
                    
                    // populates Aliases attribute in slice profiles and
                    // performs crude interprocedural to connect use/def data
                    ComputeAliasInterprocedural();
                    ComputeInterprocedural();
                    
                    // merge worker blob into main structure
                    MergeStructures(workers[i]);
                    
                    // replace worker with front of queue
                    delete workers[i]; // deallocate SrcSliceWorker object
                    workers[i] = nullptr;
                } else {
                    // job is active do not continue
                    // the iteration and potentially replace this active worker
                    continue;
                }
            }

            // pass pointer by reference so we can reassign its reference
            replaceWorker(&workers[i]);
        }

        // all jobs have been dispatched and completed
        // and no jobs are active
        if (allDispatched && activeJobs == 0) {
            runningJobs = false;
            continue;
        }

        // prevent full utilization of CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::unordered_map<std::string, std::vector<SliceProfile>>& SrcSliceHandler::GetProfileMap() {
    return profileMap;
}

// Component of function FindOtherPaths
void SrcSliceHandler::ComputeOuterPaths(std::set<std::pair<SlicePosition,SlicePosition>>& otherPaths, std::vector<SlicePosition>& sLines) {
    /*
    std::set<std::pair<Position,Position>> ifGroup;

    // Find all valid connections between if,else-if,and else blocks
    for (const auto& ifblock : ifdata) {
        bool isSingleIf = true;
        
        // ensure we dont try indexing non-existing items
        for (const auto& elseifblock : elseifdata) {
            if (ifblock == elseifblock) {
                ifGroup.insert(std::make_pair(ifblock, elseifblock));
                isSingleIf = false;
            }
        }

        // ensure we dont try indexing non-existing items
        for (const auto& elseblock : elsedata) {
            if (ifblock == elseblock) {
                ifGroup.insert(std::make_pair(ifblock, elseblock));
                isSingleIf = false;
            }
        }

        // occurs when we have a single if statement that does not connect
        // to any else-if or else statements
        if (isSingleIf)
            ifGroup.insert(std::make_pair(ifblock, ifblock));
    }

    // Iterate sLines and find potential paths between
    // sLines[i] and sLines[k], focusing on outter paths
    for (int i = 0; i < sLines.size(); ++i) {
        // iterate the remaining sLines to see
        // if a path can be formed
        for (int k = i+1; k < sLines.size(); ++k) {
            for (const auto& lineRange : ifGroup) {
                // Finding the first sLine[k] that is >= lineRange.second
                if (sLines[i] == lineRange.first && sLines[k] >= lineRange.second) {
                    // std::cout << "Potential Outter-Path --> " << sLines[i] << "," << sLines[k] << std::endl;
                    otherPaths.insert(std::make_pair(sLines[i], sLines[k]));

                    // Increment outter-control i, so we can find the next outter path
                    if (i < sLines.size()) {
                        ++i;
                    } else {
                        // escape the entire loop
                        k = i;
                        break;
                    }
                }
            }
        }
    }
        */
}

// Component of function FindOtherPaths
void SrcSliceHandler::ComputeExitPaths(std::set<std::pair<SlicePosition,SlicePosition>>& otherPaths,
                                        std::vector<SlicePosition>& sLines, std::set<SlicePosition>& ignoreLines) {
    /*
    // Iterate sLines and find potential paths between
    // sLines[i] and sLines[k], focusing on block exits
    for (int i = 0; i < sLines.size(); ++i) {
        if (ignoreLines.find(sLines[i]) != ignoreLines.end()) continue;

        if (i+1 < sLines.size()) {
            std::pair<int,int> containedBlock(0,0);
            // iterate if-data to see which if-block the current sLine is most likely contained in
            for (const auto& ifblock : ifdata) {
                // check if the current sline is contained in the ifblock
                if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                    // std::cout << sLines[i] << " contained in -> (" << ifblock.first << "," << ifblock.second << ")" << std::endl;

                    // focus on the block with the smallest gap
                    int containedBlockSize = containedBlock.second - containedBlock.first;
                    int blockSize = ifblock.second - ifblock.first;
                    // update the block we suspect the current sline is contained in if
                    // either we have not initially updated the block, or we found a block
                    // of smaller size containing sLines[i]
                    if (containedBlock.first == 0 || (blockSize < containedBlockSize)) {
                        containedBlock = ifblock;

                        //std::cout << "[*] Focusing on block -> (" <<
                        //containedBlock.first << "," << containedBlock.second <<
                        //") => " << sLines[i] << std::endl;
                    }
                }
            }

            // if we have marked a ifdata block attempt to form a connection
            if (containedBlock.first != 0) {
                // std::cout << "[*] Block of Interest -> (" <<
                // containedBlock.first << "," << containedBlock.second <<
                // ") => " << sLines[i] << std::endl;

                // reduce the search area based on the block of interest
                for (int k = i+1; k < sLines.size(); ++k) {
                    // find first sLines[k] that is not contained
                    // in a reduce union of sets: ifdata, elseifdata, and elsedata
                    bool potentialExitEnd = true;

                    for (const auto& ifblock : ifdata) {
                        // incase the block sLines[i] is nested in an if-block
                        if (ifblock.first < containedBlock.first) continue;
                        if (sLines[k] >= ifblock.first && sLines[k] <= ifblock.second) {
                            potentialExitEnd = false;
                            break;
                        }
                    }
                    for (const auto& elseblock : elsedata) {
                        // incase the block sLines[i] is nested in an else-block
                        if (elseblock.first < containedBlock.first) continue;
                        if (sLines[k] >= elseblock.first && sLines[k] <= elseblock.second) {
                            potentialExitEnd = false;
                            break;
                        }
                    }

                    // when we find the first sLines[k] that is not contained in the reduced sets
                    // form the connection and exit the loops
                    if (potentialExitEnd) {
                        // std::cout << "Potential Exit-Path --> " << sLines[i] << "," << sLines[k] << std::endl;
                        otherPaths.insert(std::make_pair(sLines[i], sLines[k]));
                        break;
                    }
                }
            }
        }
    }
    */
}

// Attempt to find other Forward Control-Flow paths | ComputeControlPaths Helper Function
std::set<std::pair<SlicePosition,SlicePosition>> SrcSliceHandler::FindOtherPaths(std::vector<SlicePosition>& sLines, std::set<SlicePosition>& ignoreLines) {
    std::set<std::pair<SlicePosition,SlicePosition>> otherPaths;

    // For each path we need to compute, there is a specific function for it
    //ComputeExitPaths(otherPaths, sLines, ignoreLines);
    //ComputeOuterPaths(otherPaths, sLines);

    return otherPaths;
}

// srcSlice focuses on Forward-Slicing, therefor our Control-Flows are going to be forward-flowing
// we are not focusing on backwards-flows.
void SrcSliceHandler::ComputeControlPaths() {
    /*
    for (std::pair<std::string, std::vector<SliceProfile>> var : profileMap) {
        // Collect the slice lines and put them in numerical order
        std::set<int> sLinesOrdered;
        sLinesOrdered.insert(var.second.back().definitions.begin(), var.second.back().definitions.end());
        sLinesOrdered.insert(var.second.back().uses.begin(), var.second.back().uses.end());
        // Convert set into vector using vector ctor
        std::vector<int> sLines(sLinesOrdered.begin(), sLinesOrdered.end());
        // Used in helper function
        std::set<int> ignoreLines;

        // Handles controledges based from: for-loops, while-loops, do-while-loops
        for (auto loop : loopdata) {
            int predecessor = 0;
            int falseSuccessor = 0;
            int trueSuccessor = loop.second;
            bool trueSuccessorExists = false;

            for (auto sl : sLines) {
                if (sl <= loop.first) {
                    predecessor = sl;
                }
                if (sl >= loop.first) {
                    falseSuccessor = sl;
                }
                for (auto firstLine : sLines) {
                    if (firstLine >= loop.first && firstLine <= loop.second && firstLine <= trueSuccessor) {
                        trueSuccessor = firstLine;
                        trueSuccessorExists = true;
                    }
                }
            }
            if (predecessor < falseSuccessor) {
                if (trueSuccessorExists) {
                    if (predecessor != trueSuccessor) {
                        if (predecessor != 0 && trueSuccessor != 0) {
                            profileMap.find(var.first)->second.back().controlEdges.insert(
                                std::make_pair(predecessor, trueSuccessor)
                            );
                        }
                    }
                }

                if (predecessor != falseSuccessor) {
                    if (predecessor != 0 && trueSuccessor != 0) {
                        profileMap.find(var.first)->second.back().controlEdges.insert(
                            std::make_pair(predecessor, falseSuccessor)
                        );
                    }
                }
            }
        }

        int prevSL = 0;
        bool loopPresent = false;
        for (int i = 0; i < sLines.size(); i++) {
            // Handle checking the next sline
            if (i + 1 < sLines.size()) {
                bool outIf = true;
                // bool outElseIf = true;
                bool outElse = true;
                
                for (auto ifblock : ifdata) {
                    if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                        outIf = false;
                        break;
                    }
                }

                if (!outIf) {
                    // when inside an if-block,
                    // check if the next sline is contained within a else-if-block
                    for (auto elseifblock : elseifdata) {
                        if (sLines[i + 1] >= elseifblock.first && sLines[i + 1] <= elseifblock.second) {
                            outElse = false;
                            if (sLines[i] >= elseifblock.first && sLines[i] <= elseifblock.second)
                                outElse = true;
                            break;
                        }
                    }
                }

                if (!outIf) {
                    // when inside an if-block AND not inside a else-if-block,
                    // check if the next sline is contained within a else-block
                    for (auto elseblock : elsedata) {
                        if (sLines[i + 1] >= elseblock.first && sLines[i + 1] <= elseblock.second) {
                            outElse = false;
                            break;
                        }
                    }
                }

                // if we are outside an if or else AND both slines do not equal each other
                if ((outIf || outElse) && sLines[i] != sLines[i + 1]) {
                    // make sure we do not push a sline of 0 into the set
                    if (sLines[i] != 0 && sLines[i + 1] != 0) {
                        profileMap.find(var.first)->second.back().controlEdges.insert(
                            std::make_pair(sLines[i], sLines[i + 1])
                        );
                        ignoreLines.insert(sLines[i]);
                        // std::cout << "[*] " << __LINE__ << " | Normal Path --> " << sLines[i] << "," << sLines[i+1] << std::endl;
                    }
                }
            }
            
            // Focus exclusively on the current sline
            bool outControlBlock = true;
            for (auto loop : loopdata) {
                // check if the current sline is contained within a: for-loop, while-loop, or do-while loop
                if (sLines[i] >= loop.first && sLines[i] <= loop.second) {
                    loopPresent = true;
                    outControlBlock = false;
                    break;
                }
            }

            if (outControlBlock) {
                // check if the current sline is contained an if-block
                for (auto ifblock : ifdata) {
                    if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                        outControlBlock = false;
                        break;
                    }
                }
            }
            if (outControlBlock) {
                // check if the current sline is contained an if-block
                for (auto elseifblock : elseifdata) {
                    if (sLines[i] >= elseifblock.first && sLines[i] <= elseifblock.second) {
                        outControlBlock = false;
                        break;
                    }
                }
            }
            if (outControlBlock) {
                // check if the current sline is contained an else-block
                for (auto elseblock : elsedata) {
                    if (sLines[i] >= elseblock.first && sLines[i] <= elseblock.second) {
                        outControlBlock = false;
                        break;
                    }
                }
            }

            // if the current sline is not contained within a conditional (body or condition/control)
            if (outControlBlock) {
                if (prevSL == 0) {
                    prevSL = sLines[i];
                } else {
                    if (prevSL != sLines[i]) {
                        if (prevSL != 0 && sLines[i] != 0 && loopPresent) {
                            profileMap.find(var.first)->second.back().controlEdges.insert(
                                std::make_pair(prevSL, sLines[i])
                            );
                            ignoreLines.insert(prevSL);
                            // std::cout << "[*] " << __LINE__ << " | Normal Path --> " << sLines[i] << "," << sLines[i+1] << std::endl;
                            loopPresent = false;
                        }
                    }
                    prevSL = sLines[i];
                }
            }
        }

        // Attempt fetching other control-flows via helper function
        std::set<std::pair<int, int>> otherPaths = FindOtherPaths(sLines, ignoreLines);
        profileMap.find(var.first)->second.back().controlEdges.insert(
            otherPaths.begin(),
            otherPaths.end()
        );
    }
    */
}

SliceProfileIterator SrcSliceHandler::ArgumentProfile(const std::string& funcName, FunctionSignatureData& funcSig, int paramIndex) {
    if (funcSig.parameters.empty()) return profileMap.end();
    if (paramIndex >= funcSig.parameters.size()) return profileMap.end();

    auto Spi = profileMap.find(funcSig.parameters.at(paramIndex)->name.ToString());
    for (auto& param : funcSig.parameters) {
        if (param && param->name && param->name.GetElement()) {
            if (profileMap.find(param->name.ToString()) == profileMap.end()) {
                if (verboseMode) {
                    std::cout << "[-] " << __LINE__ << " | Could not find SliceProfile for parameter '"
                    << param->name.ToString() << "' of function '" << funcName << "'" << std::endl;
                }
                continue;
            }

            if (profileMap.find(param->name.ToString())->second.back().visited) {
                return Spi;
            } else {
                if (profileMap.find(param->name.ToString())->second.back().cfunctions.size() > 0) {
                    for (auto& cfunc : profileMap.find(param->name.ToString())->second.back().cfunctions) {
                        if (cfunc.functionName == funcName) {
                            auto funcSigCollection = functionSigMap.find(cfunc.functionName);
                            if (funcSigCollection != functionSigMap.end()) {
                                size_t sigIndex = 0;

                                // Attempt to fingerprint the right signature based on function call definition line and called function
                                // def line data
                                for (sigIndex = 0; sigIndex < funcSigCollection->second.size(); ++sigIndex) {
                                    if (cfunc.invokePosition == funcSigCollection->second[sigIndex].position) {
                                        break;
                                    }
                                }

                                if (sigIndex < funcSigCollection->second.size()) {
                                    bool matchingName = cfunc.functionName == funcSigCollection->second[sigIndex].name;
                                    bool notVisited = visited_func.find(cfunc.functionName) == visited_func.end();

                                    if (matchingName && notVisited) {
                                        visited_func.insert(cfunc.functionName);

                                        bool containsParameters = !funcSigCollection->second[sigIndex].parameters.empty();
                                        bool validParamIndex = cfunc.parameterIndex < funcSigCollection->second[sigIndex].parameters.size();

                                        // Ensure before we run ArgumentProfile that parameters has non-zero size and can be indexed safely
                                        if (containsParameters && validParamIndex) {
                                            if (funcSigCollection->second[sigIndex].parameters[cfunc.parameterIndex]->name) {
                                                // Only run this section if the parameter name can be extracted
                                                auto recursiveSpi = ArgumentProfile(
                                                    cfunc.functionName,
                                                    funcSigCollection->second[sigIndex],
                                                    cfunc.parameterIndex - 1
                                                );
    
                                                if (profileMap.find(param->name.ToString()) != profileMap.end() &&
                                                    profileMap.find(recursiveSpi->first) != profileMap.end()) {
                                                    // Uses and Defs need to reflect based on whether its pass by reference or pass by value
                                                    if (!recursiveSpi->second.back().isReference && !recursiveSpi->second.back().isPointer) {
                                                        // pass by value
                                                        profileMap.find(param->name.ToString())->second.back().uses.insert(
                                                            recursiveSpi->second.back().definitions.begin(),
                                                            recursiveSpi->second.back().definitions.end()
                                                        );
                                                        profileMap.find(param->name.ToString())->second.back().uses.insert(
                                                                recursiveSpi->second.back().uses.begin(),
                                                                recursiveSpi->second.back().uses.end()
                                                        );
                                                    } else {
                                                        // pass by reference
                                                        profileMap.find(param->name.ToString())->second.back().definitions.insert(
                                                            recursiveSpi->second.back().definitions.begin(),
                                                            recursiveSpi->second.back().definitions.end()
                                                        );
                                                        profileMap.find(param->name.ToString())->second.back().uses.insert(
                                                                recursiveSpi->second.back().uses.begin(),
                                                                recursiveSpi->second.back().uses.end()
                                                        );
                                                    }
    
                                                    profileMap.find(param->name.ToString())->second.back().cfunctions.insert(
                                                        profileMap.find(param->name.ToString())->second.back().cfunctions.end(),
                                                        recursiveSpi->second.back().cfunctions.begin(),
                                                        recursiveSpi->second.back().cfunctions.end()
                                                    );
                                                    // ensure dependencies and aliases are within local scope
                                                    if (recursiveSpi->second.back().function == profileMap.find(param->name.ToString())->second.back().function) {
                                                        profileMap.find(param->name.ToString())->second.back().aliases.insert(
                                                            profileMap.find(param->name.ToString())->second.back().aliases.end(),
                                                            recursiveSpi->second.back().aliases.begin(),
                                                            recursiveSpi->second.back().aliases.end()
                                                        );
                                                        profileMap.find(param->name.ToString())->second.back().dvars.insert(
                                                            profileMap.find(param->name.ToString())->second.back().dvars.end(),
                                                            recursiveSpi->second.back().dvars.begin(),
                                                            recursiveSpi->second.back().dvars.end()
                                                        );
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    profileMap.find(param->name.ToString())->second.back().visited = true;
                }
            }
        }
    }

    return Spi;
}

// Need to track Aliases we have already read through
// InterProcedural from the normal call should also be reflected
// if a profile with alias(s) is used in function calls
void SrcSliceHandler::ComputeAliasInterprocedural() {
    std::unordered_set <std::string> visited_alias;

    for (auto& sliceGroup : profileMap) {
        // expand list of potential targets (aliases)
        for (auto& sp : sliceGroup.second) {
            if (!sp.containsDeclaration) continue;
            for (auto& alias : sp.aliases) {
                // view aliases of the slice profile
                auto spi = profileMap.find(alias.first);
                if (spi != profileMap.end()) {
                    // fingerprint the profile based on contained use
                    for (auto& aspi : spi->second) {
                        if (!aspi.containsDeclaration) continue;
                        auto usesItr = std::find(aspi.uses.begin(), aspi.uses.end(), alias.second);
                        if (usesItr != aspi.uses.end()) {
                            // determine if the potential target is a pointer or reference
                            if (aspi.isPointer || aspi.isReference) {
                                // check if the alias has been visited
                                if (visited_alias.find(aspi.variableName) == visited_alias.end()) {
                                    // mark alias as visited so we dont review this alias entry again (circular dependence protection)
                                    visited_alias.insert(aspi.variableName);

                                    // push_back alias slice profile's aliases into the source slice aliases
                                    sp.aliases.insert(sp.aliases.end(), aspi.aliases.begin(), aspi.aliases.end());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void SrcSliceHandler::Finalize() {
    if (verboseMode || progressMode) std::cout << "[*] Performing Second-Pass" << std::endl;

    for (size_t i = 0; i < partialSliceProfiles.size(); ++i) {
        if (partialSliceProfiles[i] == nullptr) continue;
        ModifySlice(*partialSliceProfiles[i]);
    }

    if (verboseMode || progressMode) std::cout << "[*] Finished Second-Pass" << std::endl;
}

// modular reusable component
void SrcSliceHandler::ModifySlice(SliceProfile& sp) {
    // do not revisit profiles
    if (sp.updated) return;

    // make sp as unvisited before passing it into ResolveCall
    sp.visited = false;

    // mark the slice as updated so we do not push
    // duplicate addresses into the partialSlices container
    sp.updated = true;

    // attempt to update the cfunctions entries with the full function-signature dictionary
    UpdateCalls(sp);

    // attempt ComputeInterprocedural against the slice profile (sp)
    ResolveCall(sp);

    // mark sp as visited once again
    sp.visited = true;
}

void SrcSliceHandler::UpdateCalls(SliceProfile& sp) {
    for (auto& cfunc : sp.cfunctions) {
        std::string name = cfunc.functionName;
        auto funcSig = functionSigMap.find(name);

        if (funcSig != functionSigMap.end()) {
            // if there is only one record of a function signature
            if (funcSig->second.size() == 1) {
                // update definitionPosition
                FunctionCallData updatedData(cfunc);
                updatedData.definitionPosition = funcSig->second[0].position;

                // replace old data with new data
                cfunc = updatedData;

                // evaluate the slice profile based off the updatedData and if its within
                // partialSliceProfiles we want to jump to it and update it (recursively)
                // before passing it back into inter-procedural
                unsigned int ArgProfParam = updatedData.parameterIndex - 1;
                auto spi = ArgumentProfile(
                    updatedData.functionName,
                    funcSig->second[0],
                    ArgProfParam
                );

                if (spi != profileMap.end()) {
                    auto sliceItr = spi->second.begin();
                    std::string desiredVariableName = sliceItr->variableName;

                    for (sliceItr = spi->second.begin(); sliceItr != spi->second.end(); ++sliceItr) {
                        if (sliceItr->containsDeclaration) {
                            if (sliceItr->variableName != desiredVariableName) {
                                continue;
                            }
                            if (SrcSliceOperations::GetSimpleFunctionName(sliceItr->function) != updatedData.functionName) {
                                continue;
                            }
                            auto paramDecl = funcSig->second[0].parameters[ArgProfParam];
                            SlicePosition paramDeclPos(paramDecl->name->startPosition, paramDecl->name->endPosition, sctx.currentFilePath);
                            if (sliceItr->initialPosition != paramDeclPos) {
                                continue;
                            }

                            break;
                        }
                    }

                    bool isPartialSlice = std::find(partialSliceProfiles.begin(), partialSliceProfiles.end(), &(*sliceItr)) != partialSliceProfiles.end();
                    if (sliceItr != spi->second.end() && isPartialSlice) {
                        ModifySlice(*sliceItr);
                    }
                }
            } else {
                // if a function is overloaded
            }
        }
    }
}

void SrcSliceHandler::ComputeInterprocedural() {
    for (auto& var : profileMap) {
        SliceProfile& sp = var.second.back();
        // Need to watch the Slices we attempt to dig into because
        // we are collecting slices we have no interest in
        if (!sp.visited && (sp.variableName != "*LITERAL*")) {
            ResolveCall(sp);
            sp.visited = true;
        }
    }
}

void SrcSliceHandler::ResolveCall(SliceProfile &sp) {
    if (!sp.cfunctions.empty()) {
        for (auto& cfunc : sp.cfunctions) {
            if (cfunc.ignore) {
                continue; // if a cfunc ignore flag is enabled skip this index and continue
            }
            auto funcSigCollection = functionSigMap.find(cfunc.functionName);

            if (funcSigCollection != functionSigMap.end()) {
                size_t sigIndex = 0;
                
                // Attempt to fingerprint the right signature based on
                // function call definition line and called function
                // def line data
                for (sigIndex = 0; sigIndex < funcSigCollection->second.size(); ++sigIndex) {
                    if (cfunc.definitionPosition == funcSigCollection->second[sigIndex].position) {
                        break;
                    }
                }

                if (sigIndex >= funcSigCollection->second.size()) {
                    if (!sp.updated) {
                        // do not create mulitple pointers pointing to the same thing
                        if (std::find(partialSliceProfiles.begin(), partialSliceProfiles.end(), &sp) == partialSliceProfiles.end()) {
                            partialSliceProfiles.push_back(&sp);
                        }
                    }
                    continue; // no signature could be found
                }

                if (funcSigCollection->second[sigIndex].name.empty()) {
                    continue; // the name is not usable in computation
                }

                std::string simpleFunctionName = SrcSliceOperations::GetSimpleFunctionName(funcSigCollection->second[sigIndex].name);

                unsigned int ArgProfParam = cfunc.parameterIndex - 1;

                // Ensure before we run ArgumentProfile that parameters has non-zero size and can be indexed safely

                /*** @todo look for potential issues with overload case */

                bool matchingName = cfunc.functionName == simpleFunctionName;
                bool containsParameters = !funcSigCollection->second[sigIndex].parameters.empty();
                bool validSigIndex = sigIndex < funcSigCollection->second.size();
                bool validArgIndex = ArgProfParam < funcSigCollection->second[sigIndex].parameters.size();

                if (matchingName && containsParameters && validSigIndex && validArgIndex) {
                    if (funcSigCollection->second[sigIndex].parameters[ArgProfParam]->name) {
                        // Only run this section if the parameter name can be extracted
                        auto Spi = ArgumentProfile(
                            cfunc.functionName,
                            funcSigCollection->second[sigIndex],
                            ArgProfParam
                        );

                        if (Spi != profileMap.end()) {
                            auto sliceItr = Spi->second.begin();
                            std::string desiredVariableName = sliceItr->variableName;

                            for (sliceItr = Spi->second.begin(); sliceItr != Spi->second.end(); ++sliceItr) {
                                if (sliceItr->containsDeclaration) {
                                    if (sliceItr->variableName != desiredVariableName) {
                                        continue;
                                    }
                                    if (SrcSliceOperations::GetSimpleFunctionName(sliceItr->function) != cfunc.functionName) {
                                        continue;
                                    }

                                    auto paramDecl = funcSigCollection->second[sigIndex].parameters[ArgProfParam];
                                    SlicePosition paramDeclPos(
                                        paramDecl->name->startPosition,
                                        paramDecl->name->endPosition,
                                        sctx.currentFilePath
                                    );

                                    if (sliceItr->initialPosition != paramDeclPos) {
                                        continue;
                                    }

                                    break;
                                }
                            }

                            if (profileMap.find(sp.variableName) != profileMap.end()) {
                                if (profileMap.find(Spi->first) != profileMap.end()) {
                                    if (!Spi->second.empty() && sliceItr != Spi->second.end()) {
                                        if (!sliceItr->isReference && !sliceItr->isPointer) {
                                            // pass by value
                                            profileMap.find(sp.variableName)->second.back().uses.insert(
                                                sliceItr->definitions.begin(),
                                                sliceItr->definitions.end());
                                        } else {
                                            // pass by reference
                                            profileMap.find(sp.variableName)->second.back().definitions.insert(
                                                sliceItr->definitions.begin(),
                                                sliceItr->definitions.end());
                                        }

                                        // Parameter initial declaration def line is considered a use towards the argument
                                        profileMap.find(sp.variableName)->second.back().definitions.erase(sliceItr->initialPosition);
                                        profileMap.find(sp.variableName)->second.back().uses.insert(sliceItr->initialPosition);

                                        profileMap.find(sp.variableName)->second.back().uses.insert(
                                            sliceItr->uses.begin(),
                                            sliceItr->uses.end());

                                        // ensure dependencies and aliases are within local-scope
                                        if (profileMap.find(sp.variableName)->second.back().function == sliceItr->function) {
                                            profileMap.find(sp.variableName)->second.back().aliases.insert(
                                                profileMap.find(sp.variableName)->second.back().aliases.end(),
                                                sliceItr->aliases.begin(),
                                                sliceItr->aliases.end()
                                            );
                                            profileMap.find(sp.variableName)->second.back().dvars.insert(
                                                profileMap.find(sp.variableName)->second.back().dvars.end(),
                                                sliceItr->dvars.begin(),
                                                sliceItr->dvars.end()
                                            );
                                        }
                                    }
                                }
                            } else {
                                if (verboseMode) {
                                    std::cout << std::boolalpha << "Is '" << sp.variableName << "' a Map Entry? "
                                              << (profileMap.find(sp.variableName) != profileMap.end())
                                              << " | Is Spi '" << Spi->first << "' a Map Entry? " << (profileMap.find(Spi->first) != profileMap.end())
                                              << " | Is The sliceItr Valid? " << (sliceItr != Spi->second.end()) << std::endl;

                                    std::cout << "Tried Accessing Slice Variable :: " << sp.variableName << std::endl;
                                    std::cout << "[-] " << __LINE__ << " | An Error has Occured in `ComputeInterprocedural`" << std::endl;
                                }
                            }
                        } else {
                            if (verboseMode) {
                                std::cout << "[-] " << __LINE__ << " | ArgumentProfile could not resolve SliceProfile for Call '"
                                << cfunc.functionName << "[" << ArgProfParam << "]" << "'" << std::endl;
                            }
                        }
                    }
                }
            } else {
                if (verboseMode) {
                    std::cout << "[-] " << __LINE__ << " | Cannot find Function Signature Collection for '" << cfunc.functionName << "'" << std::endl;
                }
            }
        }
    }
}