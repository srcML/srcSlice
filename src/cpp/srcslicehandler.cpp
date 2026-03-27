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
        std::cout << "[Slicing Started]" << "\n";

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
        std::cout << "[Slicing Finished]" << "\n";
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
        // track what files are including to be used for fragment gluing
        for (const auto& includeData : unit->includes) {
            fileDependencyTable[ctx.currentFilePath].push_back(includeData->path.ToString());
        }

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

        // manually move map entry values, merge will not merge if the keys between both maps exist
        for (auto& profileGroup : data.profileMap) {
            auto& dest = profileMap[profileGroup.first];
            dest.insert(
                dest.end(),
                std::make_move_iterator(profileGroup.second.begin()),
                std::make_move_iterator(profileGroup.second.end())
            );
        }

        // manually move map entry values, merge will not merge if the keys between both maps exist
        for (auto& funcSig : data.functionSigMap) {
            auto& dest = functionSigMap[funcSig.first];
            dest.insert(
                dest.end(),
                std::make_move_iterator(funcSig.second.begin()),
                std::make_move_iterator(funcSig.second.end())
            );
        }
        
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
        ifStmts.insert(ifStmts.end(),
            std::make_move_iterator(data.ifStmts.begin()),
            std::make_move_iterator(data.ifStmts.end())
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

// Find forward control-edges between conditional conditions. ie: if -> else if -> else
void SrcSliceHandler::ComputeOuterPaths(std::set<std::pair<SlicePosition,SlicePosition>>& controlEdges, std::vector<SlicePosition>& sLines) {
    /** @todo
     * Checking metadata within each SlicePosition, connect sline[i] to a sline[k] where k > i
     * corresponding if conditions to preceding elif conditions or the first sline within an else
    */

    for (size_t i = 0; i < sLines.size() - 1; ++i) {
        // sline[i] must be within the condition block of an if or else-if
        if (!sLines[i].GetData().isIfCondition && !sLines[i].GetData().isElifCondition)
            continue;

        // find the condition context base
        SlicePosition baseBlock;
        for (SlicePosition& data : ifdata) {
            if (sLines[i] < data) continue;
            if (IsContained(sLines[i], data)) {
                baseBlock = data;
            }
        }
        for (SlicePosition& data : elseifdata) {
            if (sLines[i] < data) continue;
            if (IsContained(sLines[i], data)) {
                baseBlock = data;
            }
        }

        if (baseBlock.GetFileName().empty()) continue;

        // find the preceding sline[k]
        size_t k = i+1;
        while (k < sLines.size()) {
            if (!IsContained(sLines[k], baseBlock)) break;
            ++k;
        }
        if (k == sLines.size()) continue;
        
        controlEdges.insert(std::make_pair(sLines[i], sLines[k]));
    }
}

// Collects the forward exiting control-flows of conditionals. ie: exiting conditional body
void SrcSliceHandler::ComputeExitPaths(std::set<std::pair<SlicePosition,SlicePosition>>& controlEdges, std::vector<SlicePosition>& sLines) {
    /** @todo
     * Find the context if_stmt the sline[i] is within to find the first sline[k] not contained in the context if_stmt where k > i
     * sline[i] must be the sline that occurs LAST within the inner context of the if_stmt context (specific if, elif, else block)
    */

    for (size_t i = 0; i < sLines.size() - 1; ++i) {
        // skip slines that are within the conditions
        if (sLines[i].GetData().isIfCondition || sLines[i].GetData().isElifCondition)
            continue;

        // sline[i] must be contained within the block of either an if, else-if, or else statement
        size_t ctxIndex = FindContextBlock(sLines[i], ifStmts);
        if (ctxIndex == -1) continue;

        // find inner-most context of sline[i]
        SlicePosition innerContext;
        for (SlicePosition& data : ifdata) {
            if (IsContained(sLines[i], data)) {
                innerContext = data;
            }
        }
        for (SlicePosition& data : elseifdata) {
            // if the inner context has been found skip this loop
            if (!innerContext.ToString().empty()) break;

            if (IsContained(sLines[i], data)) {
                innerContext = data;
            }
        }
        for (SlicePosition& data : elsedata) {
            // if the inner context has been found skip this loop
            if (!innerContext.ToString().empty()) break;

            if (IsContained(sLines[i], data)) {
                innerContext = data;
            }
        }

        // check if sline[i] is the last sline within the inner-most context
        bool isExitPath = false;
        size_t k = i+1;
        while (k < sLines.size()) {
            // if sline[k] is contained within the inner-most context
            // then sline[i], sline[k] is not an exit-path
            if (IsContained(sLines[k], innerContext)) {
                break;
            }

            // check if sline[k] is outside of the if_stmt context
            if (!IsContained(sLines[k], ifStmts[ctxIndex])) {
                isExitPath = true;
                break;
            }

            ++k;
        }
        if (!isExitPath) continue;

        
        controlEdges.insert(std::make_pair(sLines[i], sLines[k]));
    }
}

// srcSlice focuses on Forward-Slicing, therefor our Control-Flows are going to be forward-flowing
// we are not focusing on backwards-flows.
void SrcSliceHandler::ComputeControlPaths() {
    auto InBlock = [this](SlicePosition& sline, std::vector<SlicePosition>& groups) -> bool {
        // groups are focused on if, elif, else position ranges
        size_t ctxIndex = -1;
        if (groups.size() > 1) {
            ctxIndex = FindContextBlock(sline, ifStmts);
            if (ctxIndex == -1) return false;
        }

        for (auto& data : groups) {
            // if there is nesting we only want to be concerned with data related to the if_stmt context
            if (ctxIndex != -1 && IsContained(ifStmts[ctxIndex], data)) {
                continue;
            }

            if (IsContained(sline, data)) {
                return true;
            }
        }
        return false;
    };
    
    auto InLoop = [](SlicePosition& sline, std::vector<SlicePosition>& loopdata) -> bool {
        size_t ctxIndex = -1;
        if (loopdata.size() > 1) {
            ctxIndex = FindContextBlock(sline, loopdata);
            if (ctxIndex == -1) return false;
        }

        for (auto& data : loopdata) {
            if (ctxIndex != -1 && IsContained(loopdata[ctxIndex], data)) {
                continue;
            }

            if (IsContained(sline, data)) {
                return true;
            }
        }
        return false;
    };

    for (auto& varGroup : profileMap) {
        for (auto& profile : varGroup.second) {
            if (!profile.containsDeclaration) continue;

            // Collect the slice lines and put them in numerical order
            std::set<SlicePosition> sLinesOrdered;
            sLinesOrdered.insert(profile.definitions.begin(), profile.definitions.end());
            sLinesOrdered.insert(profile.uses.begin(), profile.uses.end());

            // Convert set into vector using vector ctor
            std::vector<SlicePosition> sLines(sLinesOrdered.begin(), sLinesOrdered.end());

            // Handles controledges based from: for-loops, while-loops, do-while-loops
            for (auto& loop : loopdata) {
                SlicePosition predecessor;    // origin position
                SlicePosition falseSuccessor; // where to go when loop-condition is false
                SlicePosition trueSuccessor;  // where to go when loop-condition is true (first sline within loop block)

                for (auto& sline : sLines) {
                    // see if the sline is before the loop or in the loop-control statement
                    if (predecessor.GetFileName().empty() && IsContained(sline, loop)) {
                        predecessor = sline;
                        continue;
                    }

                    // find the first sline outside the loop-block
                    if (falseSuccessor.GetFileName().empty() && sline > loop && !IsContained(sline, loop)) {
                        falseSuccessor = sline;
                        continue;
                    }

                    // find the first sline inside the loop-block
                    if (trueSuccessor.GetFileName().empty() && IsContained(sline, loop)) {
                        trueSuccessor = sline;
                        continue;
                    }
                }

                bool validPositions = !predecessor.GetFileName().empty() && !trueSuccessor.GetFileName().empty() && !falseSuccessor.GetFileName().empty();
                if (validPositions && predecessor < falseSuccessor) {
                    profile.controlEdges.insert(
                        std::make_pair(predecessor, trueSuccessor)
                    );
                    profile.controlEdges.insert(
                        std::make_pair(predecessor, falseSuccessor)
                    );
                }
            }

            // Create sline pairs between successive lines with case of if-else matching
            for (size_t i = 0; i < sLines.size() - 1; ++i) {
                if ( !(InBlock(sLines[i], ifdata) && (InBlock(sLines[i+1], elseifdata) || InBlock(sLines[i+1], elsedata)) ) ) {
                    profile.controlEdges.insert(
                        std::make_pair(sLines[i], sLines[i+1])
                    );
                }

                // if the sline[i] does not belong to any loop or condition block
                // then connect this sline to the first successive sline[k] that
                // does not also belong to any loop or condiiton block
                if (!InLoop(sLines[i], loopdata) && !InBlock(sLines[i], ifdata) && !InBlock(sLines[i], elseifdata) && !InBlock(sLines[i], elsedata)) {
                    profile.controlEdges.insert(
                        std::make_pair(sLines[i], sLines[i+1])
                    );
                }
            }

            // Find Exit-Paths and Outer-Paths that are not initially found from this algorithm
            ComputeExitPaths(profile.controlEdges, sLines);
            ComputeOuterPaths(profile.controlEdges, sLines);
        }
    }
}

SliceProfileIterator SrcSliceHandler::ArgumentProfile(const std::string& funcName, FunctionSignatureData& funcSig, int paramIndex) {
    if (funcSig.parameters.empty()) return profileMap.end();
    if (paramIndex >= funcSig.parameters.size()) return profileMap.end();

    auto Spi = profileMap.find(funcSig.parameters.at(paramIndex)->name.ToString());
    for (auto& param : funcSig.parameters) {
        if (param && param->name && param->name.GetElement()) {
            if (profileMap.find(param->name.ToString()) == profileMap.end()) {
                if (verboseMode) {
                    std::cout << "[-] " << __FUNCTION__ << ":" << __LINE__ << " | Could not find SliceProfile for parameter '"
                    << param->name.ToString() << "' of function '" << funcName << "'" << "\n";
                }
                continue;
            }

            SliceProfile& argSp = profileMap.find(param->name.ToString())->second.back();

            if (argSp.visited) {
                return Spi;
            } else {
                ResolveCall(argSp);
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
                if (spi == profileMap.end()) continue;

                // fingerprint the profile based on contained use
                for (auto& aspi : spi->second) {
                    if (!aspi.containsDeclaration) continue;
                    
                    auto usesItr = std::find(aspi.uses.begin(), aspi.uses.end(), alias.second);
                    if (usesItr == aspi.uses.end()) continue;
                    
                    // determine if the potential target is a pointer or reference
                    if (!aspi.isPointer && !aspi.isReference) continue;

                    // check if the alias has been visited
                    if (visited_alias.find(aspi.variableName) != visited_alias.end()) continue;

                    // mark alias as visited so we dont review this alias entry again (circular dependence protection)
                    visited_alias.insert(aspi.variableName);

                    // push_back alias slice profile's aliases into the source slice aliases
                    sp.aliases.insert(sp.aliases.end(), aspi.aliases.begin(), aspi.aliases.end());
                }
            }
        }
    }
}

void SrcSliceHandler::Finalize() {
    if (verboseMode || progressMode) std::cout << "[*] Performing Second-Pass" << "\n";

    for (auto& [name, profiles] : profileMap) {
        mergeFragments(profiles);
        for (auto& profile : profiles) {
            if (profile.partial) ModifySlice(profile);
        }
    }

    if (verboseMode || progressMode) std::cout << "[*] Finished Second-Pass" << "\n";
}

void SrcSliceHandler::mergeFragments(std::vector<SliceProfile>& profiles) {
    typedef std::vector<SliceProfile>::iterator Spi;
    std::unordered_map<std::string, std::vector<Spi>> possibleRootFragments;

    // move fragments to the back so roots are at the front
    std::partition(profiles.begin(), profiles.end(), [](const SliceProfile& p) {
        // root fragments remain at the front of the vector
        // while fragement profiles are moved to the back
        // to ensure upon iteration we mark all root fragments
        // before handling a fragment
        return !p.isFragment;
    });
    
    for (auto itr = profiles.begin(); itr != profiles.end();) {
        if (itr->isFragment) {
            // find the root fragment and merge data
            // then remove fragment and continue
            auto dFiles = fileDependencyTable.find(itr->file);

            auto mergeFragment = [&possibleRootFragments](Spi itr, std::vector<std::string> includedFiles) {
                for (auto& [baseFile, rootFragments] : possibleRootFragments) {
                    for (auto& rootFragment : rootFragments) {
                        // root fragment and partial fragment exist in the same file
                        if (rootFragment->file == itr->file) {
                            rootFragment->merge(*itr);
                            return;
                        }

                        // inclusions in the file the fragment originates from
                        for (auto& includeName : includedFiles) {
                            // search from end of rootFragment file string
                            // if includeName substring occurs (C++20 string)
                            if (rootFragment->file.ends_with(includeName)) {
                                // merge fragment (itr) with rootFragment
                                rootFragment->merge(*itr);
                                return;
                            }
                        }
                    }
                }
            };
            mergeFragment(itr, (dFiles != fileDependencyTable.end()) ? dFiles->second : std::vector<std::string>{});
            itr = profiles.erase(itr);
        } else {
            // check if profile is potential root
            if (itr->isGlobal || itr->classMemberVar) {
                possibleRootFragments[itr->file].push_back(itr);
            }
            ++itr;
        }
    }
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
    // sp gets marked as visited once again
    ResolveCall(sp);
}

void SrcSliceHandler::UpdateCalls(SliceProfile& sp) {
    // do not read/modify the slice if its not a partial
    if (!sp.partial) return;

    // collection of call data that is to be later added to the given slice profile
    std::vector<FunctionCallData> toInsert;
    
    for (auto itr = sp.cfunctions.begin(); itr != sp.cfunctions.end(); ++itr) {
        FunctionCallData& cfunc = *itr;

        // only update cfunc entries that do not have a definition position
        if (!cfunc.definitionPosition.GetFileName().empty()) {
            continue;
        }

        std::string name = cfunc.functionName;
        auto funcSig = functionSigMap.find(name);

        if (funcSig == functionSigMap.end()) continue;

        // if there is only one record of a function signature
        if (funcSig->second.size() == 1) {
            if (funcSig->second[0].parameters.empty()) continue;
            if (cfunc.argumentCount > funcSig->second[0].parameters.size()) continue;

            // update definitionPosition
            FunctionCallData updatedData(cfunc);
            updatedData.definitionPosition = funcSig->second[0].position;

            // replace old data with new data
            cfunc = updatedData;

            // evaluate the slice profile based off the updatedData and if its within
            // partialSliceProfiles we want to jump to it and update it (recursively)
            // before passing it back into inter-procedural
            unsigned int ArgProfParam = updatedData.parameterIndex - 1;

            /**
             * @todo may need to remove argument profile usage,
             * interprocedural skips if the argument cant be
             * resolved from a given call
             **/

            // argument slice profile
            auto spi = ArgumentProfile(
                updatedData.functionName,
                funcSig->second[0],
                ArgProfParam
            );

            if (spi == profileMap.end()) continue;

            auto sliceItr = spi->second.begin();
            std::string desiredVariableName = sliceItr->variableName;

            for (sliceItr = spi->second.begin(); sliceItr != spi->second.end(); ++sliceItr) {
                if (!sliceItr->containsDeclaration)
                    continue;
                if (sliceItr->variableName != desiredVariableName)
                    continue;
                if (SrcSliceOperations::GetSimpleFunctionName(sliceItr->function) != updatedData.functionName)
                    continue;

                auto paramDecl = funcSig->second[0].parameters[ArgProfParam];
                SlicePosition paramDeclPos(
                    paramDecl->name->startPosition,
                    paramDecl->name->endPosition,
                    sliceItr->file
                );

                if (sliceItr->initialPosition != paramDeclPos)
                    continue;

                break;
            }
        } else {
            // if a function is overloaded

            unsigned int ArgProfParam = cfunc.parameterIndex - 1;
            std::vector<FunctionSignatureData> signatures;

            for (auto& funcData : funcSig->second) {
                if (cfunc.argumentCount == funcData.parameters.size()) {
                    signatures.push_back(funcData);
                }
            }

            if (signatures.empty()) {
                for (auto& funcData : funcSig->second) {
                    if (cfunc.argumentCount == 0 || cfunc.argumentCount >= funcData.parameters.size()) continue;
                    // 1 <= argCount < paramCount
                    signatures.push_back(funcData);
                }
            }

            // update the current cfunc and add new entries per possible signature
            for (size_t i = 0; i < signatures.size(); ++i) {

                /**
                 * @todo may need to remove argument profile usage,
                 * interprocedural skips if the argument cant be
                 * resolved from a given call
                 **/

                // argument slice profile
                auto spi = ArgumentProfile(
                    cfunc.functionName,
                    signatures[i],
                    ArgProfParam
                );

                if (spi == profileMap.end())
                    continue;

                auto sliceItr = spi->second.begin();
                std::string desiredVariableName = sliceItr->variableName;

                for (sliceItr = spi->second.begin(); sliceItr != spi->second.end(); ++sliceItr) {
                    if (!sliceItr->containsDeclaration)
                        continue;
                    if (sliceItr->variableName != desiredVariableName)
                        continue;
                    if (SrcSliceOperations::GetSimpleFunctionName(sliceItr->function) != cfunc.functionName)
                        continue;

                    auto paramDecl = signatures[i].parameters[ArgProfParam];
                    SlicePosition paramDeclPos(
                        paramDecl->name->startPosition,
                        paramDecl->name->endPosition,
                        sliceItr->file
                    );

                    if (sliceItr->initialPosition != paramDeclPos)
                        continue;

                    break;
                }

                if (sliceItr == spi->second.end())
                    continue;
                if (cfunc.definitionPosition.GetFileName().empty()) {
                    // update cfunc data
                    FunctionCallData updatedData(cfunc);
                    updatedData.definitionPosition = signatures[i].position;
                    cfunc = updatedData;
                } else {
                    // append new cfunc data
                    FunctionCallData newData(cfunc);
                    newData.definitionPosition = signatures[i].position;
                    toInsert.push_back(newData);
                }
            }
        }
    }

    // insert the new calls into the profile after iteration
    // to not risk iterator invalidation
    for (auto& data : toInsert) {
        sp.insertCfunction(data);
    }

    // toggle off just incase so we do not run over this profile again
    sp.partial = false;
}

void SrcSliceHandler::ComputeInterprocedural() {
    for (auto& [name, profiles] : profileMap) {
        for (auto& sp : profiles) {
            ResolveCall(sp);
        }
    }
}

void SrcSliceHandler::ResolveCall(SliceProfile &sp) {
    // safety to ensure we avoid circular profile linkage
    if (sp.visited) return;
    // ignore profiles with no value
    if ((sp.variableName == "*LITERAL*")) return;
    
    auto addPartialSlice = [this](SliceProfile &sp) {
        if (sp.updated) return;
        sp.partial = true;
    };

    // label the profile as visited to avoid circular profile linkage
    sp.visited = true;

    if (!sp.cfunctions.empty()) {
        for (auto& cfunc : sp.cfunctions) {
            // if a cfunc ignore flag is enabled skip this index and continue
            if (cfunc.ignore)
                continue;

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
                    addPartialSlice(sp);
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
                                        funcSigCollection->second[sigIndex].currentFilePath
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

                                        // ensure we do not remove the initial decl from a slices definitions set
                                        if (profileMap.find(sp.variableName)->second.back().initialPosition != sliceItr->initialPosition) {
                                            // Parameter initial declaration def line is considered a use towards the argument
                                            profileMap.find(sp.variableName)->second.back().definitions.erase(sliceItr->initialPosition);
                                        }
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
                                              << " | Is The sliceItr Valid? " << (sliceItr != Spi->second.end()) << "\n";

                                    std::cout << "Tried Accessing Slice Variable :: " << sp.variableName << "\n";
                                    std::cout << "[-] " << __FUNCTION__ << ":" << __LINE__ << " | An Error has Occured in `ComputeInterprocedural`" << "\n";
                                }
                            }
                        } else {
                            if (verboseMode) {
                                std::cout << "[-] " << __FUNCTION__ << ":" << __LINE__ << " | ArgumentProfile could not resolve SliceProfile for Call '"
                                << cfunc.functionName << "[" << ArgProfParam << "]" << "'" << "\n";
                            }
                        }
                    }
                }
            } else {
                if (verboseMode) {
                    std::cout << "[-] " << __FUNCTION__ << ":" << __LINE__ << " | Cannot find Function Signature Collection for '" << cfunc.functionName << "'" << "\n";
                }
                addPartialSlice(sp);
            }
        }
    }
}