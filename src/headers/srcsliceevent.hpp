#ifndef SRCSLICEEVENT
#define SRCSLICEEVENT

#include <srcsliceprofile.hpp>
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

struct SliceEventData {
    SliceEventData() = default;
    
    // members of the struct
    std::unordered_map<std::string, std::vector<SliceProfile>>* pmPtr = nullptr;
    std::unordered_map<std::string, std::vector<unsigned int>> funcDefMap;
    SignatureData functionsigdata;
    std::unordered_map<std::string, SignatureData> functionSigMap;
    std::string currentExprName;
    std::vector<std::string> declDvars;

    std::vector<unsigned int>* possibleDefinitions = nullptr;
    std::vector<DvarData>* possibleDvars = nullptr;
    std::unordered_map<std::string, std::vector<unsigned int>>* switchUses = nullptr;
    std::unordered_map<std::string, std::vector<unsigned int>>* switchDefs = nullptr;

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elsedata;
    std::vector<std::pair<std::string, unsigned int>> initDeclData;
    std::map<std::string, unsigned int> overloadFunctionCount;
    std::set<unsigned int> functionCallList;
    int startLine;
    int endLine;

    std::string currentName;
};

bool StringContainsCharacters(const std::string &str);

class SrcSliceEvent
        : public srcSAXEventDispatch::EventListener,
          public srcSAXEventDispatch::PolicyDispatcher,
          public srcSAXEventDispatch::PolicyListener {
public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    SliceEventData sliceEventData;

    ~SrcSliceEvent() {};
    SrcSliceEvent(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {})
            : srcSAXEventDispatch::PolicyDispatcher(listeners),
            declPolicy({this}), exprPolicy({this}), callPolicy({this}), initPolicy({this}),
            functionPolicy({this}), returnPolicy({this}), conditionalPolicy({this}) {
        // making SSP a listener for FSPP
        InitializeEventHandlers();
    }

    void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
        using namespace srcSAXEventDispatch;
        if (typeid(DeclTypePolicy) == typeid(*policy)) {
            DeclData* decldata = policy->Data<DeclData>();
            auto sliceProfileItr = profileMap.find(decldata->nameOfIdentifier);

            // Dumps out the variable names of variables
            // declared in a function body :: main(), ...

            sliceEventData.initDeclData.push_back(std::make_pair(decldata->nameOfIdentifier, decldata->lineNumber));

            //Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap.end()) {
                auto sliceProfile = SliceProfile(decldata->nameOfIdentifier, decldata->lineNumber,
                                                 (decldata->isPointer), true,
                                                 std::set<unsigned int>{decldata->lineNumber});
                sliceProfile.nameOfContainingClass = ctx.currentClassName;
                sliceProfileItr->second.push_back(sliceProfile);
                sliceProfileItr->second.back().containsDeclaration = true;
            } else {
                auto sliceProf = SliceProfile(decldata->nameOfIdentifier, decldata->lineNumber,
                                              (decldata->isPointer), false,
                                              std::set<unsigned int>{decldata->lineNumber});
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProf.containsDeclaration = true;
                profileMap.insert(std::make_pair(decldata->nameOfIdentifier,
                                                  std::vector<SliceProfile>{
                                                          std::move(sliceProf)
                                                  }));
            }

            // Do not remove, it will cause a segmentation fault
            sliceProfileItr = profileMap.find(decldata->nameOfIdentifier);

            sliceProfileItr->second.back().isReference = decldata->isReference;
            sliceProfileItr->second.back().isPointer = decldata->isPointer;

            //look at the dvars and add this current variable to their dvar's lists. If we haven't seen this name before, add its slice profile
            for (std::string dvar : sliceEventData.declDvars) {
                auto updateDvarAtThisLocation = profileMap.find(dvar);
                if (updateDvarAtThisLocation != profileMap.end()) {
                    if (!StringContainsCharacters(decldata->nameOfIdentifier)) continue;
                    if (sliceProfileItr != profileMap.end() && sliceProfileItr->second.back().potentialAlias) {
                        if ( decldata->nameOfIdentifier != sliceProfileItr->second.back().variableName) {
                            updateDvarAtThisLocation->second.back().aliases.insert(std::make_pair(decldata->nameOfIdentifier, ctx.currentLineNumber));
                        }
                        continue;
                    }
                    updateDvarAtThisLocation->second.back().dvars.insert(std::make_pair(decldata->nameOfIdentifier, ctx.currentLineNumber));
                } else {
                    auto sliceProf = SliceProfile(dvar, decldata->lineNumber, false, false, std::set<unsigned int>{},
                                                  std::set<unsigned int>{decldata->lineNumber});
                    sliceProf.nameOfContainingClass = ctx.currentClassName;
                    auto newSliceProfileFromDeclDvars = profileMap.insert(std::make_pair(dvar,
                                                                                          std::vector<SliceProfile>{
                                                                                                  std::move(sliceProf)
                                                                                          }));
                    if (!StringContainsCharacters(decldata->nameOfIdentifier)) continue;
                    if (sliceProfileItr != profileMap.end() && sliceProfileItr->second.back().potentialAlias) {
                        if ( decldata->nameOfIdentifier != sliceProfileItr->second.back().variableName &&
                            newSliceProfileFromDeclDvars.first->second.back().variableName != sliceEventData.currentName) {
                            newSliceProfileFromDeclDvars.first->second.back().aliases.insert(std::make_pair(decldata->nameOfIdentifier, ctx.currentLineNumber));
                        }
                        continue;
                    }
                    newSliceProfileFromDeclDvars.first->second.back().dvars.insert(std::make_pair(decldata->nameOfIdentifier, ctx.currentLineNumber));
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

            sliceEventData.declDvars.clear();
            decldata->clear();
            delete decldata;
        } else if (typeid(ExprPolicy) == typeid(*policy)) {
            ExprPolicy::ExprDataSet* exprDataSet = policy->Data<ExprPolicy::ExprDataSet>();
            //iterate through every token found in the expression statement
            for (auto exprdata : exprDataSet->dataSet) {
                auto sliceProfileExprItr = profileMap.find(exprdata.second.nameOfIdentifier);
                auto sliceProfileLHSItr = profileMap.find(exprDataSet->lhsName);
                //Just update definitions and uses if name already exists. Otherwise, add new name.
                if (sliceProfileExprItr != profileMap.end()) {
                    sliceProfileExprItr->second.back().nameOfContainingClass = ctx.currentClassName;
                    sliceProfileExprItr->second.back().uses.insert(exprdata.second.uses.begin(),
                                                                   exprdata.second.uses.end());
                    sliceProfileExprItr->second.back().definitions.insert(exprdata.second.definitions.begin(),
                                                                          exprdata.second.definitions.end());

                    if (!StringContainsCharacters(exprDataSet->lhsName)) continue;
                    if (sliceProfileLHSItr != profileMap.end() && sliceProfileLHSItr->second.back().potentialAlias) {
                        if ( exprDataSet->lhsName != sliceProfileExprItr->second.back().variableName ) {
                            sliceProfileExprItr->second.back().aliases.insert(std::make_pair(exprDataSet->lhsName, ctx.currentLineNumber));
                        }
                        continue;
                    }
                    
                    // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                    // IE : abc = abc + i;
                    if (!StringContainsCharacters(sliceEventData.currentName)) continue;
                    if (!sliceEventData.currentName.empty() &&
                        (exprdata.second.lhs || sliceEventData.currentName != exprdata.second.nameOfIdentifier) &&
                        sliceProfileExprItr->second.back().variableName != sliceEventData.currentName) {
                        sliceProfileExprItr->second.back().dvars.insert(std::make_pair(sliceEventData.currentName, ctx.currentLineNumber));
                        continue;
                    }

                } else {
                    auto sliceProfileExprItr2 = profileMap.insert(std::make_pair(exprdata.second.nameOfIdentifier,
                                                                                  std::vector<SliceProfile>{
                                                                                          SliceProfile(
                                                                                                  exprdata.second.nameOfIdentifier,
                                                                                                  ctx.currentLineNumber,
                                                                                                  false, false,
                                                                                                  exprdata.second.definitions,
                                                                                                  exprdata.second.uses)
                                                                                  }));
                    sliceProfileExprItr2.first->second.back().nameOfContainingClass = ctx.currentClassName;

                    if (!StringContainsCharacters(exprDataSet->lhsName)) continue;
                    if (sliceProfileLHSItr != profileMap.end() && sliceProfileLHSItr->second.back().potentialAlias) {
                        if ( exprDataSet->lhsName != sliceProfileLHSItr->second.back().variableName ) {
                            sliceProfileExprItr2.first->second.back().aliases.insert(std::make_pair(exprDataSet->lhsName, ctx.currentLineNumber));
                        }
                        continue;
                    }

                    // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                    // IE : abc = abc + i;
                    if (!StringContainsCharacters(sliceEventData.currentName)) continue;
                    if (!sliceEventData.currentName.empty() &&
                        (exprdata.second.lhs || sliceEventData.currentName != exprdata.second.nameOfIdentifier)) {
                        sliceProfileExprItr2.first->second.back().dvars.insert(std::make_pair(sliceEventData.currentName, ctx.currentLineNumber));
                        continue;
                    }
                }
            }
            exprDataSet->clear();
            delete exprDataSet;
        } else if (typeid(InitPolicy) == typeid(*policy)) {
            InitPolicy::InitDataSet* initDataSet = policy->Data<InitPolicy::InitDataSet>();
            //iterate through every token found in the initialization of a decl_stmt
            for (auto initdata : initDataSet->dataSet) {
                sliceEventData.declDvars.push_back(initdata.second.nameOfIdentifier);
                auto sliceProfileItr = profileMap.find(initdata.second.nameOfIdentifier);
                //Just update definitions and uses if name already exists. Otherwise, add new name.
                if (sliceProfileItr != profileMap.end()) {
                    sliceProfileItr->second.back().uses.insert(initdata.second.uses.begin(),
                                                               initdata.second.uses.end());
                } else {
                    auto sliceProf = SliceProfile(initdata.second.nameOfIdentifier, ctx.currentLineNumber, false, false,
                                                  std::set<unsigned int>{}, initdata.second.uses);
                    sliceProf.nameOfContainingClass = ctx.currentClassName;
                    profileMap.insert(std::make_pair(initdata.second.nameOfIdentifier,
                                                      std::vector<SliceProfile>{sliceProf}));
                }
            }
            initDataSet->clear();
            delete initDataSet;
        } else if (typeid(CallPolicy) == typeid(*policy)) {
            // Runs when function calls are detected

            sliceEventData.functionCallList.insert(ctx.currentLineNumber);

            CallPolicy::CallData* calldata = policy->Data<CallPolicy::CallData>();
            // This loop is for simply tracking how many args are within a loaded function call
            int trueArgCount = 0;
            bool psuedoBool = false;
            for (auto currentCallToken : calldata->callargumentlist) {
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
            for (auto currentCallToken : calldata->callargumentlist) {
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
                    auto sliceProfileItr = profileMap.find(currentCallToken);

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
                    orderedFunctionSigMap.insert(sliceEventData.functionSigMap.begin(), sliceEventData.functionSigMap.end());

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
                    if (sliceProfileItr != profileMap.end()) {
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
                        profileMap.insert(std::make_pair(currentCallToken,
                                                          std::vector<SliceProfile>{sliceProf}));
                    }
                    if (!funcNameAndCurrArgumentPos.empty()) ++funcNameAndCurrArgumentPos.back().second;
                }
            }

            delete calldata;
        } else if (typeid(ParamTypePolicy) == typeid(*policy)) {
            DeclData* paramdata = policy->Data<DeclData>();
            
            //record parameter data-- this is done exact as it is done for decl_stmts except there's no initializer
            auto sliceProfileItr = profileMap.find(paramdata->nameOfIdentifier);
            //Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap.end()) {
                auto sliceProf = SliceProfile(paramdata->nameOfIdentifier, paramdata->lineNumber,
                                              (paramdata->isPointer), true,
                                              std::set<unsigned int>{paramdata->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProfileItr->second.push_back(std::move(sliceProf));
            } else {
                auto sliceProf = SliceProfile(paramdata->nameOfIdentifier, paramdata->lineNumber,
                                              (paramdata->isPointer), true,
                                              std::set<unsigned int>{paramdata->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                profileMap.insert(std::make_pair(paramdata->nameOfIdentifier,
                                                  std::vector<SliceProfile>{std::move(sliceProf)}));
            }

            // Attempt to insert data-types for sliceprofiles found in function/ctor parameters
            profileMap.find(paramdata->nameOfIdentifier)->second.back().variableType = paramdata->nameOfType;

            // Link the filepath this slice is located in
            profileMap.find(paramdata->nameOfIdentifier)->second.back().file = ctx.currentFilePath;

            // Link the function the XML Originates from
            profileMap.find(paramdata->nameOfIdentifier)->second.back().function = ctx.currentFunctionName;

            paramdata->clear();
            delete paramdata;
        } else if (typeid(FunctionSignaturePolicy) == typeid(*policy)) {
            SignatureData* funcSigDataPtr = policy->Data<SignatureData>();
            sliceEventData.functionsigdata = *funcSigDataPtr;

            if (sliceEventData.functionSigMap.find(sliceEventData.functionsigdata.name) != sliceEventData.functionSigMap.end() && !sliceEventData.functionsigdata.name.empty()) {
                // overloaded function detected

                // construct a new name to log the overloaded function under
                std::string functName = sliceEventData.functionsigdata.name;
                unsigned int overloadID = ++sliceEventData.overloadFunctionCount[sliceEventData.functionsigdata.name];
                functName += "-" + std::to_string(overloadID);

                sliceEventData.functionSigMap.insert(
                        std::make_pair(functName, sliceEventData.functionsigdata)
                        );
            } else
            {
                sliceEventData.functionSigMap.insert(
                        std::make_pair(sliceEventData.functionsigdata.name, sliceEventData.functionsigdata)
                        );
                sliceEventData.overloadFunctionCount[sliceEventData.functionsigdata.name] = 0;
            }

            delete funcSigDataPtr;
        } else if (typeid(ReturnPolicy) == typeid(*policy)) {
            for (auto dataSet : *returnPolicy.GetReturnUses()) {
                auto sliceProfileItr = profileMap.find(dataSet.first);
                
                // incase we have multiple slices of the same name under the hood
                // we determine if we have the right slice by checking its name
                // and whether we've already inserted data into it
                while (sliceProfileItr != profileMap.end()) {
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
                auto sliceProfileItr = profileMap.find(dataSet.first);
                std::vector<SliceProfile*> slicePtrs;

                // incase we have multiple slices of the same name under the hood
                // we determine if we have the right slice by checking its name
                // and whether we've already inserted data into it
                if (sliceProfileItr != profileMap.end()) {
                    for (auto& slice : sliceProfileItr->second) {
                        if (slice.containsDeclaration &&
                            slice.variableName == dataSet.first &&
                            !slice.conditionalUsesInserted) {

                            // Building a vector of pointer to the referenced slices
                            // in reverse by pushing to the front instead of push_back
                            slicePtrs.insert(slicePtrs.begin(), &slice);
                            
                            for (auto useLines : dataSet.second) {
                                if (useLines >= *(slice.definitions.begin())) {
                                    slice.uses.insert(useLines);
                                }
                            }
                            slice.conditionalUsesInserted = true;
                            insertTargets.insert(dataSet.first);
                        }
                    }

                    // Iterate the reverse array of ptrs to remove
                    // falsely set uses between slices in the same function
                    std::set<unsigned int> referencedUses;
                    for (auto& slicePtr : slicePtrs) {
                        if (slicePtr->function == conditionalPolicy.GetLastFunction()) {
                            slicePtr->variableName += " ";

                            // When we have referenced data it should be to a slice that is
                            // in front of the current slice being indexed
                            if (referencedUses.size() > 0) {
                                for (const auto num : referencedUses) {
                                    slicePtr->uses.erase(num);
                                }
                            } else {
                                referencedUses = slicePtr->uses;
                            }
                        }
                    }
                }
            }
            for (auto name : insertTargets) {
                conditionalPolicy.DeleteUsesCollection(name);
            }
            insertTargets.clear();

            for (auto dataSet : *conditionalPolicy.GetConditionalDefs()) {
                auto sliceProfileItr = profileMap.find(dataSet.first);
                
                // incase we have multiple slices of the same name under the hood
                // we determine if we have the right slice by checking its name
                // and whether we've already inserted data into it
                if (sliceProfileItr != profileMap.end()) {
                    for (auto& slice : sliceProfileItr->second) {
                        if (slice.containsDeclaration &&
                            slice.variableName == dataSet.first &&
                            !slice.conditionalDefsInserted) {
                            for (auto defLines : dataSet.second) {
                                if (defLines >= *(slice.definitions.begin())) {
                                    slice.uses.insert(defLines);
                                }
                            }
                            slice.conditionalDefsInserted = true;
                            insertTargets.insert(dataSet.first);
                        }
                    }
                }
            }
            for (auto name : insertTargets) {
                conditionalPolicy.DeleteDefsCollection(name);
            }
            insertTargets.clear();
            // RepairVariableNames();
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy [[maybe_unused]], srcSAXEventDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}

protected:
    void * DataInner() const override {
        return new SliceEventData(sliceEventData);
    }

private:
    DeclTypePolicy declPolicy;
    InitPolicy initPolicy;
    ExprPolicy exprPolicy;
    CallPolicy callPolicy;
    ReturnPolicy returnPolicy;
    ConditionalPolicy conditionalPolicy;
    FunctionSignaturePolicy functionPolicy;

    void InitializeEventHandlers() {
        using namespace srcSAXEventDispatch;

        openEventMap[ParserState::decl] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&declPolicy);
        };
        closeEventMap[ParserState::decl] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListener(&declPolicy);
            declPolicy.Finalize(ctx);
            sliceEventData.currentName.clear();
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
            sliceEventData.currentName.clear();
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
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&initPolicy);
            ctx.dispatcher->RemoveListener(&conditionalPolicy);
        };

        openEventMap[ParserState::forstmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::forstmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.endLine = ctx.currentLineNumber;
            sliceEventData.loopdata.push_back(std::make_pair(sliceEventData.startLine, sliceEventData.endLine));
        };

        openEventMap[ParserState::whilestmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::whilestmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.endLine = ctx.currentLineNumber;
            sliceEventData.loopdata.push_back(std::make_pair(sliceEventData.startLine, sliceEventData.endLine));
        };

        openEventMap[ParserState::ifstmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::ifstmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.endLine = ctx.currentLineNumber;
            sliceEventData.ifdata.push_back(std::make_pair(sliceEventData.startLine, sliceEventData.endLine));
        };

        openEventMap[ParserState::elseif] = [this](srcSAXEventContext &ctx) {
            sliceEventData.startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::elseif] = [this](srcSAXEventContext &ctx) {
            sliceEventData.endLine = ctx.currentLineNumber;
            sliceEventData.elsedata.push_back(std::make_pair(sliceEventData.startLine, sliceEventData.endLine));
        };

        openEventMap[ParserState::elsestmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::elsestmt] = [this](srcSAXEventContext &ctx) {
            sliceEventData.endLine = ctx.currentLineNumber;
            sliceEventData.elsedata.push_back(std::make_pair(sliceEventData.startLine, sliceEventData.endLine));
        };

        closeEventMap[ParserState::op] = [this](srcSAXEventContext &ctx) {
            bool isAssignmentOperator = (ctx.currentToken == "=" || ctx.currentToken == "+=" ||
                                        ctx.currentToken == "-=" || ctx.currentToken == "*=" ||
                                        ctx.currentToken == "/=" || ctx.currentToken == "%=");
            if (isAssignmentOperator) {
                sliceEventData.currentName = sliceEventData.currentExprName;
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
                    sliceEventData.currentExprName = ctx.currentToken;
                }
            }
        };

        closeEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
            sliceEventData.pmPtr = &profileMap;

            sliceEventData.possibleDefinitions = declPolicy.GetPossibleDefs();
            sliceEventData.possibleDvars = conditionalPolicy.GetPossibleDvars();
            sliceEventData.switchUses = conditionalPolicy.GetSwitchUses();
            sliceEventData.switchDefs = conditionalPolicy.GetSwitchDefs();

            NotifyAll(ctx);
        };
    } // End of Initialize Event Handlers
};

#endif