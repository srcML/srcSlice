#ifndef SRCSLICEHANDLER
#define SRCSLICEHANDLER

#include <srcsliceprofile.hpp>
#include <srcsliceevent.hpp>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>
#include <InitPolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <FunctionCallPolicy.hpp>
#include <fstream>
#include <ReturnPolicy.hpp>
#include <ConditionalPolicy.hpp>
#include <sstream>

class SrcSliceHandler
        : public srcSAXEventDispatch::EventListener,
          public srcSAXEventDispatch::PolicyDispatcher,
          public srcSAXEventDispatch::PolicyListener {
public:
    ~SrcSliceHandler() { delete sliceEventData; };
    std::unordered_map<std::string, std::vector<SliceProfile>>* profileMap;

    SrcSliceHandler(const char* filename, std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {})
            : srcSAXEventDispatch::PolicyDispatcher(listeners) {
        srcSAXController control(filename);
        srcSAXEventDispatch::srcSAXEventDispatcher<SrcSliceEvent> handler(this);
        control.parse(&handler); // Start parsing
    }

    void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx [[maybe_unused]]) override {
        if (typeid(SrcSliceEvent) == typeid(*policy)) {
            sliceEventData = policy->Data<SliceEventData>();

            profileMap = sliceEventData->pmPtr;

            std::cout << "Collected Data Size :: " << profileMap->size() << std::endl;
            std::cout << "New Addr of PM :: " << profileMap << std::endl;

            SrcSliceFinalize();
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy [[maybe_unused]], srcSAXEventDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}
    

    void RepairVariableNames() {
        for (auto& mapItr : *profileMap) {
            for (auto& slice : mapItr.second) {
                // repairing variable names from line 677
                slice.variableName = slice.variableName.substr(0, slice.variableName.find(32));
            }
        }
    }
    
    auto ArgumentProfile(std::pair<std::string, SignatureData> func, int paramIndex, std::unordered_set<std::string> visit_func) {
	    auto Spi = profileMap->find(func.second.parameters.at(paramIndex).nameOfIdentifier);

        // Ensure the key exists in the map
        std::string functionName = func.first;
        if (sliceEventData->funcDefMap.find(functionName) == sliceEventData->funcDefMap.end()) {
            sliceEventData->funcDefMap[functionName] = std::vector<unsigned int>();
        }

        // Check for Duplicates before pushing a line number
        auto& lineVector = sliceEventData->funcDefMap[functionName];
        if (std::find(lineVector.begin(), lineVector.end(), func.second.lineNumber) == lineVector.end()) {
            lineVector.push_back(func.second.lineNumber);
            // std::cout << functionName << " | Line Num " << func.second.lineNumber << std::endl;
        }

        for (auto param : func.second.parameters) {
            if (profileMap->find(param.nameOfIdentifier)->second.back().visited) {
                return Spi;
            } else {
                for (auto cfunc : profileMap->find(param.nameOfIdentifier)->second.back().cfunctions) {
                    if (cfunc.first.compare(func.first) != 0) {
                        auto function = sliceEventData->functionSigMap.find(cfunc.first);
                        if (function != sliceEventData->functionSigMap.end()) {
                            if (cfunc.first.compare(function->first) == 0 && visit_func.find(cfunc.first) == visit_func.end()) {
				                visit_func.insert(cfunc.first);
                                auto recursiveSpi = ArgumentProfile(*function, std::atoi(cfunc.second.first.c_str()) - 1, visit_func);
                                if (profileMap->find(param.nameOfIdentifier) != profileMap->end() &&
                                    profileMap->find(recursiveSpi->first) != profileMap->end()) {
                                    profileMap->find(param.nameOfIdentifier)->second.back().definitions.insert(
                                            recursiveSpi->second.back().definitions.begin(),
                                            recursiveSpi->second.back().definitions.end());
                                    profileMap->find(param.nameOfIdentifier)->second.back().uses.insert(
                                            recursiveSpi->second.back().uses.begin(),
                                            recursiveSpi->second.back().uses.end());
                                    profileMap->find(param.nameOfIdentifier)->second.back().cfunctions.insert(
                                            profileMap->find(
                                                    param.nameOfIdentifier)->second.back().cfunctions.begin(),
                                            recursiveSpi->second.back().cfunctions.begin(),
                                            recursiveSpi->second.back().cfunctions.end());
                                    profileMap->find(param.nameOfIdentifier)->second.back().aliases.insert(
                                            recursiveSpi->second.back().aliases.begin(),
                                            recursiveSpi->second.back().aliases.end());
                                    profileMap->find(param.nameOfIdentifier)->second.back().dvars.insert(
                                            recursiveSpi->second.back().dvars.begin(),
                                            recursiveSpi->second.back().dvars.end());
                                }
                            }
                        }
                    }
                }
                profileMap->find(param.nameOfIdentifier)->second.back().visited = true;
            }
        }
        return Spi;
    }

    void InsertSwitchData(SliceProfile& sliceProfile) {
        for (auto initDeclItem : sliceEventData->initDeclData) {
            // With a collection of data concerning where all variables are initially declared
            // check if the sliceProfile passed matches with a variable name within the collection
            if (sliceProfile.variableName != initDeclItem.first) continue;

            // Verify the correct slice by checking if the slice definition contains
            // the initial decl line number for the slice with the matching name
            if (sliceProfile.definitions.find(initDeclItem.second) == sliceProfile.definitions.end()) continue;

            // Store iterators of the functionSigMap in a vector
            std::vector<std::unordered_map<std::string, SignatureData>::iterator> funcSigMapItrs;
            for (auto it = sliceEventData->functionSigMap.begin(); it != sliceEventData->functionSigMap.end(); ++it) {
                funcSigMapItrs.push_back(it);
            }

            // Reverse Iterate through a collection of functions
            for (auto func = funcSigMapItrs.rbegin(); func != funcSigMapItrs.rend(); ++func) {
                // Check if there is a function after where we currently are
                auto nextFunc = std::next(func);
                
                // There is a function after the current
                if (nextFunc != funcSigMapItrs.rend()) {
                    // Using the initial variable decl line, check if it is within the scope of a function
                    // to prevent using variables with the same names in different functions, this works as
                    // another verify check
                    if (initDeclItem.second >= (*func)->second.lineNumber && initDeclItem.second <= (*nextFunc)->second.lineNumber - 1) {
                        for (auto data : *sliceEventData->switchUses) {
                            // When iterating through the collection of collected Switch Uses
                            // check if the name of the the variable the use list goes to
                            // matches with the slice profile variable name
                            if (data.first != initDeclItem.first) continue;

                            // While iterating the collection of use line numbers
                            // we want to verify that the uses are after the initial decl line
                            // and we want to check if the use number is not contained inside
                            // an outside function
                            for (auto nums : data.second) {
                                if (nums < initDeclItem.second) continue;
                                if (nums > (*nextFunc)->second.lineNumber - 1) continue;
                                sliceProfile.uses.insert(nums);
                            }
                        }

                        for (auto data : *sliceEventData->switchDefs) {
                            // When iterating through the collection of collected Switch Defs
                            // check if the name of the the variable the def list goes to
                            // matches with the slice profile variable name
                            if (data.first != initDeclItem.first) continue;

                            // While iterating the collection of def line numbers
                            // we want to verify that the defs are after the initial decl line
                            // and we want to check if the def number is not contained inside
                            // an outside function
                            for (auto nums : data.second) {
                                if (nums < initDeclItem.second) continue;
                                sliceProfile.definitions.insert(nums);
                            }
                        }
                    }
                } else { // We're on the last/only function
                    // Check if the variables initial declaration is within the final functions scope
                    if (initDeclItem.second >= (*func)->second.lineNumber) {
                        // Uses the same logic as up above
                        for (auto data : *sliceEventData->switchUses) {
                            if (data.first != initDeclItem.first) continue;
                            for (auto nums : data.second) {
                                if (nums < initDeclItem.second) continue;
                                sliceProfile.uses.insert(nums);
                            }
                        }

                        for (auto data : *sliceEventData->switchDefs) {
                            if (data.first != initDeclItem.first) continue;
                            for (auto nums : data.second) {
                                if (nums < initDeclItem.second) continue;
                                sliceProfile.definitions.insert(nums);
                            }
                        }
                    }
                }
            }   
        }
    }

    std::unordered_map<std::string, std::vector<SliceProfile>>& GetProfileMap() const {
        std::cout << "Size on Get :: " << profileMap->size() << std::endl;
        std::cout << "   Addr of Get :: " << profileMap << std::endl;
        return *profileMap;
    }

    // split into 3 readable functions
    void PassOver() {
        // Create a set of data representing function scopes
        // in ascending order from line number
        std::map<std::string, unsigned int> functionBounds;
        for (auto funcSig : sliceEventData->functionSigMap) {
            functionBounds[funcSig.first] = funcSig.second.lineNumber;
        }

        // Pass Over to Update any errors from slices first run
        for (auto mapItr = profileMap->begin(); mapItr != profileMap->end(); ++mapItr) {
            for (auto sliceItr = mapItr->second.begin(); sliceItr != mapItr->second.end(); ++sliceItr) {
                if (sliceItr->containsDeclaration) {
                    // Variables that are reference variables should not carry aliases
                    if (sliceItr->isReference) {
                        sliceItr->aliases.clear();
                    }

                    // Remove def lines concerning lines where params are
                    // declared and moving those lines to use

                    /*
                    --------------------------------------
                    1    int bar (int x) {
                    2        return ++x; // def use
                    3    }
                    4    int main () {
                    5        int y = 0;
                    6        bar(y);
                    7        std::cout << y << std::endl;
                    8        return 0;
                    9    }
                    --------------------------------------
                    For the following, profile y should place
                    lines 1 and 2 as a use and not a def.

                    
                    --------------------------------------
                    1    int bar (int x) {
                    2        x = 5; // def
                    3        return x; // use
                    4    }
                    5    int main () {
                    6        int y = 0;
                    7        bar(y);
                    8        std::cout << y << std::endl;
                    9        return 0;
                    10   }
                    --------------------------------------
                    For the following, profile y should place
                    lines 1, 3. Should be uses but 2 concerning
                    redefining x we dont want to add this as a use
                    */

                    std::unordered_set <std::string> junkMap; // Need this to use the ArgumentProfile function

                    for (auto line : *sliceEventData->possibleDefinitions) {
                        // within the slice does the line exist within the def set
                        if ( sliceItr->definitions.find(line) != sliceItr->definitions.end() ) {
                            // Check the cfunctions to see if defs need switched
                            for (auto cfunctData : sliceItr->cfunctions) {
                                std::string name = cfunctData.first;
                                
                                auto funct = sliceEventData->funcDefMap.find(name);
                                if (funct != sliceEventData->funcDefMap.end()) {
                                    auto Spi = ArgumentProfile(*sliceEventData->functionSigMap.find(name), std::atoi(cfunctData.second.first.c_str()) - 1, junkMap);
                                    auto sliceParamItr = Spi->second.begin();

                                    for (auto lineNum : funct->second) {
                                        // ensure the defs contains the line number before swapping
                                        if (sliceItr->definitions.find(lineNum) != sliceItr->definitions.end()) {
                                            // Incase we run into a recursive function that the sliceParamItr points back to sliceItr
                                            // we dont want to remove its true definition
                                            if (sliceParamItr != sliceItr) {
                                                sliceItr->definitions.erase(lineNum);
                                            }
                                            sliceItr->uses.insert(lineNum);
                                        }
                                    }
                                }
                            }
                        }
                    }


                    // Iterate through called function data if the slice has any data
                    for (auto cfunctData : sliceItr->cfunctions) {
                        std::string name = cfunctData.first;
                        auto funct = sliceEventData->functionSigMap.find(name);
                        if (funct != sliceEventData->functionSigMap.end()) {
                            // If we get a valid find on a function shown in a profiles called functions
                            // we want to extract the slice profile associated in the function params
                            auto Spi = ArgumentProfile(*funct, std::atoi(cfunctData.second.first.c_str()) - 1, junkMap);
                            auto sliceParamItr = Spi->second.begin();
                            std::string paramVarName = funct->second.parameters.at(std::atoi(cfunctData.second.first.c_str()) - 1).nameOfIdentifier;

                            // If we run into recursive algorithms we want to ensure
                            // we dont hit an infinite loop due to the sliceItr pointing
                            // to the same object as sliceParamItr
                            if (sliceParamItr == sliceItr) break;

                            for (auto sliceParamItr = Spi->second.begin(); sliceParamItr != Spi->second.end(); ++sliceParamItr) {
                                if (sliceParamItr->containsDeclaration) {
                                    if (sliceParamItr->function == name && *(sliceParamItr->definitions.begin()) == funct->second.lineNumber && sliceParamItr->variableName == paramVarName ) {
                                        // If the sliceParamItr is a pointer or a reference
                                        // we want to push the redefinitions of the sliceParamItr
                                        // and push the uses of the sliceParamItr to sliceItr

                                        if (sliceParamItr->isReference || sliceParamItr->isPointer) {
                                            sliceItr->uses.insert(sliceParamItr->uses.begin(), sliceParamItr->uses.end());
                                            sliceItr->definitions.insert(sliceParamItr->definitions.begin(), sliceParamItr->definitions.end());

                                            // we need to swap the initial decl line from sliceItrs def to a use
                                            for (auto initDeclItem : sliceEventData->initDeclData) {
                                                // With a collection of data concerning where all variables are initially declared
                                                // check if the sliceProfile passed matches with a variable name within the collection
                                                if (sliceParamItr->variableName != initDeclItem.first) continue;

                                                // Verify the correct slice by checking if the slice definition contains
                                                // the initial decl line number for the slice with the matching name
                                                if (sliceParamItr->definitions.find(initDeclItem.second) == sliceParamItr->definitions.end()) continue;

                                                // Ensure the defLine exists in the sliceItrs definitons list
                                                if (sliceItr->definitions.find(initDeclItem.second) != sliceItr->definitions.end()) {
                                                    sliceItr->definitions.erase(initDeclItem.second);
                                                    sliceItr->uses.insert(initDeclItem.second);
                                                }
                                            }
                                        } else {
                                            // Once we have the correct parameter slice, we need to compare its
                                            // uses and defs to whats present in sliceItr and remove some non-initial defs
                                            // from the slices as the use of sliceItr concerning params should only have the
                                            // line where the param was initially declared, we dont want to store pure redefinitons
                                            // at this point in time
                                            for (auto defLines : sliceParamItr->definitions) {
                                                // Ensure the defLine exists in the sliceItrs definitons list
                                                if (sliceItr->definitions.find(defLines) != sliceItr->definitions.end()) {
                                                    sliceItr->definitions.erase(defLines);
                                                }
                                            }
                                        }

                                        break;
                                    }
                                }
                            }
                        }
                    }

                    // Aliases are to be local within scope of their focused slice
                    for (auto alias = sliceItr->aliases.begin(); alias != sliceItr->aliases.end();) {
                        bool removedData = false;

                        if (sliceEventData->functionCallList.find(alias->second) != sliceEventData->functionCallList.end()) {
                            // remove aliases formed at the occurance of a function call
                            sliceItr->aliases.erase(alias++);
                        } else {
                            // check if the line where the alias is formed is within
                            // local scope of the sliceItr variable
                            unsigned int localScopeStart = functionBounds[sliceItr->function];

                            // if the alias was formed in a function above the local scope
                            // we can assume it is not within local scope
                            if (alias->second < localScopeStart) {
                                sliceItr->aliases.erase(alias++);
                                removedData = true;
                            } else {
                                // define the rough estimated end of local scope
                                unsigned int endOfLocalScope = 0;
                                for (auto data : functionBounds) {
                                    if (data.second > localScopeStart) {
                                        endOfLocalScope = data.second;
                                        break;
                                    }
                                }

                                // if the alias is formed farther down then the
                                // end of the local scope remove this alias
                                if (endOfLocalScope < alias->second) {
                                    if (endOfLocalScope == 0) break;
                                    sliceItr->aliases.erase(alias++);
                                    removedData = true;
                                }
                            }

                            if (!removedData) ++alias;
                        }
                    }

                    // Dvars are to be local within scope of their focused slice
                    for (auto dvar = sliceItr->dvars.begin(); dvar != sliceItr->dvars.end();) {
                        bool removedData = false;

                        if (sliceEventData->functionCallList.find(dvar->second) != sliceEventData->functionCallList.end()) {
                            // remove aliases formed at the occurance of a function call
                            sliceItr->dvars.erase(dvar++);
                        } else {
                            // check if the line where the alias is formed is within
                            // local scope of the sliceItr variable
                            unsigned int localScopeStart = functionBounds[sliceItr->function];

                            // if the alias was formed in a function above the local scope
                            // we can assume it is not within local scope
                            if (dvar->second < localScopeStart) {
                                sliceItr->dvars.erase(dvar++);
                                removedData = true;
                            } else {
                                // definte the rough estimated end of local scope
                                unsigned int endOfLocalScope = 0; // if no end of scope is found we are viewing the scope of the final function
                                for (auto data : functionBounds) {
                                    if (data.second > localScopeStart) {
                                        endOfLocalScope = data.second;
                                        break;
                                    }
                                }

                                // if the alias is formed farther down then the
                                // end of the local scope remove this alias
                                if (endOfLocalScope < dvar->second) {
                                    if (endOfLocalScope == 0) break;

                                    sliceItr->dvars.erase(dvar++);
                                    removedData = true;
                                }
                            }

                            if (!removedData) ++dvar;
                        }
                    }
                }
            }
        }

        // Update Dvars
        for (auto dvarData : *sliceEventData->possibleDvars) {
            for (auto slice : dvarData.dvars) {
                // by using the pair we can find the correct slice profile
                // we will insert dvarData.lhsName into, along with using
                // other data we have collected
                auto Spi = profileMap->find(slice.first);
                for (auto sliceParamItr = Spi->second.begin(); sliceParamItr != Spi->second.end(); ++sliceParamItr) {
                    if (sliceParamItr->containsDeclaration) {
                        if (sliceParamItr->function != dvarData.function) {
                            continue;
                        }
                        if (sliceParamItr->uses.find(dvarData.lhsDefLine) == sliceParamItr->uses.end()) {
                            continue;
                        }

                        sliceParamItr->dvars.insert(std::make_pair(dvarData.lhsName, dvarData.lhsDefLine));
                    }
                }
            }
        }
    }

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;
	    for (std::pair<std::string, std::vector<SliceProfile>> var : *profileMap) {
            // Need to watch the Slices we attempt to dig into because we are collecting slices we have no interest in
            if (!profileMap->find(var.first)->second.back().visited && (var.second.back().variableName != "*LITERAL*")) {
                if (!var.second.back().cfunctions.empty()) {
                    for (auto cfunc : var.second.back().cfunctions) {
                        auto funcIt = sliceEventData->functionSigMap.find(cfunc.first);
                        if(funcIt != sliceEventData->functionSigMap.end()) {
                            if (cfunc.first.compare(funcIt->first) == 0) { //TODO fix for case: Overload
                                auto Spi = ArgumentProfile(*funcIt, std::atoi(cfunc.second.first.c_str()) - 1, visited_func);
                                auto sliceItr = Spi->second.begin();
                                std::string desiredVariableName = sliceItr->variableName;

                                for (sliceItr = Spi->second.begin(); sliceItr != Spi->second.end(); ++sliceItr) {
                                    if (sliceItr->containsDeclaration) {
                                        if (sliceItr->variableName != desiredVariableName) {
                                            continue;
                                        }
                                        if (sliceItr->function != cfunc.first.substr(0, cfunc.first.find('-'))) {
                                            continue;
                                        }
                                        if (sliceItr->lineNumber != std::stoi(cfunc.second.second)) {
                                            continue;
                                        }

                                        break;
                                    }
                                }

                                if (profileMap->find(var.first) != profileMap->end() && profileMap->find(Spi->first) != profileMap->end() && sliceItr != Spi->second.end()) {
                                    if (!sliceItr->isReference && !sliceItr->isPointer) {
                                        // pass by value
                                        profileMap->find(var.first)->second.back().uses.insert(
                                                sliceItr->definitions.begin(),
                                                sliceItr->definitions.end());
                                    } else
                                    {
                                        // pass by reference
                                        profileMap->find(var.first)->second.back().definitions.insert(
                                                sliceItr->definitions.begin(),
                                                sliceItr->definitions.end());
                                    }

                                    profileMap->find(var.first)->second.back().uses.insert(
                                            sliceItr->uses.begin(),
                                            sliceItr->uses.end());

                                    // By converting the cfunctions vector to a set, allows us to remove
                                    // duplicate entries, once those are removed we can convert this cleaned
                                    // set back into its vector form
                                    profileMap->find(var.first)->second.back().cfunctions.insert(
                                            profileMap->find(var.first)->second.back().cfunctions.begin(),
                                            sliceItr->cfunctions.begin(),
                                            sliceItr->cfunctions.end());
                                    auto oldCalledFunctions = profileMap->find(var.first)->second.back().cfunctions;
                                    std::set<std::pair<std::string, std::pair<std::string, std::string>>> calledFunctionSet(oldCalledFunctions.begin(), oldCalledFunctions.end());
                                    profileMap->find(var.first)->second.back().cfunctions = std::vector<std::pair<std::string, std::pair<std::string, std::string>>>(calledFunctionSet.begin(), calledFunctionSet.end());

                                    profileMap->find(var.first)->second.back().aliases.insert(
                                            sliceItr->aliases.begin(),
                                            sliceItr->aliases.end());
                                    profileMap->find(var.first)->second.back().dvars.insert(
                                            sliceItr->dvars.begin(),
                                            sliceItr->dvars.end());
                                } else {
                                    std::cout << "[-] An Error has Occured in `ComputeInterprocedural`" << std::endl;
                                }
                            }
                        }
                    }
                }
                profileMap->find(var.first)->second.back().visited = true;
            }
        }
    }

    void ComputeControlPaths() {
        for (std::pair<std::string, std::vector<SliceProfile>> var : *profileMap) {
            std::vector<int> sLines;
            std::merge(var.second.back().definitions.begin(), var.second.back().definitions.end(),
                       var.second.back().uses.begin(), var.second.back().uses.end(),
                       std::inserter(sLines, sLines.begin()));
            for (auto loop : sliceEventData->loopdata) {
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
                            profileMap->find(var.first)->second.back().controlEdges.insert(
                                    std::make_pair(predecessor, trueSuccessor));
                        }
                    }

                    if (predecessor != falseSuccessor) {
                        profileMap->find(var.first)->second.back().controlEdges.insert(
                                std::make_pair(predecessor, falseSuccessor));
                    }
                }
            }
            int prevSL = 0;
            for (int i = 0; i < sLines.size(); i++) {
                if (i + 1 < sLines.size()) {
                    bool outIf = true;
                    bool outElse = true;
                    for (auto ifblock : sliceEventData->ifdata) {
                        if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                            outIf = false;
                            break;
                        }
                    }
                    if (!outIf) {
                        for (auto elseblock : sliceEventData->elsedata) {
                            if (sLines[i + 1] >= elseblock.first && sLines[i + 1] <= elseblock.second) {
                                outElse = false;
                                break;
                            }
                        }
                    }
                    if ((outIf || outElse) && sLines[i] != sLines[i + 1]) {
                        if (sLines[i] != sLines[i + 1]) {
                            profileMap->find(var.first)->second.back().controlEdges.insert(
                                    std::make_pair(sLines[i], sLines[i + 1]));
                        }
                    }
                }
                bool outControlBlock = true;
                for (auto loop : sliceEventData->loopdata) {
                    if (sLines[i] >= loop.first && sLines[i] <= loop.second) {
                        outControlBlock = false;
                        break;
                    }
                }
                if (outControlBlock) {
                    for (auto ifblock : sliceEventData->ifdata) {
                        if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                            outControlBlock = false;
                            break;
                        }
                    }
                }
                if (outControlBlock) {
                    for (auto elseblock : sliceEventData->elsedata) {
                        if (sLines[i] >= elseblock.first && sLines[i] <= elseblock.second) {
                            outControlBlock = false;
                            break;
                        }
                    }
                }
                if (outControlBlock) {
                    if (prevSL == 0) {
                        prevSL = sLines[i];
                    } else {
                        if (prevSL != sLines[i]) {
                            profileMap->find(var.first)->second.back().controlEdges.insert(
                                    std::make_pair(prevSL, sLines[i]));
                        }

                        prevSL = 0;
                    }
                }
            }
        }
    }

