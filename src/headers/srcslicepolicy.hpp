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
#include <fstream>
#include <ReturnPolicy.hpp>
#include <ConditionalPolicy.hpp>
#include <sstream>

bool StringContainsCharacters(const std::string &str);

class SliceProfile {
public:
    SliceProfile() : index(0), containsDeclaration(false), potentialAlias(false), dereferenced(false),
                     isGlobal(false) { jsonOut = false; isPointer = false; isReference = false; }

    SliceProfile(
            std::string name, int line, bool alias = 0, bool global = 0,
            std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {},
            std::vector<std::pair<std::string, std::pair<std::string, std::string>>> cFunc = {},
            std::set<std::pair<std::string, unsigned int>> dv = {}, bool containsDecl = false,
            std::set<std::pair<int, int>> edges = {}, bool visit = false) :
            variableName(name), lineNumber(line), potentialAlias(alias),
            isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc),
            dvars(dv), containsDeclaration(containsDecl), controlEdges(edges),
            visited(visit) {
        jsonOut = false;
        dereferenced = false;
        isPointer = false;
        isReference = false;
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

    bool isPointer = false, isReference = false;

    std::string variableName;
    std::string variableType;
    std::unordered_set<std::string> memberVariables;

    std::set<unsigned int> definitions;
    std::set<unsigned int> uses;

    std::set<std::pair<std::string, unsigned int>> dvars;
    std::set<std::pair<std::string, unsigned int>> aliases;

    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> cfunctions;

    std::set<std::pair<int, int>> controlEdges;

    bool visited;
    bool jsonOut;

    bool returnUsesInserted = false;
    bool conditionalUsesInserted = false;
    bool conditionalDefsInserted = false;

    size_t sliceIndex;

    void SetJsonOut(bool b) { jsonOut = b; }

    void SetSliceIndex(size_t ind) { sliceIndex = ind; }

    friend std::ostream& operator<<(std::ostream& out, SliceProfile& profile) {
        if (!profile.jsonOut)
        {
            std::cout << "[-] Sorry, plain-text output unavailable." << std::endl;
            /*
            out << "==========================================================================" << std::endl;
            out << "File: " << profile.file << std::endl << "Function: " << profile.function << std::endl;
            out << "Name: " << profile.variableName << "\nType: " << profile.variableType << std::endl;
            out << "Class: " << profile.nameOfContainingClass << std::endl;
            out << "Dependent Variables: {";
            for (auto dvar : profile.dvars) {
                if (dvar != *(--profile.dvars.end()))
                    out << dvar << ",";
                else
                    out << dvar;
            }
            out << "}" << std::endl;
            out << "Aliases: {";
            for (auto alias : profile.aliases) {
                if (alias != *(--profile.aliases.end()))
                    out << alias.first << "|" << alias.second << ", ";
                else
                    out << alias.first << "|" << alias.second;
            }
            out << "}" << std::endl;
            out << "Called Functions: {";
            for (auto cfunc : profile.cfunctions) {
                if (cfunc != *(--profile.cfunctions.end()))
                    out << cfunc.first << " " << cfunc.second << ",";
                else
                    out << cfunc.first << " " << cfunc.second;
            }
            out << "}" << std::endl;
            out << "Use: {";
            for (auto use : profile.uses) {
                if (use != *(--profile.uses.end()))
                    out << use << ",";
                else
                    out << use;
            }
            out << "}" << std::endl;
            out << "Definition: {";
            for (auto def : profile.definitions) {
                if (def != *(--profile.definitions.end()))
                    out << def << ",";
                else
                    out << def;
            }
            out << "}" << std::endl;
            // out << "Control Edges: {";
            // for (auto edge : profile.controlEdges) {
            //     if (edge != *(--profile.controlEdges.end()))
            //         out << "(" << edge.first << ", " << edge.second << ")" << ",";
            //     else
            //         out << "(" << edge.first << ", " << edge.second << ")";
            // }
            // out << "}" << std::endl;
            out << "==========================================================================" << std::endl;
            */
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
                        out << "\"" << dvar.first << "\", ";
                    else
                        out << "\"" << dvar.first << "\"";
                }
                out << " ]," << std::endl;

                out << "    \"aliases\": [ ";
                for (auto alias : profile.aliases) {
                    if (alias != *(--profile.aliases.end()))
                        out << "{ \"" << alias.first << "\": " << alias.second << " },";
                    else
                        out << "{ \"" << alias.first << "\": " << alias.second << " }";
                }
                out << " ]," << std::endl;

                out << "    \"calledFunctions\": [ ";
                for (auto cfunc : profile.cfunctions) {
                    if (cfunc != *(--profile.cfunctions.end()))
                        out << "{\"functionName\": \"" << cfunc.first.substr(0, cfunc.first.find('-')) << "\", \"parameterNumber\": \"" << cfunc.second.first << "\", \"definitionLine\": \"" << cfunc.second.second << "\"}, ";
                    else
                        out << "{\"functionName\": \"" << cfunc.first.substr(0, cfunc.first.find('-')) << "\", \"parameterNumber\": \"" << cfunc.second.first << "\", \"definitionLine\": \"" << cfunc.second.second << "\"}";
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
                out << " ]" << std::endl;

                // out << "    \"controlEdges\": [ ";
                // for (auto edge : profile.controlEdges) {
                //     if (edge != *(--profile.controlEdges.end()))
                //         out << "[" << edge.first << ", " << edge.second << "], ";
                //     else
                //         out << "[" << edge.first << ", " << edge.second << "]";
                // }
                // out << " ]" << std::endl;
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
        functionPolicy.AddListener(this);
        returnPolicy.AddListener(this);
        conditionalPolicy.AddListener(this);

        profileMap = pm;
    }

    void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
        using namespace srcSAXEventDispatch;
        if (typeid(DeclTypePolicy) == typeid(*policy)) {
            decldata = *policy->Data<DeclData>();
            auto sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);

            // Dumps out the variable names of variables
            // declared in a function body :: main(), ...

            initDeclData.push_back(std::make_pair(decldata.nameOfIdentifier, decldata.lineNumber));

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

            // Do not remove, it will cause a segmentation fault
            sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);

            sliceProfileItr->second.back().isReference = decldata.isReference;
            sliceProfileItr->second.back().isPointer = decldata.isPointer;

            //look at the dvars and add this current variable to their dvar's lists. If we haven't seen this name before, add its slice profile
            for (std::string dvar : declDvars) {
                auto updateDvarAtThisLocation = profileMap->find(dvar);
                if (updateDvarAtThisLocation != profileMap->end()) {
                    if (!StringContainsCharacters(decldata.nameOfIdentifier)) continue;
                    if (sliceProfileItr != profileMap->end() && sliceProfileItr->second.back().potentialAlias) {
                        if ( decldata.nameOfIdentifier != sliceProfileItr->second.back().variableName) {
                            updateDvarAtThisLocation->second.back().aliases.insert(std::make_pair(decldata.nameOfIdentifier, ctx.currentLineNumber));
                        }
                        continue;
                    }
                    updateDvarAtThisLocation->second.back().dvars.insert(std::make_pair(decldata.nameOfIdentifier, ctx.currentLineNumber));
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
                        if ( decldata.nameOfIdentifier != sliceProfileItr->second.back().variableName ) {
                            newSliceProfileFromDeclDvars.first->second.back().aliases.insert(std::make_pair(decldata.nameOfIdentifier, ctx.currentLineNumber));
                        }
                        continue;
                    }
                    newSliceProfileFromDeclDvars.first->second.back().dvars.insert(std::make_pair(decldata.nameOfIdentifier, ctx.currentLineNumber));
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
                        if ( exprDataSet.lhsName != sliceProfileExprItr->second.back().variableName ) {
                            sliceProfileExprItr->second.back().aliases.insert(std::make_pair(exprDataSet.lhsName, ctx.currentLineNumber));
                        }
                        continue;
                    }

                    // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                    // IE : abc = abc + i;
                    // if (!StringContainsCharacters(currentName)) continue;
                    // if (!currentName.empty() &&
                    //     (exprdata.second.lhs || currentName != exprdata.second.nameOfIdentifier)) {
                    //     sliceProfileExprItr->second.back().dvars.insert(currentName);
                    //     continue;
                    // }

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
                        if ( exprDataSet.lhsName != sliceProfileLHSItr->second.back().variableName ) {
                            sliceProfileExprItr2.first->second.back().aliases.insert(std::make_pair(exprDataSet.lhsName, ctx.currentLineNumber));
                        }
                        continue;
                    }

                    // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                    // IE : abc = abc + i;
                    // if (!StringContainsCharacters(currentName)) continue;
                    // if (!currentName.empty() &&
                    //     (exprdata.second.lhs || currentName != exprdata.second.nameOfIdentifier)) {
                    //     sliceProfileExprItr2.first->second.back().dvars.insert(currentName);
                    //     continue;
                    // }
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
            // Runs when function calls are detected

            functionCallList.insert(ctx.currentLineNumber);

            calldata = *policy->Data<CallPolicy::CallData>();
            // This loop is for simply tracking how many args are within a loaded function call
            int trueArgCount = 0;
            bool psuedoBool = false;
            for (auto currentCallToken : calldata.callargumentlist) {
                switch (currentCallToken[0]) {
                    case '(': {
                        psuedoBool = true;
                        continue;
                    }
                    case ')': {
                        continue;
                    }
                }
                if (psuedoBool) {
                    psuedoBool = false;
                } else {
                    ++trueArgCount;
                }
            }

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
                    if (!callOrder.empty())callOrder.erase(callOrder.size() - 1); // need to implement join
                    if (!argumentOrder.empty()) argumentOrder.erase(argumentOrder.size() - 1); // need to implement join

                    int isMemberFunctCall = callOrder.find('.');
                    int functionDefinitionLine = 0;

                    // Convert the unordered_map into a ordered map
                    std::map<std::string, SignatureData> orderedFunctionSigMap;
                    orderedFunctionSigMap.insert(functionSigMap.begin(), functionSigMap.end());

                    // class member functions have a '.' within callOrder variable
                    if (isMemberFunctCall == -1) {
                        // These are free-functions

                        // Find the first instance of the callOrder within functionSigMap
                        auto functSigComponent = orderedFunctionSigMap.find(callOrder);

                        if (functSigComponent != orderedFunctionSigMap.end()) {
                            std::string functSigName = functSigComponent->first;
                            functionDefinitionLine = functSigComponent->second.lineNumber;

                            // iterate all possible overload orderedFunctionSigMap instances that
                            // represent overload functions collected
                            for (auto& funcSig = functSigComponent; funcSig != orderedFunctionSigMap.end(); ++funcSig) {
                                if (functSigName == callOrder) {

                                    // rename functSigName only if a ID tag is included in the string
                                    if (funcSig->first.find('-') != -1) {
                                        functSigName = funcSig->first.substr(0, funcSig->first.find('-'));
                                        functionDefinitionLine = funcSig->second.lineNumber;
                                    }

                                    // Find a Match between functSigMap.second.parameters.size() and trueArgCount
                                    if (funcSig->second.parameters.size() == trueArgCount && funcSig->second.nameOfContainingClass.empty()) {
                                        callOrder = funcSig->first;
                                        break;
                                    }
                                }
                            }
                        }
                    } else
                        {
                            // parse out the '.' and everything else before it
                            std::string targetFunction = callOrder.substr(callOrder.find('.') + 1);
                            std::string classScope = "";

                            // get function definition line for member function being targeted
                            auto functSigComponent = orderedFunctionSigMap.find(targetFunction);

                            if (functSigComponent != orderedFunctionSigMap.end()) {
                                functionDefinitionLine = functSigComponent->second.lineNumber;
                                classScope = functSigComponent->second.nameOfContainingClass;

                                // Collect function name from overloads recorded
                                std::string functSigName = functSigComponent->first;
                                if (functSigComponent->first.find('-') != -1) {
                                    functSigName = functSigComponent->first.substr(0, functSigComponent->first.find('-'));
                                }

                                // iterate all possible overload functionSigMap instances that
                                // represent overload functions collected
                                for (auto& funcSig = functSigComponent; funcSig != orderedFunctionSigMap.end(); ++funcSig) {
                                    if (targetFunction == functSigName) {
                                        // rename functionName only if a ID tag is included in the string
                                        if (funcSig->first.find('-') != -1) {
                                            functSigName = funcSig->first.substr(0, funcSig->first.find('-'));
                                            functionDefinitionLine = funcSig->second.lineNumber;
                                            classScope = funcSig->second.nameOfContainingClass;
                                        }

                                        // Find a Match between functSigMap.second.parameters.size() and trueArgCount along with using class scoping
                                        if (funcSig->second.parameters.size() == trueArgCount && classScope == funcSig->second.nameOfContainingClass) {
                                            callOrder = funcSig->first;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                    //Just update cfunctions if name already exists. Otherwise, add new name.
                    if (sliceProfileItr != profileMap->end()) {
                        if (sliceProfileItr->second.back().cfunctions.size() > 0) {
                            // Dont insert the same set multiple times into the vector
                            auto newCalledFunctItr = std::find(sliceProfileItr->second.back().cfunctions.begin(),
                                                            sliceProfileItr->second.back().cfunctions.end(),
                                                            std::make_pair(callOrder, std::make_pair(argumentOrder, std::to_string(functionDefinitionLine))));

                            if ( newCalledFunctItr == sliceProfileItr->second.back().cfunctions.end() ) {
                                sliceProfileItr->second.back().cfunctions.push_back(std::make_pair(callOrder, std::make_pair(argumentOrder, std::to_string(functionDefinitionLine))));
                            }
                        } else
                        {
                            sliceProfileItr->second.back().cfunctions.push_back(std::make_pair(callOrder, std::make_pair(argumentOrder, std::to_string(functionDefinitionLine))));
                        }
                    } else {
                        auto sliceProf = SliceProfile(currentCallToken, ctx.currentLineNumber, true, true,
                                                      std::set<unsigned int>{},
                                                      std::set<unsigned int>{ctx.currentLineNumber},
                                                      std::vector<std::pair<std::string, std::pair<std::string, std::string>>>{
                                                              std::make_pair(callOrder, std::make_pair(argumentOrder, std::to_string(functionDefinitionLine)))});
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

            if (functionSigMap.find(functionsigdata.name) != functionSigMap.end() && !functionsigdata.name.empty()) {
                // overloaded function detected

                // construct a new name to log the overloaded function under
                std::string functName = functionsigdata.name;
                unsigned int overloadID = ++overloadFunctionCount[functionsigdata.name];
                functName += "-" + std::to_string(overloadID);

                functionSigMap.insert(
                        std::make_pair(functName, functionsigdata)
                        );
            } else
            {
                functionSigMap.insert(
                        std::make_pair(functionsigdata.name, functionsigdata)
                        );
                overloadFunctionCount[functionsigdata.name] = 0;
            }
        } else if (typeid(ReturnPolicy) == typeid(*policy)) {
            for (auto dataSet : *returnPolicy.GetReturnUses()) {
                auto sliceProfileItr = profileMap->find(dataSet.first);
                
                // incase we have multiple slices of the same name under the hood
                // we determine if we have the right slice by checking its name
                // and whether we've already inserted data into it
                while (sliceProfileItr != profileMap->end()) {
                    if (sliceProfileItr->second.back().containsDeclaration &&
                        sliceProfileItr->second.back().variableName == dataSet.first &&
                        !sliceProfileItr->second.back().returnUsesInserted) {
                        for (auto useLines : dataSet.second) {
                            sliceProfileItr->second.back().uses.insert(useLines);
                        }
                        sliceProfileItr->second.back().returnUsesInserted = true;
                    }

                    ++sliceProfileItr;
                }
            }

            returnPolicy.ClearCollection();
        } else if (typeid(ConditionalPolicy) == typeid(*policy)) {
            std::set<std::string> insertTargets;
            for (auto dataSet : *conditionalPolicy.GetConditionalUses()) {
                auto sliceProfileItr = profileMap->find(dataSet.first);
                
                // incase we have multiple slices of the same name under the hood
                // we determine if we have the right slice by checking its name
                // and whether we've already inserted data into it
                while (sliceProfileItr != profileMap->end()) {
                    if (sliceProfileItr->second.back().containsDeclaration &&
                        sliceProfileItr->second.back().variableName == dataSet.first &&
                        !sliceProfileItr->second.back().conditionalUsesInserted) {
                        for (auto useLines : dataSet.second) {
                            sliceProfileItr->second.back().uses.insert(useLines);
                        }
                        sliceProfileItr->second.back().conditionalUsesInserted = true;
                        insertTargets.insert(dataSet.first);
                    }

                    ++sliceProfileItr;
                }
            }
            for (auto name : insertTargets) {
                conditionalPolicy.DeleteUsesCollection(name);
            }
            insertTargets.clear();

            for (auto dataSet : *conditionalPolicy.GetConditionalDefs()) {
                auto sliceProfileItr = profileMap->find(dataSet.first);
                
                // incase we have multiple slices of the same name under the hood
                // we determine if we have the right slice by checking its name
                // and whether we've already inserted data into it
                while (sliceProfileItr != profileMap->end()) {
                    if (sliceProfileItr->second.back().containsDeclaration &&
                        sliceProfileItr->second.back().variableName == dataSet.first &&
                        !sliceProfileItr->second.back().conditionalDefsInserted) {
                        for (auto defLines : dataSet.second) {
                            sliceProfileItr->second.back().definitions.insert(defLines);
                        }
                        sliceProfileItr->second.back().conditionalDefsInserted = true;
                        insertTargets.insert(dataSet.first);
                    }

                    ++sliceProfileItr;
                }
            }
            for (auto name : insertTargets) {
                conditionalPolicy.DeleteDefsCollection(name);
            }
            insertTargets.clear();
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx) {}
    
    auto ArgumentProfile(std::pair<std::string, SignatureData> func, int paramIndex, std::unordered_set<std::string> visit_func) {
	    auto Spi = profileMap->find(func.second.parameters.at(paramIndex).nameOfIdentifier);

        // Ensure the key exists in the map
        std::string functionName = func.first;
        if (funcDefMap.find(functionName) == funcDefMap.end()) {
            funcDefMap[functionName] = std::vector<unsigned int>();
        }

        // Check for Duplicates before pushing a line number
        auto& lineVector = funcDefMap[functionName];
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
                        auto function = functionSigMap.find(cfunc.first);
                        if (function != functionSigMap.end()) {
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
        for (auto initDeclItem : initDeclData) {
            // With a collection of data concerning where all variables are initially declared
            // check if the sliceProfile passed matches with a variable name within the collection
            if (sliceProfile.variableName != initDeclItem.first) continue;

            // Verify the correct slice by checking if the slice definition contains
            // the initial decl line number for the slice with the matching name
            if (sliceProfile.definitions.find(initDeclItem.second) == sliceProfile.definitions.end()) continue;

            // Store iterators of the functionSigMap in a vector
            std::vector<std::unordered_map<std::string, SignatureData>::iterator> funcSigMapItrs;
            for (auto it = functionSigMap.begin(); it != functionSigMap.end(); ++it) {
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
                        for (auto data : *conditionalPolicy.GetSwitchUses()) {
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

                        for (auto data : *conditionalPolicy.GetSwitchDefs()) {
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
                        for (auto data : *conditionalPolicy.GetSwitchUses()) {
                            if (data.first != initDeclItem.first) continue;
                            for (auto nums : data.second) {
                                if (nums < initDeclItem.second) continue;
                                sliceProfile.uses.insert(nums);
                            }
                        }

                        for (auto data : *conditionalPolicy.GetSwitchDefs()) {
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

    void PassOver() {
        // Create a set of data representing function scopes
        // in ascending order from line number
        std::map<std::string, unsigned int> functionBounds;
        for (auto funcSig : functionSigMap) {
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

                    for (auto line : *declPolicy.GetPossibleDefs()) {
                        // within the slice does the line exist within the def set
                        if ( sliceItr->definitions.find(line) != sliceItr->definitions.end() ) {
                            // Check the cfunctions to see if defs need switched
                            for (auto cfunctData : sliceItr->cfunctions) {
                                std::string name = cfunctData.first;
                                
                                auto funct = funcDefMap.find(name);
                                if (funct != funcDefMap.end()) {
                                    auto Spi = ArgumentProfile(*functionSigMap.find(name), std::atoi(cfunctData.second.first.c_str()) - 1, junkMap);
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
                        auto funct = functionSigMap.find(name);
                        if (funct != functionSigMap.end()) {
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
                                            for (auto initDeclItem : initDeclData) {
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

                        if (functionCallList.find(alias->second) != functionCallList.end()) {
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
                                // definte the rough estimated end of local scope
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

                        if (functionCallList.find(dvar->second) != functionCallList.end()) {
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
                                unsigned int endOfLocalScope = 0;
                                for (auto data : functionBounds) {
                                    if (data.second > localScopeStart) {
                                        endOfLocalScope = data.second;
                                        break;
                                    }
                                }

                                // if the alias is formed farther down then the
                                // end of the local scope remove this alias
                                if (endOfLocalScope < dvar->second) {
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
    }

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;
	    for (std::pair<std::string, std::vector<SliceProfile>> var : *profileMap) {
            // Need to watch the Slices we attempt to dig into because we are collecting slices we have no interest in
            if (!profileMap->find(var.first)->second.back().visited && (var.second.back().variableName != "*LITERAL*")) {
                if (!var.second.back().cfunctions.empty()) {
                    for (auto cfunc : var.second.back().cfunctions) {
                        auto funcIt = functionSigMap.find(cfunc.first);
                        if(funcIt != functionSigMap.end()) {
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
                                        if (*(sliceItr->definitions.begin()) != std::stoi(cfunc.second.second)) {
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
                        if (sLines[i] != sLines[i + 1]) {
                            profileMap->find(var.first)->second.back().controlEdges.insert(
                                    std::make_pair(sLines[i], sLines[i + 1]));
                        }
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
    DeclTypePolicy declPolicy;
    DeclData decldata;

    DeclData paramdata;

    InitPolicy initPolicy;
    InitPolicy::InitDataSet initDataSet;

    ExprPolicy::ExprDataSet exprDataSet;
    ExprPolicy exprPolicy;

    CallPolicy callPolicy;
    CallPolicy::CallData calldata;
    
    ReturnPolicy returnPolicy;
    std::unordered_map<std::string, std::vector<unsigned int>> funcDefMap;
    
    ConditionalPolicy conditionalPolicy;

    FunctionSignaturePolicy functionPolicy;
    SignatureData functionsigdata;
    std::unordered_map<std::string, SignatureData> functionSigMap;
    std::string currentExprName;
    std::vector<std::string> declDvars;

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elsedata;
    std::vector<std::pair<std::string, unsigned int>> initDeclData;
    std::map<std::string, unsigned int> overloadFunctionCount;
    std::set<unsigned int> functionCallList;
    int startLine;
    int endLine;

    std::string currentName;

    void InitializeEventHandlers() {
        using namespace srcSAXEventDispatch;

        openEventMap[ParserState::decl] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&declPolicy);
        };
        closeEventMap[ParserState::decl] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListener(&declPolicy);
            declPolicy.Finalize(ctx);
            currentName.clear();
        };

        openEventMap[ParserState::name] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&returnPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::name] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&returnPolicy);
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&exprPolicy);
        };
        closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&exprPolicy);
            currentName.clear();
        };
        
        openEventMap[ParserState::expr] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::expr] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::switchstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
            conditionalPolicy.EditDepth(1);
        };
        closeEventMap[ParserState::switchstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
            conditionalPolicy.EditDepth(-1);
        };
        
        openEventMap[ParserState::switchcase] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::switchcase] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
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
            bool isAssignmentOperator = (ctx.currentToken == "=" || ctx.currentToken == "+=" ||
                                        ctx.currentToken == "-=" || ctx.currentToken == "*=" ||
                                        ctx.currentToken == "/=" || ctx.currentToken == "%=");
            if (isAssignmentOperator) {
                currentName = currentExprName;
            }
        };

        openEventMap[ParserState::function] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&functionPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::functionblock] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
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
                            auto functSigData = functionSigMap.find(cfunct.first);
                            if (functSigData != functionSigMap.end()) {
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
        };
    }
};


#endif