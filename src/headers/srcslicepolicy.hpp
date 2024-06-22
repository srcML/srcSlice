#ifndef SRCSLICEPOLICY
#define SRCSLICEPOLICY

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
#include <fstream> // Added this to allow my save to file implementation to work in srcslice.cpp

bool StringContainsCharacters(const std::string &str) {
    for (char ch : str) {
        if (std::isalpha(ch)) {
            return true;
        }
    }
    return false;
}

class SliceProfile {
public:
    SliceProfile() : index(0), containsDeclaration(false), potentialAlias(false), dereferenced(false),
                     isGlobal(false) { jsonOut = false; }

    SliceProfile(
            std::string name, int line, bool alias = 0, bool global = 0,
            std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {},
            std::vector<std::pair<std::string, std::string>> cFunc = {},
            std::set<std::string> dv = {}, bool containsDecl = false,
            std::set<std::pair<int, int>> edges = {}, bool visit = false) :
            variableName(name), lineNumber(line), potentialAlias(alias),
            isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc),
            dvars(dv), containsDeclaration(containsDecl), controlEdges(edges),
            visited(visit) {
        jsonOut = false;
        dereferenced = false;
    }

    unsigned int index;
    int lineNumber;
    std::string file;
    std::string function;
    std::string nameOfContainingClass;
    bool potentialAlias;
    bool dereferenced;

    bool isGlobal;
    bool containsDeclaration;

    std::string variableName;
    std::string variableType;
    std::unordered_set<std::string> memberVariables;

    std::set<unsigned int> definitions;
    std::set<unsigned int> uses;

    std::set<std::string> dvars;
    std::set<std::string> aliases;

    std::vector<std::pair<std::string, std::string>> cfunctions;

    std::set<std::pair<int, int>> controlEdges;

    bool visited;
    bool jsonOut;

    size_t sliceIndex;

    // simple switch allowing to output operator overload to know
    // when we need to output json rather than plain-text
    void SetJsonOut(bool b) { jsonOut = b; }

    void SetSliceIndex(size_t ind) { sliceIndex = ind; }

    // Added ostream operator overload to control where the output goes
    // whether to screen or a file.

    // THIS OUTPUT is not meant for human eyes!!!
    friend std::ostream& operator<<(std::ostream& out, SliceProfile& profile) {
        if (!profile.jsonOut)
        {
            out << "==========================================================================" << std::endl;
            out << "File: " << profile.file << std::endl << "Function Location: " << profile.function << std::endl;
            out << "Name and type: " << profile.variableName << " " << profile.variableType << std::endl;
            out << "Containing class: " << profile.nameOfContainingClass << std::endl;
            out << "Dvars: {";
            for (auto dvar : profile.dvars) {
                out << dvar << ",";
            }
            out << "}" << std::endl;
            out << "Aliases: {";
            for (auto alias : profile.aliases) {
                out << alias << ",";
            }
            out << "}" << std::endl;
            out << "Cfunctions: {";
            for (auto cfunc : profile.cfunctions) {
                out << cfunc.first << " " << cfunc.second << ",";
            }
            out << "}" << std::endl;
            out << "Use: {";
            for (auto use : profile.uses) {
                out << use << ",";
            }
            out << "}" << std::endl;
            out << "Def: {";
            for (auto def : profile.definitions) {
                out << def << ",";
            }
            out << "}" << std::endl;
            out << "Control Edges: {";
            for (auto edge : profile.controlEdges) {
                out << "(" << edge.first << ", " << edge.second << ")" << ",";
            }
            out << "}" << std::endl;
            out << "==========================================================================" << std::endl;
        } else
            {
                out << "\"slice_" << profile.sliceIndex << "\" : {" << std::endl;
                
                out << "    \"file\":\"" << profile.file << "\"," << std::endl;
                out << "    \"class\":\"" << profile.nameOfContainingClass << "\"," << std::endl;
                out << "    \"function\":\"" << profile.function << "\"," << std::endl;
                out << "    \"type\":\"" << profile.variableType << "\"," << std::endl;
                out << "    \"name\":\"" << profile.variableName << "\"," << std::endl;

                out << "    \"dependentVariables\": [ ";
                for (auto dvar : profile.dvars) {
                    if (dvar != *(--profile.dvars.end()))
                        out << "\"" << dvar << "\", ";
                    else
                        out << "\"" << dvar << "\"";
                }
                out << " ]," << std::endl;

                out << "    \"aliases\": [ ";
                for (auto alias : profile.aliases) {
                    if (alias != *(--profile.aliases.end()))
                        out << "\"" << alias << "\", ";
                    else
                        out << "\"" << alias << "\"";
                }
                out << " ]," << std::endl;

                out << "    \"calledFunctions\": [ ";
                for (auto cfunc : profile.cfunctions) {
                    if (cfunc != *(--profile.cfunctions.end()))
                        out << "{\"functionName\": \"" << cfunc.first << "\", \"parameterNumber\": \"" << cfunc.second << "\"}, ";
                    else
                        out << "{\"functionName\": \"" << cfunc.first << "\", \"parameterNumber\": \"" << cfunc.second << "\"}";
                }
                out << " ]," << std::endl;

                out << "    \"use\": [ ";
                for (auto use : profile.uses) {
                    if (use != *(--profile.uses.end()))
                        out << use << ", ";
                    else
                        out << use;
                }
                out << " ]," << std::endl;

                out << "    \"definition\": [ ";
                for (auto def : profile.definitions) {
                    if (def != *(--profile.definitions.end()))
                        out << def << ", ";
                    else
                        out << def;
                }
                out << " ]," << std::endl;

                out << "    \"controlEdges\": [ ";
                for (auto edge : profile.controlEdges) {
                    if (edge != *(--profile.controlEdges.end()))
                        out << "[" << edge.first << ", " << edge.second << "], ";
                    else
                        out << "[" << edge.first << ", " << edge.second << "]";
                }
                out << " ]" << std::endl;
                out << "}";
            }

        return out;
    }
};