protected:
    void *DataInner() const override {
        return (void *) 0; // export profile to listeners
    }

private:
    SliceEventData* sliceEventData;

    void SrcSliceFinalize() {
        std::cout << "Finalize Size :: " << profileMap->size() << std::endl;
        for (auto it = profileMap->begin(); it != profileMap->end(); ++it) {
            for (std::vector<SliceProfile>::iterator sIt = it->second.begin(); sIt != it->second.end(); ++sIt) {
                if (sIt->containsDeclaration) {
                    std::vector<SliceProfile>::iterator sIt2 = it->second.begin();
                    while (sIt2 != it->second.end()) {
                        if (!sIt2->containsDeclaration) {
                            std::cout << "NAME: " << sIt2->variableName << std::endl;
                            sIt->uses.insert(sIt2->uses.begin(), sIt2->uses.end());
                            sIt->definitions.insert(sIt2->definitions.begin(), sIt2->definitions.end());
                            sIt->dvars.insert(sIt2->dvars.begin(), sIt2->dvars.end());
                            sIt->aliases.insert(sIt2->aliases.begin(), sIt2->aliases.end());
                            sIt->cfunctions.reserve(sIt->cfunctions.size() + sIt2->cfunctions.size());
                            sIt->cfunctions.insert(sIt->cfunctions.end(), sIt2->cfunctions.begin(),
                                                    sIt2->cfunctions.end());
                            sIt2 = it->second.erase(sIt2);
                            sIt = sIt2;
                        } else {
                            ++sIt2;
                        }
                    }
                }
            }
        }

        // Updating Data before PassOver
        // need to use auto ref to ensure the profile objects
        // are references and not copies
        for (auto& profile : *profileMap) {
            for (auto& slice : profile.second) {
                if (slice.containsDeclaration) {
                    // Attempt to insert the Switch_Stmt data collected to the appropriate slice
                    InsertSwitchData(slice);

                    // Updating the called function definition line for cases
                    // where function calls occured before the function signature was created
                    for (auto& cfunct : slice.cfunctions) {
                        auto functSigData = sliceEventData->functionSigMap.find(cfunct.first);
                        if (functSigData != sliceEventData->functionSigMap.end()) {
                            cfunct.second.second = std::to_string(functSigData->second.lineNumber);
                        }
                    }
                }
            }
        }

        // Handles Collecting Control-Edges
        // ComputeControlPaths();

        ComputeInterprocedural();

        // Performs a pass over the data to fix any discrepancies
        // possibly produce by the original output
        PassOver();
        std::cout << "End Of Finalize Size :: " << profileMap->size() << std::endl << std::endl;
    }
};


#endif