#ifndef SRCSLICEHANDLER
#define SRCSLICEHANDLER

#include <srcsliceprofile.hpp>
#include <srcslicecollection.hpp>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <srcDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <IfStmtPolicySingleEvent.hpp>
#include <SwitchPolicySingleEvent.hpp>
#include <WhilePolicySingleEvent.hpp>
#include <ForPolicySingleEvent.hpp>
#include <DoPolicySingleEvent.hpp>
#include <FunctionPolicySingleEvent.hpp>
#include <ClassPolicySingleEvent.hpp>
#include <UnitPolicySingleEvent.hpp>
#include <CLI11.hpp>

class SrcSliceHandler
        : public srcDispatch::EventListener,
          public srcDispatch::PolicyDispatcher,
          public srcDispatch::PolicyListener {
public:
    ~SrcSliceHandler() { };

    // Use literal string filename ctor of srcSAXController
    SrcSliceHandler(const char* filename, std::initializer_list<srcDispatch::PolicyListener *> listeners = {})
            : srcDispatch::PolicyDispatcher(listeners) {
        srcSAXController control(filename);
        srcDispatch::srcDispatcherSingleEvent<UnitPolicy> handler(this);
        control.parse(&handler); // Start parsing

        ComputeInterprocedural();
    }

    // Use string srcml buffer ctor of srcSAXController
    SrcSliceHandler(const std::string sourceCodeStr, std::initializer_list<srcDispatch::PolicyListener *> listeners = {})
            : srcDispatch::PolicyDispatcher(listeners) {
        srcSAXController control(sourceCodeStr);
        srcDispatch::srcDispatcherSingleEvent<UnitPolicy> handler(this);
        control.parse(&handler); // Start parsing
    }

    void Notify(const PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) override {
        if(typeid(ClassPolicy) == typeid(*policy)) {
            ProcessClassData(policy->Data<ClassData>(), ctx);
        } else if(typeid(FunctionPolicy) == typeid(*policy)) {
            ProcessFunctionData(policy->Data<FunctionData>(), "", ctx);
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}

    void ProcessFunctionData(std::shared_ptr<FunctionData> function_data, std::string className, const srcDispatch::srcSAXEventContext& ctx) {
        ProcessFunctionSignature(function_data, className, ctx);
        ProcessDeclStmts(function_data, nullptr, className, ctx);
        ProcessExprStmts(function_data, className, ctx);
    }

    void ProcessClassData(std::shared_ptr<ClassData> class_data, const srcDispatch::srcSAXEventContext& ctx) {

        std::string className = class_data->name->ToString();
        
        // Process Class Member Variables
        ProcessDeclStmts(nullptr, class_data, className, ctx);

        // Process Class Contructors
        for (auto& funcVec : class_data->constructors) {// [ vect<func>, vect<func>, vect<func> ]
            for (auto& func : funcVec)
                ProcessFunctionData(func, className, ctx);
        }

        // Process Class Methods (Member Functions)
        for (auto& funcVec : class_data->methods) {// [ vect<func>, vect<func>, vect<func> ]
            for (auto& func : funcVec)
                ProcessFunctionData(func, className, ctx);
        }
        
        // Process Operator Overloading
        for (auto& funcVec : class_data->operators) {// [ vect<func>, vect<func>, vect<func> ]
            for (auto& func : funcVec)
                ProcessFunctionData(func, className, ctx);
        }
        
    }

    void ProcessDeclStmts(std::shared_ptr<FunctionData> funcData, const std::shared_ptr<ClassData> classData, std::string className, const srcDispatch::srcSAXEventContext& ctx) {
        std::vector<std::shared_ptr<DeclData>> localGroup;

        if (funcData != nullptr) {
            // Capture general locals (decls)
            if (funcData->block != nullptr) {
                if (funcData->block->locals.size() > 0) {
                    localGroup.insert(localGroup.end(), funcData->block->locals.begin(), funcData->block->locals.end());
                }

                // Capture Conditional locals (decls)
                if (funcData->block->conditionals.size() > 0) {
                    CollectConditionalData(nullptr, &localGroup, funcData->block->conditionals);
                }
            }

        }

        // Look at all declared members from each class field
        if (classData != nullptr) {
            for (unsigned int j=0; j < classData->fields[ClassData::PUBLIC].size(); ++j) {
                localGroup.insert(localGroup.end(), classData->fields[ClassData::PUBLIC].begin(), classData->fields[ClassData::PUBLIC].end());
            }
            for (unsigned int j=0; j < classData->fields[ClassData::PROTECTED].size(); ++j) {
                localGroup.insert(localGroup.end(), classData->fields[ClassData::PROTECTED].begin(), classData->fields[ClassData::PROTECTED].end());
            }
            for (unsigned int j=0; j < classData->fields[ClassData::PRIVATE].size(); ++j) {
                localGroup.insert(localGroup.end(), classData->fields[ClassData::PRIVATE].begin(), classData->fields[ClassData::PRIVATE].end());
            }
            localGroup.insert(localGroup.end(), classData->fields->begin(), classData->fields->end());
            className = classData->name->ToString();
        }

        // loop through all the expression statements within Decl Statements
        for (const auto localVar : localGroup) {
            if (localVar == nullptr) continue;
            if (localVar->name == nullptr) continue;

            std::vector<std::shared_ptr<VariableData>> varDataGroup;

            if (localVar->init != nullptr)
                varDataGroup = ParseExpr(*localVar->init, localVar->init->lineNumber);

            // Collect pieces about the newly declared variable to use later when adding it into
            // our profileMap
            std::string declVarName = localVar->name->ToString();
            std::string declVarType = "";

            bool isPointer = false;
            bool isReference = false;

            // Extract just the Data-Type name without extra data
            for (std::size_t pos = 0; pos < localVar->type->types.size(); ++pos) {
                const std::pair<std::any, TypeData::TypeType>& type = localVar->type->types[pos];

                if (type.second == TypeData::POINTER) {
                    isPointer = true;
                    declVarType += "*";
                } else if (type.second == TypeData::REFERENCE) {
                    isReference = true;
                    declVarType += "&";
                } /* else if (type.second == TypeData::RVALUE) {
                } else if (type.second == TypeData::SPECIFIER) {
                } else if (type.second == TypeData::TYPENAME) {
                } */

                if (type.second == TypeData::TYPENAME) {
                    declVarType = std::any_cast<std::shared_ptr<NameData>>(type.first)->ToString();
                    // remove `std ` in `std string` if neccessary
                    declVarType = declVarType.substr(declVarType.find(' ')+1);
                }
            }

            auto sliceProfileItr = profileMap.find(declVarName);

            //Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap.end()) {
                // Check if the new slice we potentially try to create has not already been made
                // (we dont want to have duplicates of the same slice)
                
                // We may have variables of the same name, but each slice of the same name must be initially declared on different lines
                if (sliceProfileItr->second.back().variableName != declVarName || sliceProfileItr->second.back().lineNumber != localVar->lineNumber) {
                    auto sliceProfile = SliceProfile(declVarName, localVar->lineNumber, isPointer, true, std::set<unsigned int>{localVar->lineNumber});

                    sliceProfile.nameOfContainingClass = className;
                    // sliceProfile.containingNameSpaces = ctx.currentNamespaces;
                    sliceProfile.language = ctx.currentFileLanguage;

                    sliceProfile.isPointer = isPointer;
                    sliceProfile.isReference = isReference;

                    sliceProfileItr->second.push_back(sliceProfile);
                    sliceProfileItr->second.back().containsDeclaration = true;
                }
            } else {
                auto sliceProf = SliceProfile(declVarName, localVar->lineNumber, (isPointer), false, std::set<unsigned int>{localVar->lineNumber});

                sliceProf.nameOfContainingClass = className;
                // sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.containsDeclaration = true;

                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                profileMap.insert(std::make_pair(declVarName, std::vector<SliceProfile>{ std::move(sliceProf) }));
            }

            // Do not remove, it will cause a segmentation fault
            sliceProfileItr = profileMap.find(declVarName);

            sliceProfileItr->second.back().isReference = isReference;
            sliceProfileItr->second.back().isPointer = isPointer;

            // Look at the dvars and add this current variable to their dvar's lists.
            // If we haven't seen this name before, add its slice profile
            for (auto& varData : varDataGroup) {
                UpdateLHSSlices(varData);
                for (auto& dvarData : varData->rhsElems) {
                    std::string dvar = dvarData->GetNameOfIdentifier();

                    auto updateDvarAtThisLocation = profileMap.find(dvar);
                    if (updateDvarAtThisLocation != profileMap.end()) {

                        // Update the use/defs for already existing slices
                        // ProcessExprStmts does not capture expr_stmts
                        // contained within Decl_stmts
                        updateDvarAtThisLocation->second.back().uses.insert(dvarData->uses.begin(), dvarData->uses.end());
                        updateDvarAtThisLocation->second.back().definitions.insert(dvarData->definitions.begin(), dvarData->definitions.end());

                        if (!StringContainsCharacters(declVarName)) continue;
                        if (sliceProfileItr != profileMap.end() && sliceProfileItr->second.back().potentialAlias) {
                            if ( declVarName != sliceProfileItr->second.back().variableName) {
                                updateDvarAtThisLocation->second.back().aliases.insert(std::make_pair(declVarName, dvarData->originLine));
                            }
                            continue;
                        }
                        updateDvarAtThisLocation->second.back().dvars.insert(std::make_pair(declVarName, dvarData->originLine));
                    } else {
                        auto sliceProf = SliceProfile(
                            dvar,
                            localVar->lineNumber,
                            false,
                            false,
                            std::set<unsigned int>{},
                            std::set<unsigned int>{localVar->lineNumber}
                        );

                        sliceProf.nameOfContainingClass = className;
                        // sliceProf.containingNameSpaces = ctx.currentNamespaces;
                        sliceProf.language = ctx.currentFileLanguage;

                        auto newSliceProfileFromDeclDvars = profileMap.insert(std::make_pair(dvar,
                                                                                            std::vector<SliceProfile>{
                                                                                                    std::move(sliceProf)
                                                                                            }));
                        if (!StringContainsCharacters(declVarName)) continue;
                        if (sliceProfileItr != profileMap.end() && sliceProfileItr->second.back().potentialAlias) {
                            if ( declVarName != sliceProfileItr->second.back().variableName ) {
                                newSliceProfileFromDeclDvars.first->second.back().aliases.insert(std::make_pair(declVarName, dvarData->originLine));
                            }
                            continue;
                        }
                        newSliceProfileFromDeclDvars.first->second.back().dvars.insert(std::make_pair(declVarName, dvarData->originLine));
                    }
                }
            }

            // This allows me to set the data type of the variable in its slice
            // after its been set up from the logic above here
            // Set the data-type of sliceprofile for decl vars inside of function bodies

            sliceProfileItr->second.back().variableType = declVarType;

            // Link the filepath the XML Originates
            sliceProfileItr->second.back().file = ctx.currentFilePath;

            // Link the file hash attribute
            sliceProfileItr->second.back().checksum = ctx.currentFileChecksum;
            
            // Link the function this slice is located in
            if (funcData != nullptr)
                sliceProfileItr->second.back().function = funcData->name->ToString();
                
            // Link the class this slice is located in
            if (classData != nullptr)
                sliceProfileItr->second.back().nameOfContainingClass = classData->name->ToString();
        }
    }

    void ProcessExprStmts(std::shared_ptr<FunctionData> funcData, std::string className, const srcDispatch::srcSAXEventContext& ctx) {
        std::vector<std::shared_ptr<ExpressionData>> exprStmts;
        
        // Capture general expressions
        if (funcData->block != nullptr) {
            if (funcData->block->expr_stmts.size() > 0) {
                exprStmts.insert(exprStmts.end(), funcData->block->expr_stmts.begin(), funcData->block->expr_stmts.end());
            }
        }
        
        // Capture general Return expressions
        if (funcData->block != nullptr) {
            if (funcData->block->returns.size() > 0) {
                exprStmts.insert(exprStmts.end(), funcData->block->returns.begin(), funcData->block->returns.end());
            }
        }

        // Capture Conditional expressions
        if (funcData->block != nullptr) {
            if (funcData->block->conditionals.size() > 0) {
                CollectConditionalData(&exprStmts, nullptr, funcData->block->conditionals);
            }
        }
        
        // loop through all the expression statements
        for (auto itr = exprStmts.begin(); itr != exprStmts.end(); ++itr) {
            std::shared_ptr<ExpressionData> expr = *itr;
            if (expr == nullptr) continue;

            // Check if any NameData within an Express contains index operator expressions
            // and insert those into the exprStmts vector
            for (const auto& data : expr->expr) {
                if (data.type() == typeid(std::shared_ptr<NameData>)) {
                    std::shared_ptr<NameData> nameData = std::any_cast<std::shared_ptr<NameData>>(data);
                    if (nameData->indices != nullptr) {
                        itr = exprStmts.insert(itr+1, nameData->indices); // set to iterator of newly inserted data
                        --itr;
                    }
                }
            }

            std::vector<std::shared_ptr<VariableData>> varDataGroup;
            varDataGroup = ParseExpr(*expr, expr->lineNumber);

            for (auto& varData : varDataGroup) {
                UpdateLHSSlices(varData);
                for (auto& rhsVarData : varData->rhsElems) {
                    std::string lhsName = varData->GetNameOfIdentifier();
                    std::string rhsName = rhsVarData->GetNameOfIdentifier();

                    auto sliceProfileExprItr = profileMap.find(rhsName);
                    auto sliceProfileLHSItr = profileMap.find(lhsName);

                    //Just update definitions and uses if name already exists. Otherwise, add new name.
                    if (sliceProfileExprItr != profileMap.end()) {
                        sliceProfileExprItr->second.back().nameOfContainingClass = className;
                        // sliceProfileExprItr->second.back().containingNameSpaces = ctx.currentNamespaces;
                        sliceProfileExprItr->second.back().language = ctx.currentFileLanguage;

                        sliceProfileExprItr->second.back().uses.insert(rhsVarData->uses.begin(),
                                                                       rhsVarData->uses.end());
                        sliceProfileExprItr->second.back().definitions.insert(rhsVarData->definitions.begin(),
                                                                              rhsVarData->definitions.end());

                        if (!StringContainsCharacters(lhsName)) continue;
                        if (sliceProfileLHSItr != profileMap.end() && sliceProfileLHSItr->second.back().potentialAlias) {
                            if ( lhsName != sliceProfileExprItr->second.back().variableName ) {
                                sliceProfileExprItr->second.back().aliases.insert(std::make_pair(lhsName, varData->originLine));
                            }
                            continue;
                        }
                        
                        // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                        // IE : abc = abc + i;
                        if (!StringContainsCharacters(lhsName)) continue;
                        if (!lhsName.empty() && sliceProfileExprItr->second.back().variableName != lhsName) {
                            sliceProfileExprItr->second.back().dvars.insert(std::make_pair(lhsName, varData->originLine));
                            continue;
                        }

                    } else {
                        auto sliceProfileExprItr2 = profileMap.insert(std::make_pair(rhsName,
                                                                                        std::vector<SliceProfile>{
                                                                                                SliceProfile(
                                                                                                        rhsName,
                                                                                                        rhsVarData->originLine,
                                                                                                        false, false,
                                                                                                        rhsVarData->definitions,
                                                                                                        rhsVarData->uses)
                                                                                        }));
                        sliceProfileExprItr2.first->second.back().nameOfContainingClass = className;
                        // sliceProfileExprItr2.first->second.back().containingNameSpaces = ctx.currentNamespaces;
                        sliceProfileExprItr2.first->second.back().language = ctx.currentFileLanguage;

                        if (!StringContainsCharacters(lhsName)) continue;
                        if (sliceProfileLHSItr != profileMap.end() && sliceProfileLHSItr->second.back().potentialAlias) {
                            if ( lhsName != sliceProfileLHSItr->second.back().variableName ) {
                                sliceProfileExprItr2.first->second.back().aliases.insert(std::make_pair(lhsName, varData->originLine));
                            }
                            continue;
                        }

                        // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                        // IE : abc = abc + i;
                        if (!StringContainsCharacters(lhsName)) continue;
                        if (!lhsName.empty() && (lhsName != rhsName)) {
                            sliceProfileExprItr2.first->second.back().dvars.insert(std::make_pair(lhsName, varData->originLine));
                            continue;
                        }
                    }
                }
            }
        }

    }

    // Use collected function call data to push a new cfunctions entry into a referenced slice profile
    void CreateSliceCallData(std::string functionName, int argIndex, int functionDefLine, SliceProfile& sliceProfile) {
        auto sliceCallData = std::make_pair(
            functionName, // function call name
            std::make_pair(
                std::to_string(argIndex), // arg index starting from 1 to n
                std::to_string(functionDefLine) // function definition line number
            )
        );

        if (sliceProfile.cfunctions.empty()) {
            sliceProfile.cfunctions.push_back(sliceCallData);
        } else if (sliceProfile.cfunctions.back() != sliceCallData) {
            sliceProfile.cfunctions.push_back(sliceCallData);
        }
    }

    void ProcessFunctionCall(std::shared_ptr<CallData> funcCallData) {
        std::string functionName = funcCallData->name->ToString();

        int argIndex = 0;
        for (auto& arg : funcCallData->arguments) {
            ++argIndex;

            // Extract the Variable Name from the expression contained within
            // the function call argument list index
            for (auto& exprElem : arg->expr) {
                if (exprElem.type() == typeid(std::shared_ptr<NameData>)) {
                    std::shared_ptr<NameData> name = std::any_cast<std::shared_ptr<NameData>>(exprElem);
                    unsigned int argUseLineNumber = funcCallData->lineNumber;

                    // Don't worry about exprElems with bad name ptrs
                    if (name == nullptr) continue;

                    // Update an existing slices Call data
                    auto sliceProfileItr = profileMap.find(name->ToString());
                    if (sliceProfileItr != profileMap.end()) {
                        // variable is used within a function call, even if a signature or fingerprint
                        // cannot be located

                        // Need to also potentially add definition line numbers incase there are
                        // increment or decrement operators with the argument expression
                        sliceProfileItr->second.back().uses.insert(argUseLineNumber);

                        std::string simpleFunctionName = GetSimpleFunctionName(functionName);

                        // Get the collection of functions by name
                        auto funcSig = funcSigCollection.functionSigMap.find(simpleFunctionName);
                        if (funcSig != funcSigCollection.functionSigMap.end()) {
                            size_t pos = 0;

                            // Attempt to fingerprint the right signature based on function call parameter list size
                            while (funcCallData->arguments.size() != funcSig->second[pos]->parameters.size()) {
                                if (++pos >= funcSig->second.size()) break;
                            }

                            if (pos < funcSig->second.size()) {
                                unsigned int funcLineDef = funcSig->second[pos]->lineNumber;
                                CreateSliceCallData(simpleFunctionName, argIndex, funcLineDef, sliceProfileItr->second.back());
                            } else {
                                std::cout << "[-] Fingerprint Not Found for -> " << simpleFunctionName << std::endl;
                                CreateSliceCallData(simpleFunctionName, argIndex, 0, sliceProfileItr->second.back());
                            }
                        } else {
                            std::cout << "[-] No Function Signature Found for -> " << simpleFunctionName << std::endl;
                            CreateSliceCallData(simpleFunctionName, argIndex, 0, sliceProfileItr->second.back());
                        }
                    }
                }
            }
        }
    }

    void CollectConditionalData(std::vector<std::shared_ptr<ExpressionData>>* exprStmts, std::vector<std::shared_ptr<DeclData>>* declStmts, std::vector<std::any>& conditionals) {
        if (&conditionals == nullptr) return;
        std::vector<std::shared_ptr<BlockData>> cntlBlocks;

        for (const auto& cntl : conditionals) {
            if (cntl.type() == typeid(std::shared_ptr<IfStmtData>)) {
                // Extract all of the block data from if statements
                std::shared_ptr<IfStmtData> ifcntl = std::any_cast<std::shared_ptr<IfStmtData>>(cntl);

                for (const auto& clause : ifcntl->clauses) {
                    if (clause.type() == typeid(std::shared_ptr<IfData>)) {
                        std::shared_ptr<IfData> data = std::any_cast<std::shared_ptr<IfData>>(clause);

                        if (exprStmts != nullptr) {
                            exprStmts->push_back(data->condition);
                        }

                        cntlBlocks.push_back(data->block);
                    } else if (clause.type() == typeid(std::shared_ptr<ElseIfData>)) {
                        std::shared_ptr<ElseIfData> data = std::any_cast<std::shared_ptr<ElseIfData>>(clause);

                        if (exprStmts != nullptr) {
                            exprStmts->push_back(data->condition);
                        }

                        cntlBlocks.push_back(data->block);
                    } else if (clause.type() == typeid(std::shared_ptr<ElseData>)) {
                        std::shared_ptr<ElseData> data = std::any_cast<std::shared_ptr<ElseData>>(clause);
                        cntlBlocks.push_back(data->block);
                    }
                }
            } else if (cntl.type() == typeid(std::shared_ptr<SwitchData>)) {
                // Extract all of the block data from Switch statements
                std::shared_ptr<SwitchData> switchcntl = std::any_cast<std::shared_ptr<SwitchData>>(cntl);

                /*
                    Ensure we are getting the uses from the case lines
                    Ensure we capture data from the case blocks as well
                */

                if (exprStmts != nullptr) {
                    exprStmts->push_back(switchcntl->condition);
                }

                cntlBlocks.push_back(switchcntl->block);
            } else if (cntl.type() == typeid(std::shared_ptr<WhileData>)) {
                // Extract all of the block data from While Loops
                std::shared_ptr<WhileData> whilecntl = std::any_cast<std::shared_ptr<WhileData>>(cntl);

                if (exprStmts != nullptr) {
                    exprStmts->push_back(whilecntl->condition);
                }

                cntlBlocks.push_back(whilecntl->block);
            } else if (cntl.type() == typeid(std::shared_ptr<ForData>)) {
                // Extract all of the block data from For Loops
                std::shared_ptr<ForData> forcntl = std::any_cast<std::shared_ptr<ForData>>(cntl);

                if (declStmts != nullptr) {
                    for (auto& initData : forcntl->control->init) {
                        if (initData.type() == typeid(std::shared_ptr<DeclData>)) {
                            std::shared_ptr<DeclData> forInitDecl = std::any_cast<std::shared_ptr<DeclData>>(initData);
                            declStmts->push_back(forInitDecl);
                        }
                    }
                }

                if (exprStmts != nullptr) {
                    exprStmts->push_back(forcntl->control->condition);
                }

                cntlBlocks.push_back(forcntl->block);
            } else if (cntl.type() == typeid(std::shared_ptr<DoData>)) {
                // Extract all of the block data from Do-While Loops
                std::shared_ptr<DoData> dowhilecntl = std::any_cast<std::shared_ptr<DoData>>(cntl);

                if (exprStmts != nullptr) {
                    exprStmts->push_back(dowhilecntl->condition);
                }

                cntlBlocks.push_back(dowhilecntl->block);
            }
        }

        for (const auto& block : cntlBlocks) {
            if (declStmts != nullptr) {
                declStmts->insert(declStmts->end(), block->locals.begin(), block->locals.end());
            }

            if (exprStmts != nullptr) {
                exprStmts->insert(exprStmts->end(), block->expr_stmts.begin(), block->expr_stmts.end());
                exprStmts->insert(exprStmts->end(), block->returns.begin(), block->returns.end());
            }

            // Recursive call to dive into nested conditionals
            if (block->conditionals.size() > 0) {
                CollectConditionalData(exprStmts, declStmts, block->conditionals);
            }
        }
    }

    std::vector<std::shared_ptr<VariableData>> ParseExpr(const ExpressionData& expr, const unsigned int& lineNumber) {
        std::vector<std::shared_ptr<VariableData>> varDataGroup;
        std::string expr_op = "";
        std::shared_ptr<VariableData> lhsVar = std::make_shared<VariableData>();

        std::vector<std::shared_ptr<VariableData>> lhsStack;
        bool groupCollect = false;
        const char* keywords[] = {"this","auto","const","true","false","signed","unsigned","long","short","cout","cin","cerr","endl"};

        // loop through each element within a specific expression statement
        for (const auto& exprElem : expr.expr) {
            bool invalidName = false;

            if (exprElem.type() == typeid(std::shared_ptr<NameData>)) {
                std::shared_ptr<NameData> name = std::any_cast<std::shared_ptr<NameData>>(exprElem);
                if (name->ToString().empty()) continue;

                std::string varName = name->ToString();
                std::string target = "std::";

                // Check if the string starts with "std::"
                if (varName.rfind(target, 0) == 0) {
                    // Remove the prefix and set it to the result
                    varName = varName.substr(target.size());
                }

                // Ignore the extracted name if its within the keywords array
                for (const auto& w : keywords) {
                    if (varName == w) {
                        invalidName = true;
                        break;
                    }
                }

                // Only track valid variables
                if (!invalidName) {
                    if (!lhsVar->isInitialized()) {
                        lhsVar->InitializeLHS(varName, lineNumber);

                        // capture use-def chains for single statements such as: ++i
                        if (expr_op == "++" || expr_op == "--") {
                            lhsVar->definitions.insert(lineNumber);
                            lhsVar->uses.insert(lineNumber);
                        }
                    } else {
                        std::shared_ptr<VariableData> newRHSVar = std::make_shared<VariableData>(varName);
                        newRHSVar->uses.insert(lineNumber);
                        newRHSVar->SetOriginLine(lineNumber);

                        // capture use-def chains for rhs var statements such as: a = ++i
                        if (expr_op == "++" || expr_op == "--") {
                            newRHSVar->definitions.insert(lineNumber);
                        }

                        lhsVar->lhs = true;
                        lhsVar->AddRHS(newRHSVar);

                        // Ensure that tracked lhs variables get assigned all of their
                        // rhs vars in the expression
                        for (auto& lhs : lhsStack)
                            lhs->AddRHS(newRHSVar);
                    }
                }
            } else if (exprElem.type() == typeid(std::shared_ptr<OperatorData>)) {
                std::shared_ptr<OperatorData> opData = std::any_cast<std::shared_ptr<OperatorData>>(exprElem);
                expr_op = opData->op;

                if (expr_op == "(") {
                    groupCollect = true;
                } else if (expr_op == ")") {
                    groupCollect = false;
                    if (lhsStack.size() > 0) {
                        varDataGroup.push_back(lhsVar);
                        lhsVar = lhsStack.back();
                        lhsStack.pop_back();
                    }
                } else if (!lhsVar->rhsElems.empty()) { // if the lhs has rhs members
                    std::shared_ptr<VariableData> prevRHSPtr = lhsVar->GetRecentRHS();

                    if (isAssignment(expr_op)) {
                        // When we encounter assignment while containing a group of RHS variables
                        // we need to push this LHS-RHS pair into the vector we later return
                        lhsVar->lhs = true;
                        varDataGroup.push_back(lhsVar);
                        lhsStack.push_back(lhsVar); // save reference to outter lhs
                        lhsVar = std::make_shared<VariableData>();

                        // We need to set the new LHS variable to start creating a new
                        // LHS-RHS pair group
                        lhsVar->InitializeLHS(prevRHSPtr->GetNameOfIdentifier(), lineNumber);
                        lhsVar->definitions.insert(lineNumber);

                        // Coumpound Assignment is a classic Use-Def Chain
                        // ie: int a = b += c; // b is used and defined by +=
                        if (isCompoundAssignment(expr_op)) {
                            lhsVar->definitions.insert(lineNumber);
                            lhsVar->uses.insert(lineNumber);
                        }

                    } else {
                        if (expr_op == "+" || expr_op == "-" || expr_op == "*" || expr_op == "/" || expr_op == "%") {
                            // We will have captured some RHS variable, if we encounter this block we've encountered
                            // a use for the most recent RHS variable we've encountered
                            if (prevRHSPtr != nullptr) {
                                prevRHSPtr->uses.insert(lineNumber);
                            }
                        } else if (expr_op == "++" || expr_op == "--" ) {
                            if (prevRHSPtr != nullptr) {
                                prevRHSPtr->uses.insert(lineNumber);
                                prevRHSPtr->definitions.insert(lineNumber);
                            }
                        }

                    }
                } else { // if the lhs has no rhs members
                    if (isAssignment(expr_op)) {
                        lhsVar->lhs = true;
                        lhsVar->definitions.insert(lineNumber);

                        // Coumpound Assignment is a classic Use-Def Chain
                        // ie: n += 2;
                        if (isCompoundAssignment(expr_op)) {
                            lhsVar->uses.insert(lineNumber);
                            lhsVar->definitions.insert(lineNumber);
                        }
                    } else if (expr_op == "++" || expr_op == "--" ) {
                        lhsVar->uses.insert(lineNumber);
                        lhsVar->definitions.insert(lineNumber);
                    } else if (expr_op == "+" || expr_op == "-" || expr_op == "*" || expr_op == "/" || expr_op == "%") {
                        lhsVar->uses.insert(lineNumber);
                    } else if (isLogical(expr_op)) {
                        // anything within logical conditionals are uses
                        // we also will need to redeclare the lhs variable
                        lhsVar->uses.insert(lineNumber);
                        varDataGroup.push_back(lhsVar);

                        lhsVar = std::make_shared<VariableData>();
                    } else if (expr_op == "[" || expr_op == "]")
                    {
                        lhsVar->uses.insert(lineNumber);
                        varDataGroup.push_back(lhsVar);

                        lhsVar = std::make_shared<VariableData>();
                    }
                }
            } else if (exprElem.type() == typeid(std::shared_ptr<CallData>)) {
                // This will read through the Call Args and attempt
                // to find the slice profile for the extracted arg
                // and will attempt to insert potential:
                // use/def/call data
                std::shared_ptr<CallData> callData = std::any_cast<std::shared_ptr<CallData>>(exprElem);

                // if the call came from some ptr or object we want to add the source variable use
                if (callData->name->ToString().find('.') != std::string::npos) {
                    GetFuncCallSource(callData, callData->name->ToString().find('.'), varDataGroup);
                } else if (callData->name->ToString().find('-') != std::string::npos) {
                    GetFuncCallSource(callData, callData->name->ToString().find('-'), varDataGroup);
                }

                ProcessFunctionCall(callData);
            }
        } // end of looping elements

        // Only handle initialized variable data
        if (lhsVar->isInitialized()) {
            // For expressions with only a single variable name
            // where we never encounter an operator, ie `return x;`
            if (lhsVar->rhsElems.size() == 0 || !lhsVar->lhs) {
                lhsVar->uses.insert(lineNumber);
            }

            // Ensure the final potential LHS-RHS pair is pushed into the
            // collection we return
            if (lhsVar->rhsElems.size() > 0) {
                lhsVar->lhs = true;
            }
            varDataGroup.push_back(lhsVar);
        }

        return varDataGroup;
    }

    void ProcessFunctionParameters(std::vector<std::shared_ptr<DeclData>>& parameters, const std::string& currentFunctionName, std::string className, const srcDispatch::srcSAXEventContext& ctx) {
        for (auto& parameter : parameters) {
            if (parameter->name == nullptr) continue;
            std::string paramName = parameter->name->ToString();

            // the Type string also includes the symbols along with data-type name
            // so this needs to be parsed out of the ToString() output
            std::string paramType = "";

            bool isPointer = false;
            bool isReference = false;

            for(std::size_t pos = 0; pos < parameter->type->types.size(); ++pos) {
                const std::pair<std::any, TypeData::TypeType> & type = parameter->type->types[pos];
                if (type.second == TypeData::POINTER) {
                    isPointer = true;
                    paramType += "*";
                } else if (type.second == TypeData::REFERENCE) {
                    isReference = true;
                    paramType += "&";
                } /* else if (type.second == TypeData::RVALUE) {
                } else if (type.second == TypeData::SPECIFIER) {
                } else if (type.second == TypeData::TYPENAME) {
                } */

                if (type.second == TypeData::TYPENAME) {
                    paramType = std::any_cast<std::shared_ptr<NameData>>(type.first)->ToString();
                    // remove `std ` in `std string` if neccessary
                    paramType = paramType.substr(paramType.find(' ')+1);
                }
            }
            
            // Record parameter data-- this is done exact as it is done for decl_stmts except there's no initializer
            auto sliceProfileItr = profileMap.find(paramName);
            // Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap.end()) {
                auto sliceProf = SliceProfile(paramName, parameter->lineNumber,
                                              isPointer, true,
                                              std::set<unsigned int>{parameter->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = className;
                // sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                sliceProfileItr->second.push_back(std::move(sliceProf));
            } else {
                auto sliceProf = SliceProfile(paramName, parameter->lineNumber,
                                              isPointer, true,
                                              std::set<unsigned int>{parameter->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = className;
                // sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProf.language = ctx.currentFileLanguage;
                
                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                profileMap.insert(std::make_pair(paramName,
                                                  std::vector<SliceProfile>{std::move(sliceProf)}));
            }

            // Attempt to insert data-types for sliceprofiles found in function/ctor parameters
            profileMap.find(paramName)->second.back().variableType = paramType;

            // Link the filepath this slice is located in
            profileMap.find(paramName)->second.back().file = ctx.currentFilePath;

            // Link the file hash attribute
            profileMap.find(paramName)->second.back().checksum = ctx.currentFileChecksum;

            // Link the function the XML Originates from
            profileMap.find(paramName)->second.back().function = currentFunctionName;
        }
    }

    void ProcessFunctionSignature(std::shared_ptr<FunctionData> funcData, std::string className, const srcDispatch::srcSAXEventContext& ctx) {
        std::string functionName = funcData->name->ToString();
        if (functionName.empty()) return;
        bool updateSignature = (functionName.find("::") != std::string::npos);
        std::string scopeName = "";

        // Update a Signature Entry due to out-of-line definition
        if (updateSignature) {
            scopeName = functionName.substr(0, functionName.find("::"));
            functionName = functionName.substr(functionName.find_last_of("::")+1, -1);
        }

        // Process the parameters in a separate function
        ProcessFunctionParameters(funcData->parameters, funcData->name->ToString(), className, ctx);
        auto funcSig = funcSigCollection.functionSigMap.find(functionName);

        if (funcSig != funcSigCollection.functionSigMap.end()) {
            if (updateSignature) {
                for (auto& func : funcSig->second) {
                    if (func->parameters.size() == funcData->parameters.size()) {
                        func = funcData;
                        break;
                    }
                }
            } else {
                // overloaded function detected
                funcSig->second.push_back(funcData);
            }
        } else {
            // Insert a new signature
            funcSigCollection.functionSigMap.insert(
                    std::make_pair(functionName, std::vector<std::shared_ptr<FunctionData>>{funcData})
                    );
        }
    }

    // Extract the function name within either a call or a complex function name
    std::string GetSimpleFunctionName(std::string funcName) {
        std::string simpleFunctionName = funcName;
        bool containsScope = (funcName.find_last_of("::") != std::string::npos);
        bool containsAccessor = (funcName.find_last_of(".") != std::string::npos);
        bool containsPointsTo = (funcName.find_last_of("->") != std::string::npos);

        if (containsScope) {
            // Get the sub-string of a function call with a scope resolution
            // due to map keys containing no scope resolution symbol (::)
            simpleFunctionName = funcName.substr(funcName.find_last_of("::")+1, -1);
        } else if (containsAccessor) {
            // Get sub-string of function call using accessor operator
            simpleFunctionName = funcName.substr(funcName.find_last_of(".")+1, -1);
        } else if (containsPointsTo) {
            // Get sub-string of function call using points-to operator
            simpleFunctionName = funcName.substr(funcName.find_last_of("->")+1, -1);
        }

        return simpleFunctionName;
    }

    // Use for inserting Uses and Defs for Slices in the LHS of an Expression Statement
    void UpdateLHSSlices(std::shared_ptr<VariableData> varData) {
        if (varData->GetNameOfIdentifier().empty()) return;

        auto sliceProfileItr = profileMap.find(varData->GetNameOfIdentifier());

        // Just update definitions and uses if name already exists. Otherwise, add new name.
        if (sliceProfileItr != profileMap.end()) {
            sliceProfileItr->second.back().uses.insert(varData->uses.begin(),
                                                       varData->uses.end());
            sliceProfileItr->second.back().definitions.insert(varData->definitions.begin(),
                                                              varData->definitions.end());
        } else {
            std::cout << "[*] There is no Slice of --> '" << varData->GetNameOfIdentifier() << "'" << std::endl;
        }
    }

    // Attempts to read in callData name such as 'vec.size' or 'strPtr->size' and extract the root-variable
    // that eventually lead to the function call to mark it as a use
    void GetFuncCallSource(std::shared_ptr<CallData> callData, int end, std::vector<std::shared_ptr<VariableData>>& varDataGroup) {
        std::string srcName = callData->name->ToString().substr(0, end);
        std::shared_ptr<VariableData> sourceVar = std::make_shared<VariableData>(srcName);

        sourceVar->uses.insert(callData->lineNumber);
        sourceVar->SetOriginLine(callData->lineNumber);

        varDataGroup.push_back(sourceVar);
    }

    bool StringContainsCharacters(const std::string &str) {
        for (char ch : str) {
            if (std::isalpha(ch)) {
                return true;
            }
        }
        return false;
    }

    bool isAssignment(const std::string& expr_op) {
        return (expr_op == "=") || (expr_op == "+=") || (expr_op == "-=") || (expr_op == "*=") || (expr_op == "/=") || (expr_op == "%=");
    }

    bool isCompoundAssignment(const std::string& expr_op) {
        return (expr_op == "+=") || (expr_op == "-=") || (expr_op == "*=") || (expr_op == "/=") || (expr_op == "%=");
    }

    bool isLogical(const std::string& expr_op) {
        return ( (expr_op == "<") || (expr_op == ">") || (expr_op == "<=") || (expr_op == ">=") || (expr_op == "==")
                || (expr_op == "!=") || (expr_op == "&&" || (expr_op == "||")) );
    }

    bool isWhiteSpace(const std::string& str) {
        return str.find_first_not_of(" \t\n\r") == std::string::npos;
    }

    std::unordered_map<std::string, std::vector<SliceProfile>>& GetProfileMap() {
        return profileMap;
    }
    
    auto ArgumentProfile(std::pair<std::string, std::shared_ptr<FunctionData>> func, int paramIndex, std::unordered_set<std::string> visit_func) {
        // std::cerr << "[*] " << func.first << " | paramIndex -> " << paramIndex << " | " << func.second->parameters.size() << std::endl;
        // std::cerr << std::boolalpha << "No Name Data -> " << (func.second->parameters.at(paramIndex)->name == nullptr) << " | " << func.second->lineNumber << std::endl;
	    auto Spi = profileMap.find(func.second->parameters.at(paramIndex)->name->ToString());
        
        for (auto& param : func.second->parameters) {
            if (profileMap.find(param->name->ToString())->second.back().visited) {
                return Spi;
            } else {
                for (auto& cfunc : profileMap.find(param->name->ToString())->second.back().cfunctions) {
                    if (cfunc.first.compare(func.first) != 0) {
                        auto funcGroup = funcSigCollection.functionSigMap.find(cfunc.first);
                        if (funcGroup != funcSigCollection.functionSigMap.end()) {
                            size_t pos = 0;
                            std::shared_ptr<FunctionData> func = funcGroup->second[pos];

                            // Attempt to fingerprint the right signature based on function call definition line and called function
                            // def line data
                            while (cfunc.second.second != std::to_string(funcGroup->second[pos]->lineNumber)) {
                                if (++pos >= funcGroup->second.size()) break;
                            }

                            if (cfunc.first.compare(func->name->ToString()) == 0 && visit_func.find(cfunc.first) == visit_func.end()) {
				                visit_func.insert(cfunc.first);
                                // Ensure before we run ArgumentProfile that parameters has non-zero size and can be indexed safely
                                if (cfunc.first.compare(func->name->ToString()) == 0 && func->parameters.size() > 0 &&
                                    std::atoi(cfunc.second.first.c_str()) - 1 < func->parameters.size()) {
                                    auto recursiveSpi = ArgumentProfile(std::make_pair(cfunc.first, func), std::atoi(cfunc.second.first.c_str()) - 1, visit_func);
                                    if (profileMap.find(param->name->ToString()) != profileMap.end() &&
                                        profileMap.find(recursiveSpi->first) != profileMap.end()) {
                                        profileMap.find(param->name->ToString())->second.back().definitions.insert(
                                                recursiveSpi->second.back().definitions.begin(),
                                                recursiveSpi->second.back().definitions.end());
                                        profileMap.find(param->name->ToString())->second.back().uses.insert(
                                                recursiveSpi->second.back().uses.begin(),
                                                recursiveSpi->second.back().uses.end());
                                        profileMap.find(param->name->ToString())->second.back().cfunctions.insert(
                                                profileMap.find(
                                                        param->name->ToString())->second.back().cfunctions.begin(),
                                                recursiveSpi->second.back().cfunctions.begin(),
                                                recursiveSpi->second.back().cfunctions.end());
                                        profileMap.find(param->name->ToString())->second.back().aliases.insert(
                                                recursiveSpi->second.back().aliases.begin(),
                                                recursiveSpi->second.back().aliases.end());
                                        profileMap.find(param->name->ToString())->second.back().dvars.insert(
                                                recursiveSpi->second.back().dvars.begin(),
                                                recursiveSpi->second.back().dvars.end());
                                    }
                                }
                            }
                        }
                    }
                }
                profileMap.find(param->name->ToString())->second.back().visited = true;
            }
        }
        
        return Spi;
    }

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;
	    for (std::pair<std::string, std::vector<SliceProfile>> var : profileMap) {
            // Need to watch the Slices we attempt to dig into because we are collecting slices we have no interest in
            if (!profileMap.find(var.first)->second.back().visited && (var.second.back().variableName != "*LITERAL*")) {
                if (!var.second.back().cfunctions.empty()) {
                    for (auto& cfunc : var.second.back().cfunctions) {
                        auto funcGroup = funcSigCollection.functionSigMap.find(cfunc.first);
                        if(funcGroup != funcSigCollection.functionSigMap.end()) {
                            size_t pos = 0;
                            std::shared_ptr<FunctionData> func = funcGroup->second[pos];

                            // Attempt to fingerprint the right signature based on function call definition line and called function
                            // def line data
                            while (cfunc.second.second != std::to_string(funcGroup->second[pos]->lineNumber)) {
                                func = funcGroup->second[pos];
                                if (++pos >= funcGroup->second.size()) break;
                            }

                            std::string simpleFunctionName = GetSimpleFunctionName(func->name->ToString());

                            // Ensure before we run ArgumentProfile that parameters has non-zero size and can be indexed safely
                            if (cfunc.first.compare(simpleFunctionName) == 0 && func->parameters.size() > 0 &&
                                std::atoi(cfunc.second.first.c_str()) - 1 < func->parameters.size() &&
                                pos < funcGroup->second.size()) { //TODO fix for case: Overload
                                auto Spi = ArgumentProfile(std::make_pair(cfunc.first, func), std::atoi(cfunc.second.first.c_str()) - 1, visited_func);
                                auto sliceItr = Spi->second.begin();
                                std::string desiredVariableName = sliceItr->variableName;

                                for (sliceItr = Spi->second.begin(); sliceItr != Spi->second.end(); ++sliceItr) {
                                    if (sliceItr->containsDeclaration) {
                                        if (sliceItr->variableName != desiredVariableName) {
                                            // std::cout << "Name Check -> " << sliceItr->variableName << " | " << desiredVariableName << std::endl;
                                            continue;
                                        }
                                        if (GetSimpleFunctionName(sliceItr->function) != cfunc.first) {
                                            // std::cout << "Function Check -> " << sliceItr->function << " | " << cfunc.first << std::endl;
                                            continue;
                                        }
                                        std::string parameterDeclLine = std::to_string(func->parameters[std::stoi(cfunc.second.first) - 1]->lineNumber);
                                        if (std::to_string(sliceItr->lineNumber) != parameterDeclLine) {
                                            // std::cout << "Line Check -> " << std::to_string(sliceItr->lineNumber) << " | "
                                            // << func->parameters[std::stoi(cfunc.second.first)]->lineNumber << std::endl;
                                            continue;
                                        }

                                        break;
                                    }
                                }

                                if (profileMap.find(var.first) != profileMap.end() && profileMap.find(Spi->first) != profileMap.end() && sliceItr != Spi->second.end()) {
                                    if (!sliceItr->isReference && !sliceItr->isPointer) {
                                        // pass by value
                                        profileMap.find(var.first)->second.back().uses.insert(
                                                sliceItr->definitions.begin(),
                                                sliceItr->definitions.end());
                                    } else
                                    {
                                        // pass by reference
                                        profileMap.find(var.first)->second.back().definitions.insert(
                                                sliceItr->definitions.begin(),
                                                sliceItr->definitions.end());
                                    }

                                    // Parameter initial declaration def line is considered a use towards the argument
                                    profileMap.find(var.first)->second.back().definitions.erase(sliceItr->lineNumber);
                                    profileMap.find(var.first)->second.back().uses.insert(sliceItr->lineNumber);

                                    profileMap.find(var.first)->second.back().uses.insert(
                                            sliceItr->uses.begin(),
                                            sliceItr->uses.end());

                                    // By converting the cfunctions vector to a set, allows us to remove
                                    // duplicate entries, once those are removed we can convert this cleaned
                                    // set back into its vector form
                                    profileMap.find(var.first)->second.back().cfunctions.insert(
                                            profileMap.find(var.first)->second.back().cfunctions.begin(),
                                            sliceItr->cfunctions.begin(),
                                            sliceItr->cfunctions.end());
                                    auto oldCalledFunctions = profileMap.find(var.first)->second.back().cfunctions;
                                    std::set<std::pair<std::string, std::pair<std::string, std::string>>> calledFunctionSet(oldCalledFunctions.begin(), oldCalledFunctions.end());
                                    profileMap.find(var.first)->second.back().cfunctions = std::vector<std::pair<std::string, std::pair<std::string, std::string>>>(calledFunctionSet.begin(), calledFunctionSet.end());

                                    profileMap.find(var.first)->second.back().aliases.insert(
                                            sliceItr->aliases.begin(),
                                            sliceItr->aliases.end());
                                    profileMap.find(var.first)->second.back().dvars.insert(
                                            sliceItr->dvars.begin(),
                                            sliceItr->dvars.end());
                                } else {
                                    std::cout << std::boolalpha << "Is '" << var.first << "' a Map Entry? " << (profileMap.find(var.first) != profileMap.end())
                                    << " | Is Spi '"<< Spi->first <<"' a Map Entry? " << (profileMap.find(Spi->first) != profileMap.end())
                                    << " | Is The sliceItr Valid? " << (sliceItr != Spi->second.end()) << std::endl;

                                    std::cout << "Tried Accessing Slice Variable :: " << var.first << std::endl;
                                    std::cout << "[-] An Error has Occured in `ComputeInterprocedural`" << std::endl;
                                }
                            }
                        }
                    }
                }
                profileMap.find(var.first)->second.back().visited = true;
            }
        }
    }

protected:
    std::any DataInner() const override {
        return (void *) 0; // export profile to listeners
    }

private:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    std::vector<std::shared_ptr<ClassData>> classInfo;
    std::vector<std::shared_ptr<FunctionData>> functionInfo;
    FunctionSignatureData funcSigCollection;
};


#endif