class SrcSlicePolicy
        : public srcSAXEventDispatch::EventListener,
          public srcSAXEventDispatch::PolicyDispatcher,
          public srcSAXEventDispatch::PolicyListener {
public:
    ~SrcSlicePolicy() {};
    std::unordered_map<std::string, std::vector<SliceProfile>> *profileMap;

    SrcSlicePolicy(std::unordered_map<std::string, std::vector<SliceProfile>> *pm,
                   std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {})
            : srcSAXEventDispatch::PolicyDispatcher(listeners) {
        // making SSP a listener for FSPP
        InitializeEventHandlers();

        declPolicy.AddListener(this);
        exprPolicy.AddListener(this);
        callPolicy.AddListener(this);
        initPolicy.AddListener(this);
        paramPolicy.AddListener(this);
        functionPolicy.AddListener(this);

        profileMap = pm;
    }

    void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
        using namespace srcSAXEventDispatch;
        if (typeid(DeclTypePolicy) == typeid(*policy)) {
            decldata = *policy->Data<DeclData>();
            auto sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);

            // Dumps out the variable names of variables
            // declared in a function body :: main(), ...

            //Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap->end()) {
                auto sliceProfile = SliceProfile(decldata.nameOfIdentifier, decldata.lineNumber,
                                                 (decldata.isPointer || decldata.isReference), true,
                                                 std::set<unsigned int>{decldata.lineNumber});
                sliceProfile.nameOfContainingClass = ctx.currentClassName;
                sliceProfileItr->second.push_back(sliceProfile);
                sliceProfileItr->second.back().containsDeclaration = true;
            } else {
                auto sliceProf = SliceProfile(decldata.nameOfIdentifier, decldata.lineNumber,
                                              (decldata.isPointer || decldata.isReference), false,
                                              std::set<unsigned int>{decldata.lineNumber});
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProf.containsDeclaration = true;
                profileMap->insert(std::make_pair(decldata.nameOfIdentifier,
                                                  std::vector<SliceProfile>{
                                                          std::move(sliceProf)
                                                  }));
            }
            sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);
            //look at the dvars and add this current variable to their dvar's lists. If we haven't seen this name before, add its slice profile
            for (std::string dvar : declDvars) {
                auto updateDvarAtThisLocation = profileMap->find(dvar);
                if (updateDvarAtThisLocation != profileMap->end()) {
                    if (!StringContainsCharacters(decldata.nameOfIdentifier)) continue;
                    if (sliceProfileItr != profileMap->end() && sliceProfileItr->second.back().potentialAlias) {
                        updateDvarAtThisLocation->second.back().aliases.insert(decldata.nameOfIdentifier);
                        continue;
                    }
                    updateDvarAtThisLocation->second.back().dvars.insert(decldata.nameOfIdentifier);
                } else {
                    auto sliceProf = SliceProfile(dvar, decldata.lineNumber, false, false, std::set<unsigned int>{},
                                                  std::set<unsigned int>{decldata.lineNumber});
                    sliceProf.nameOfContainingClass = ctx.currentClassName;
                    auto newSliceProfileFromDeclDvars = profileMap->insert(std::make_pair(dvar,
                                                                                          std::vector<SliceProfile>{
                                                                                                  std::move(sliceProf)
                                                                                          }));
                    if (!StringContainsCharacters(decldata.nameOfIdentifier)) continue;
                    if (sliceProfileItr != profileMap->end() && sliceProfileItr->second.back().potentialAlias) {
                        newSliceProfileFromDeclDvars.first->second.back().aliases.insert(decldata.nameOfIdentifier);
                        continue;
                    }
                    newSliceProfileFromDeclDvars.first->second.back().dvars.insert(decldata.nameOfIdentifier);
                }
            }

            // This allows me to set the data type of the variable in its slice
            // after its been set up from the logic above here
            // Set the data-type of sliceprofile for decl vars inside of function bodies

            sliceProfileItr->second.back().variableType = declPolicy.data.nameOfType;

            // Link the filepath the XML Originates
            sliceProfileItr->second.back().file = ctx.currentFilePath;
            
            // Link the function this slice is located in
            sliceProfileItr->second.back().function = ctx.currentFunctionName;

            declDvars.clear();
            decldata.clear();
        } else if (typeid(ExprPolicy) == typeid(*policy)) {
            exprDataSet = *policy->Data<ExprPolicy::ExprDataSet>();
            //iterate through every token found in the expression statement
            for (auto exprdata : exprDataSet.dataSet) {
                auto sliceProfileExprItr = profileMap->find(exprdata.second.nameOfIdentifier);
                auto sliceProfileLHSItr = profileMap->find(exprDataSet.lhsName);
                //Just update definitions and uses if name already exists. Otherwise, add new name.
                if (sliceProfileExprItr != profileMap->end()) {
                    sliceProfileExprItr->second.back().nameOfContainingClass = ctx.currentClassName;
                    sliceProfileExprItr->second.back().uses.insert(exprdata.second.uses.begin(),
                                                                   exprdata.second.uses.end());
                    sliceProfileExprItr->second.back().definitions.insert(exprdata.second.definitions.begin(),
                                                                          exprdata.second.definitions.end());

                    if (!StringContainsCharacters(exprDataSet.lhsName)) continue;
                    if (sliceProfileLHSItr != profileMap->end() && sliceProfileLHSItr->second.back().potentialAlias) {
                        sliceProfileExprItr->second.back().aliases.insert(exprDataSet.lhsName);
                        continue;
                    }
                    if (!StringContainsCharacters(currentName)) continue;
                    if (!currentName.empty() &&
                        (exprdata.second.lhs || currentName != exprdata.second.nameOfIdentifier)) {
                        sliceProfileExprItr->second.back().dvars.insert(currentName);
                        continue;
                    }

                } else {
                    auto sliceProfileExprItr2 = profileMap->insert(std::make_pair(exprdata.second.nameOfIdentifier,
                                                                                  std::vector<SliceProfile>{
                                                                                          SliceProfile(
                                                                                                  exprdata.second.nameOfIdentifier,
                                                                                                  ctx.currentLineNumber,
                                                                                                  false, false,
                                                                                                  exprdata.second.definitions,
                                                                                                  exprdata.second.uses)
                                                                                  }));
                    sliceProfileExprItr2.first->second.back().nameOfContainingClass = ctx.currentClassName;

                    if (!StringContainsCharacters(exprDataSet.lhsName)) continue;
                    if (sliceProfileLHSItr != profileMap->end() && sliceProfileLHSItr->second.back().potentialAlias) {
                        sliceProfileExprItr2.first->second.back().aliases.insert(exprDataSet.lhsName);
                        continue;
                    }
                    //Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                    if (!StringContainsCharacters(currentName)) continue;
                    if (!currentName.empty() &&
                        (exprdata.second.lhs || currentName != exprdata.second.nameOfIdentifier)) {
                        sliceProfileExprItr2.first->second.back().dvars.insert(currentName);
                        continue;
                    }
                }
            }
            exprDataSet.clear();
        } else if (typeid(InitPolicy) == typeid(*policy)) {
            initDataSet = *policy->Data<InitPolicy::InitDataSet>();
            //iterate through every token found in the initialization of a decl_stmt
            for (auto initdata : initDataSet.dataSet) {
                declDvars.push_back(initdata.second.nameOfIdentifier);
                auto sliceProfileItr = profileMap->find(initdata.second.nameOfIdentifier);
                //Just update definitions and uses if name already exists. Otherwise, add new name.
                if (sliceProfileItr != profileMap->end()) {
                    sliceProfileItr->second.back().uses.insert(initdata.second.uses.begin(),
                                                               initdata.second.uses.end());
                } else {
                    auto sliceProf = SliceProfile(initdata.second.nameOfIdentifier, ctx.currentLineNumber, false, false,
                                                  std::set<unsigned int>{}, initdata.second.uses);
                    sliceProf.nameOfContainingClass = ctx.currentClassName;
                    profileMap->insert(std::make_pair(initdata.second.nameOfIdentifier,
                                                      std::vector<SliceProfile>{sliceProf}));
                }
            }
            initDataSet.clear();
        } else if (typeid(CallPolicy) == typeid(*policy)) {
            calldata = *policy->Data<CallPolicy::CallData>();
            bool isFuncNameNext = false;
            std::vector<std::pair<std::string, unsigned int>> funcNameAndCurrArgumentPos;
            //Go through each token found in a function call
            for (auto currentCallToken : calldata.callargumentlist) {
                //Check to see if we are entering a function call or exiting--
                //if entering, we know the next token is the name of the call
                //otherwise, we're exiting and need to pop the current function call off the stack
                switch (currentCallToken[0]) {
                    case '(': {
                        isFuncNameNext = true;
                        continue;
                    }
                    case ')': {
                        if (!funcNameAndCurrArgumentPos.empty()) funcNameAndCurrArgumentPos.pop_back();
                        continue;
                    }
                }
                //If we noted that a function name was coming in that switch above, record it here.
                //Otherwise, the next token is an argument in the function call
                if (isFuncNameNext) {
                    funcNameAndCurrArgumentPos.push_back(std::make_pair(currentCallToken, 1));
                    isFuncNameNext = false;
                } else {
                    auto sliceProfileItr = profileMap->find(currentCallToken);

                    std::string callOrder, argumentOrder;
                    for (auto name : funcNameAndCurrArgumentPos) {
                        if (!StringContainsCharacters(name.first)) continue;
                        callOrder += name.first + '-';
                        argumentOrder += std::to_string(name.second) + '-';
                    }
                    if (!callOrder.empty())callOrder.erase(callOrder.size() - 1); ///need to implement join
                    if (!argumentOrder.empty()) argumentOrder.erase(argumentOrder.size() - 1); ///need to implement join

                    //Just update cfunctions if name already exists. Otherwise, add new name.
                    if (sliceProfileItr != profileMap->end()) {
                        sliceProfileItr->second.back().cfunctions.push_back(std::make_pair(callOrder, argumentOrder));
                    } else {
                        auto sliceProf = SliceProfile(currentCallToken, ctx.currentLineNumber, true, true,
                                                      std::set<unsigned int>{},
                                                      std::set<unsigned int>{ctx.currentLineNumber},
                                                      std::vector<std::pair<std::string, std::string>>{
                                                              std::make_pair(callOrder, argumentOrder)});
                        sliceProf.nameOfContainingClass = ctx.currentClassName;
                        profileMap->insert(std::make_pair(currentCallToken,
                                                          std::vector<SliceProfile>{sliceProf}));
                    }
                    if (!funcNameAndCurrArgumentPos.empty()) ++funcNameAndCurrArgumentPos.back().second;
                }
            }
        } else if (typeid(ParamTypePolicy) == typeid(*policy)) {
            paramdata = *policy->Data<DeclData>();
            
            //record parameter data-- this is done exact as it is done for decl_stmts except there's no initializer
            auto sliceProfileItr = profileMap->find(paramdata.nameOfIdentifier);
            //Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap->end()) {
                auto sliceProf = SliceProfile(paramdata.nameOfIdentifier, paramdata.lineNumber,
                                              (paramdata.isPointer || paramdata.isReference), true,
                                              std::set<unsigned int>{paramdata.lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProfileItr->second.push_back(std::move(sliceProf));
            } else {
                auto sliceProf = SliceProfile(paramdata.nameOfIdentifier, paramdata.lineNumber,
                                              (paramdata.isPointer || paramdata.isReference), true,
                                              std::set<unsigned int>{paramdata.lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                profileMap->insert(std::make_pair(paramdata.nameOfIdentifier,
                                                  std::vector<SliceProfile>{std::move(sliceProf)}));
            }
            
            // Attempt to insert data-types for sliceprofiles found in function/ctor parameters
            profileMap->find(paramdata.nameOfIdentifier)->second.back().variableType = paramdata.nameOfType;

            // Link the filepath this slice is located in
            profileMap->find(paramdata.nameOfIdentifier)->second.back().file = ctx.currentFilePath;

            // Link the function the XML Originates from
            profileMap->find(paramdata.nameOfIdentifier)->second.back().function = ctx.currentFunctionName;

            paramdata.clear();
        } else if (typeid(FunctionSignaturePolicy) == typeid(*policy)) {
            functionsigdata = *policy->Data<SignatureData>();
            functionSigMap.insert(
                    std::make_pair(functionsigdata.name,
                                   functionsigdata));
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx) {}
    
    auto ArgumentProfile(std::pair<std::string, SignatureData> func, int paramIndex, std::unordered_set<std::string> visit_func) {
	auto Spi = profileMap->find(func.second.parameters.at(paramIndex).nameOfIdentifier);
        for (auto param : func.second.parameters) {
            
            
            if (profileMap->find(param.nameOfIdentifier)->second.back().visited) {
                return Spi;
            } else {
                for (auto cfunc : profileMap->find(param.nameOfIdentifier)->second.back().cfunctions) {
                    if (cfunc.first.compare(func.first) != 0) {
                        auto function = functionSigMap.find(cfunc.first);
                        if (function != functionSigMap.end()) {
                            if (cfunc.first.compare(function->first) == 0 && visit_func.find(cfunc.first) == visit_func.end()) {
				                visit_func.insert(cfunc.first);
                                auto recursiveSpi = ArgumentProfile(*function, std::atoi(cfunc.second.c_str()) - 1, visit_func);
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

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;
	for (std::pair<std::string, std::vector<SliceProfile>> var : *profileMap) {
            if (!profileMap->find(var.first)->second.back().visited) {
                if (!var.second.back().cfunctions.empty()) {
                    for (auto cfunc : var.second.back().cfunctions) {
                        auto funcIt = functionSigMap.find(cfunc.first);
                        if(funcIt != functionSigMap.end()) {
                            if (cfunc.first.compare(funcIt->first) == 0) { //TODO fix for case: Overload
                                auto Spi = ArgumentProfile(*funcIt, std::atoi(cfunc.second.c_str()) - 1, visited_func);
                                if (profileMap->find(var.first) != profileMap->end() &&
                                    profileMap->find(Spi->first) != profileMap->end()) {
                                    profileMap->find(var.first)->second.back().definitions.insert(
                                            Spi->second.back().definitions.begin(),
                                            Spi->second.back().definitions.end());
                                    profileMap->find(var.first)->second.back().uses.insert(
                                            Spi->second.back().uses.begin(),
                                            Spi->second.back().uses.end());
                                    profileMap->find(var.first)->second.back().cfunctions.insert(
                                            profileMap->find(var.first)->second.back().cfunctions.begin(),
                                            Spi->second.back().cfunctions.begin(),
                                            Spi->second.back().cfunctions.end());
                                    profileMap->find(var.first)->second.back().aliases.insert(
                                            Spi->second.back().aliases.begin(),
                                            Spi->second.back().aliases.end());
                                    profileMap->find(var.first)->second.back().dvars.insert(
                                            Spi->second.back().dvars.begin(),
                                            Spi->second.back().dvars.end());
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
                        profileMap->find(var.first)->second.back().controlEdges.insert(
                                std::make_pair(predecessor, trueSuccessor));
                    }
                    profileMap->find(var.first)->second.back().controlEdges.insert(
                            std::make_pair(predecessor, falseSuccessor));
                }
            }
            int prevSL = 0;
            for (int i = 0; i < sLines.size(); i++) {
                if (i + 1 < sLines.size()) {
                    bool outIf = true;
                    bool outElse = true;
                    for (auto ifblock : ifdata) {
                        if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                            outIf = false;
                            break;
                        }
                    }
                    if (!outIf) {
                        for (auto elseblock : elsedata) {
                            if (sLines[i + 1] >= elseblock.first && sLines[i + 1] <= elseblock.second) {
                                outElse = false;
                                break;
                            }
                        }
                    }
                    if ((outIf || outElse) && sLines[i] != sLines[i + 1]) {
                        profileMap->find(var.first)->second.back().controlEdges.insert(
                                std::make_pair(sLines[i], sLines[i + 1]));
                    }
                }
                bool outControlBlock = true;
                for (auto loop : loopdata) {
                    if (sLines[i] >= loop.first && sLines[i] <= loop.second) {
                        outControlBlock = false;
                        break;
                    }
                }
                if (outControlBlock) {
                    for (auto ifblock : ifdata) {
                        if (sLines[i] >= ifblock.first && sLines[i] <= ifblock.second) {
                            outControlBlock = false;
                            break;
                        }
                    }
                }
                if (outControlBlock) {
                    for (auto elseblock : elsedata) {
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
                        profileMap->find(var.first)->second.back().controlEdges.insert(
                                std::make_pair(prevSL, sLines[i]));
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
    DeclTypePolicy declPolicy;
    DeclData decldata;

    ParamTypePolicy paramPolicy;
    DeclData paramdata;

    InitPolicy initPolicy;
    InitPolicy::InitDataSet initDataSet;

    ExprPolicy::ExprDataSet exprDataSet;
    ExprPolicy exprPolicy;

    CallPolicy callPolicy;
    CallPolicy::CallData calldata;

    FunctionSignaturePolicy functionPolicy;
    SignatureData functionsigdata;
    std::map<std::string, SignatureData> functionSigMap;
    std::string currentExprName;
    std::vector<std::string> declDvars;

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elsedata;
    int startLine;
    int endLine;

    std::string currentName;

    void InitializeEventHandlers() {
        using namespace srcSAXEventDispatch;
        openEventMap[ParserState::declstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&declPolicy);
        };
        closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&declPolicy);
            currentName.clear();
        };

        openEventMap[ParserState::parameterlist] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&paramPolicy);
        };
        closeEventMap[ParserState::parameterlist] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&paramPolicy);
        };

        openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&exprPolicy);
        };
        closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&exprPolicy);
            currentName.clear();
        };

        openEventMap[ParserState::call] = [this](srcSAXEventContext &ctx) {
            //don't want multiple callPolicy parsers running
            if (ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                ctx.dispatcher->AddListenerDispatch(&callPolicy);
            }
        };
        closeEventMap[ParserState::call] = [this](srcSAXEventContext &ctx) {
            if (ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                ctx.dispatcher->RemoveListenerDispatch(&callPolicy);
            }
        };

        openEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&initPolicy);
        };
        closeEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&initPolicy);
        };

        openEventMap[ParserState::forstmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::forstmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            loopdata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::whilestmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::whilestmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            loopdata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::ifstmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::ifstmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            ifdata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::elseif] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::elseif] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            elsedata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::elsestmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::elsestmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            elsedata.push_back(std::make_pair(startLine, endLine));
        };

        closeEventMap[ParserState::op] = [this](srcSAXEventContext &ctx) {
            if (ctx.currentToken == "=") {
                currentName = currentExprName;
            }
        };

        openEventMap[ParserState::function] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&functionPolicy);
        };

        openEventMap[ParserState::functionblock] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
        };

        closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext &ctx) {
            //TODO: possibly, this if-statement is suppressing more than just unmarked whitespace. Investigate.
            if (!(ctx.currentToken.empty() || ctx.currentToken == " ")) {
                if (ctx.And({ParserState::name, ParserState::expr, ParserState::exprstmt}) &&
                    ctx.Nor({ParserState::specifier, ParserState::modifier, ParserState::op})) {
                    currentExprName = ctx.currentToken;
                }
            }
        };

        closeEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
            for (std::unordered_map<std::string, std::vector<SliceProfile>>::iterator it = profileMap->begin();
                 it != profileMap->end(); ++it) {
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
            ComputeControlPaths();
            ComputeInterprocedural();
        };
    }
};


#endif
