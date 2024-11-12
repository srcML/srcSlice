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
/*
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>
#include <InitPolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <FunctionCallPolicy.hpp>
#include <ReturnPolicy.hpp>
#include <ConditionalPolicy.hpp>
*/

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

        // PrintCollection();
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
            classInfo.push_back(policy->Data<ClassData>());
        } else if(typeid(FunctionPolicy) == typeid(*policy)) {
            ProcessFunctionData(policy->Data<FunctionData>(), ctx);
            functionInfo.push_back(policy->Data<FunctionData>());
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}

    void ProcessFunctionData(std::shared_ptr<FunctionData> function_data, const srcDispatch::srcSAXEventContext& ctx) {
        ProcessFunctionSignature(function_data, ctx);
        ProcessDeclStmts(function_data, nullptr, ctx);
        ProcessExprStmts(function_data, ctx);
    }

    void ProcessClassData(std::shared_ptr<ClassData> class_data, const srcDispatch::srcSAXEventContext& ctx) {
        // Process Class Member Variables
        ProcessDeclStmts(nullptr, class_data, ctx);

        // Process Class Contructors
        for (size_t i = 0; i < class_data->constructors->size(); ++i) {
            ProcessFunctionData(class_data->constructors->at(i), ctx);
        }

        // Process Class Methods (Member Functions)
        for (size_t i = 0; i < class_data->methods->size(); ++i) {
            ProcessFunctionData(class_data->methods->at(i), ctx);
        }

        // Process Operator Overloading
        for (size_t i = 0; i < class_data->operators->size(); ++i) {
            ProcessFunctionData(class_data->operators->at(i), ctx);
        }
    }

    void ProcessDeclStmts(std::shared_ptr<FunctionData> funcData, const std::shared_ptr<ClassData> classData, const srcDispatch::srcSAXEventContext& ctx) {
        std::vector<std::shared_ptr<DeclData>> localGroup;

        if (funcData != nullptr) {
            // Capture general locals (decls)
            localGroup.insert(localGroup.end(), funcData->block->locals.begin(), funcData->block->locals.end());

            // Capture Conditional locals (decls)
            CollectConditionalData(nullptr, &localGroup, funcData->block->conditionals);
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
        }

        // loop through all the expression statements within Decl Statements
        for (const auto& localVar : localGroup) {
            std::vector<VariableData*> varDataGroup;

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
                } else if (type.second == TypeData::REFERENCE) {
                    isReference = true;
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
                auto sliceProfile = SliceProfile(declVarName, localVar->lineNumber,
                                                 isPointer, true,
                                                 std::set<unsigned int>{localVar->lineNumber});

                sliceProfile.nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                sliceProfile.containingNameSpaces = ctx.currentNamespaces;
                sliceProfile.language = ctx.currentFileLanguage;

                sliceProfile.isPointer = isPointer;
                sliceProfile.isReference = isReference;

                sliceProfileItr->second.push_back(sliceProfile);
                sliceProfileItr->second.back().containsDeclaration = true;
            } else {
                auto sliceProf = SliceProfile(declVarName, localVar->lineNumber,
                                              (isPointer), false,
                                              std::set<unsigned int>{localVar->lineNumber});

                sliceProf.nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.containsDeclaration = true;

                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                profileMap.insert(std::make_pair(declVarName,
                                                  std::vector<SliceProfile>{
                                                          std::move(sliceProf)
                                                  }));
            }

            // Do not remove, it will cause a segmentation fault
            sliceProfileItr = profileMap.find(declVarName);

            sliceProfileItr->second.back().isReference = isReference;
            sliceProfileItr->second.back().isPointer = isPointer;

            // Look at the dvars and add this current variable to their dvar's lists.
            // If we haven't seen this name before, add its slice profile
            for (auto varData : varDataGroup) {
                UpdateLHSSlices(varData);
                for (auto dvarData : varData->rhsElems) {
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

                        sliceProf.nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                        sliceProf.containingNameSpaces = ctx.currentNamespaces;
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

    void ProcessExprStmts(std::shared_ptr<FunctionData> funcData, const srcDispatch::srcSAXEventContext& ctx) {
        std::vector<std::shared_ptr<ExpressionData>> exprStmts;
        
        // Capture general expressions
        exprStmts.insert(exprStmts.end(), funcData->block->expr_stmts.begin(), funcData->block->expr_stmts.end());
        
        // Capture general Return expressions
        exprStmts.insert(exprStmts.end(), funcData->block->returns.begin(), funcData->block->returns.end());

        // Capture Conditional expressions
        CollectConditionalData(&exprStmts, nullptr, funcData->block->conditionals);
        
        // loop through all the expression statements
        for (const auto& expr : exprStmts) {
            auto varDataGroup = ParseExpr(*expr, expr->lineNumber);

            for (auto varData : varDataGroup) {
                UpdateLHSSlices(varData);
                for (auto rhsVarData : varData->rhsElems) {
                    std::shared_ptr<ExpressionElement> lhsData = varData->lhsElem;
                    std::string lhsName = varData->GetNameOfIdentifier();
                    std::string rhsName = rhsVarData->GetNameOfIdentifier();

                    auto sliceProfileExprItr = profileMap.find(rhsName);
                    auto sliceProfileLHSItr = profileMap.find(lhsName);

                    //Just update definitions and uses if name already exists. Otherwise, add new name.
                    if (sliceProfileExprItr != profileMap.end()) {
                        sliceProfileExprItr->second.back().nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                        sliceProfileExprItr->second.back().containingNameSpaces = ctx.currentNamespaces;
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
                        sliceProfileExprItr2.first->second.back().nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                        sliceProfileExprItr2.first->second.back().containingNameSpaces = ctx.currentNamespaces;
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

    void ProcessFunctionCall(std::shared_ptr<CallData> funcCallData) {
        int argIndex = 0;
        for (auto& arg : funcCallData->arguments) {
            ++argIndex;

            // Extract the Variable Name from the expression contained within
            // the function call argument list index
            for (auto& exprElem : arg->expr) {
                unsigned int argUseLineNumber = funcCallData->lineNumber;

                // Don't worry about exprElems with bad name ptrs
                if (exprElem->name == nullptr) continue;

                // Update an existing slices Call data
                auto sliceProfileItr = profileMap.find(exprElem->name->name);
                if (sliceProfileItr != profileMap.end()) {
                    std::string functionName = funcCallData->name->name;
                    auto funcSig = funcSigCollection.functionSigMap.find(functionName);
                    unsigned int funcLineDef = 0;
                    if (funcSig != funcSigCollection.functionSigMap.end()) {
                        funcLineDef = funcSig->second->lineNumber;
                    }

                    auto sliceCallData = std::make_pair(
                        functionName, // function call name
                        std::make_pair(
                            std::to_string(argIndex), // arg index starting from 1 to n
                            std::to_string(funcLineDef) // function definition line number
                        )
                    );

                    // Need to also potentially add definition line numbers incase there are
                    // increment or decrement operators with the argument expression
                    sliceProfileItr->second.back().uses.insert(argUseLineNumber);

                    if (sliceProfileItr->second.back().cfunctions.empty()) {
                        sliceProfileItr->second.back().cfunctions.push_back(sliceCallData);
                    } else if (sliceProfileItr->second.back().cfunctions.back() != sliceCallData) {
                        sliceProfileItr->second.back().cfunctions.push_back(sliceCallData);
                    }
                }
            }
        }
    }

    void CollectConditionalData(std::vector<std::shared_ptr<ExpressionData>>* exprStmts, std::vector<std::shared_ptr<DeclData>>* declStmts, std::vector<std::any>& conditionals) {
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
                    declStmts->insert(declStmts->end(), forcntl->control->init.begin(), forcntl->control->init.end());
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
        }
    }

    std::vector<VariableData*> ParseExpr(const ExpressionData& expr, const unsigned int& lineNumber) {
        std::vector<VariableData*> varDataGroup;
        std::string expr_op = "";
        VariableData* lhsVar = new VariableData();

        std::vector<VariableData*> lhsStack;
        bool groupCollect = false;

        // loop through each element within a specific expression statement
        for (const auto& exprElem : expr.expr) {
            bool isPostfix = true;

            switch (exprElem->type) {
                case ExpressionElement::NAME: // 0 --> enum to integer value
                    if (!lhsVar->isInitialized()) {
                        lhsVar->InitializeLHS(exprElem, lineNumber);

                        // capture use-def chains for single statements such as: ++i
                        if (expr_op == "++" || expr_op == "--") {
                            lhsVar->definitions.insert(lineNumber);
                            lhsVar->uses.insert(lineNumber);
                        }
                    } else {
                        VariableData* newRHSVar = new VariableData(exprElem);
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
                        for (auto lhs : lhsStack)
                            lhs->AddRHS(newRHSVar);
                    }
                break;
                case ExpressionElement::OP: // 1
                    expr_op = exprElem->token->token;

                    if (expr_op == "(") {
                        groupCollect = true;
                        break;
                    }

                    if (expr_op == ")") {
                        groupCollect = false;
                        if (lhsStack.size() > 0) {
                            varDataGroup.push_back(lhsVar);
                            lhsVar = lhsStack.back();
                            lhsStack.pop_back();
                        }
                        break;
                    }

                    if (!lhsVar->rhsElems.empty()) {
                        VariableData* prevRHSPtr = lhsVar->GetRecentRHS();

                        // Take advantage of white-spaces to deduce which variable a potential
                        // pre/postfix operator is effecting so the use-def chain gets assigned
                        // correctly
                        if (isWhiteSpace(expr_op))
                            isPostfix = false;

                        if (isAssignment(expr_op)) {
                            // When we encounter assignment while containing a group of RHS variables
                            // we need to push this LHS-RHS pair into the vector we later return
                            lhsVar->lhs = true;
                            varDataGroup.push_back(lhsVar);
                            lhsStack.push_back(lhsVar); // save reference to outter lhs
                            lhsVar = new VariableData();

                            // We need to set the new LHS variable to start creating a new
                            // LHS-RHS pair group
                            lhsVar->InitializeLHS(prevRHSPtr->lhsElem, lineNumber);
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
                            }
                            
                            if (expr_op == "++" || expr_op == "--" ) {
                                if (isPostfix) {
                                    if (prevRHSPtr != nullptr) {
                                        prevRHSPtr->uses.insert(lineNumber);
                                        prevRHSPtr->definitions.insert(lineNumber);
                                    }
                                }
                            }

                            if (!groupCollect) {
                                if (lhsStack.size() > 0) {
                                    varDataGroup.push_back(lhsVar);
                                    lhsVar = lhsStack.back();
                                    lhsStack.pop_back();
                                }
                            }
                        }
                    } else {
                        if (isWhiteSpace(expr_op))
                            isPostfix = false;

                        if (isAssignment(expr_op)) {
                            lhsVar->lhs = true;
                            lhsVar->definitions.insert(lineNumber);
                        }

                        // Coumpound Assignment is a classic Use-Def Chain
                        // ie: n += 2;
                        if (isCompoundAssignment(expr_op)) {
                            lhsVar->uses.insert(lineNumber);
                            lhsVar->definitions.insert(lineNumber);
                        } else if (expr_op == "++" || expr_op == "--" ) {
                            if (isPostfix) {
                                lhsVar->uses.insert(lineNumber);
                                lhsVar->definitions.insert(lineNumber);
                            }
                        } else if (expr_op == "+" || expr_op == "-" || expr_op == "*" || expr_op == "/" || expr_op == "%") {
                            lhsVar->uses.insert(lineNumber);
                        } else if (isLogical(expr_op)) {
                            // anything within logical conditionals are uses
                            // we also will need to redeclare the lhs variable
                            lhsVar->uses.insert(lineNumber);

                            varDataGroup.push_back(lhsVar);
                        }
                    }
                break;
                case ExpressionElement::CALL: // 2
                    // This will read through the Call Args and attempt
                    // to find the slice profile for the extracted arg
                    // and will attempt to insert potential:
                    // use/def/call data
                    ProcessFunctionCall(exprElem->call);
                break;
                default:
                break;
            }
        }

        // For expressions with only a single variable name
        // where we never encounter an operator, ie `return x;`
        if (lhsVar->rhsElems.size() == 0 || !lhsVar->lhs) {
            lhsVar->uses.insert(lineNumber);
            lhsVar->definitions.erase(lineNumber);
        }

        // Ensure the final LHS-RHS pair is pushed into out collection we return
        lhsVar->lhs = true;
        varDataGroup.push_back(lhsVar);

        return varDataGroup;
    }

    void ProcessFunctionParameters(std::vector<std::shared_ptr<DeclData>>& parameters, const std::string& currentFunctionName, const srcDispatch::srcSAXEventContext& ctx) {
        for (auto& parameter : parameters) {
            std::string paramName = parameter->name->ToString();

            // the Type string also includes the symbols along with data-type name
            // so this needs to be parsed out of the ToString() output
            std::string paramType = parameter->type->ToString().substr(0,parameter->type->ToString().find(' '));

            bool isPointer = false;
            bool isReference = false;

            for(std::size_t pos = 0; pos < parameter->type->types.size(); ++pos) {
                const std::pair<std::any, TypeData::TypeType> & type = parameter->type->types[pos];
                if (type.second == TypeData::POINTER) {
                    isPointer = true;
                } else if (type.second == TypeData::REFERENCE) {
                    isReference = true;
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
                sliceProf.nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                sliceProfileItr->second.push_back(std::move(sliceProf));
            } else {
                auto sliceProf = SliceProfile(paramName, parameter->lineNumber,
                                              isPointer, true,
                                              std::set<unsigned int>{parameter->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName.substr(0, ctx.currentClassName.find('\n'));
                sliceProf.containingNameSpaces = ctx.currentNamespaces;
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

    void ProcessFunctionSignature(std::shared_ptr<FunctionData> funcData, const srcDispatch::srcSAXEventContext& ctx) {
        std::string functionName = funcData->name->ToString();
        if (functionName.empty()) return;

        // Process the parameters in a separate function
        ProcessFunctionParameters(funcData->parameters, funcData->name->ToString(), ctx);

        if (funcSigCollection.functionSigMap.find(functionName) != funcSigCollection.functionSigMap.end()) {
            // overloaded function detected

            // construct a new name to log the overloaded function under
            std::string functName = functionName;
            unsigned int overloadID = ++funcSigCollection.overloadFunctionCount[functName];
            functName += "-" + std::to_string(overloadID);

            funcSigCollection.functionSigMap.insert(
                    std::make_pair(functName, funcData)
                    );
        } else {
            // Insert a new signature
            funcSigCollection.functionSigMap.insert(
                    std::make_pair(functionName, funcData)
                    );
            funcSigCollection.overloadFunctionCount[functionName] = 0;
        }
    }

    // Use for inserting Uses and Defs for Slices in the LHS of an Expression Statement
    void UpdateLHSSlices(VariableData* varData) {
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
	    auto Spi = profileMap.find(func.second->parameters.at(paramIndex)->name->ToString());

        for (auto param : func.second->parameters) {
            if (profileMap.find(param->name->ToString())->second.back().visited) {
                return Spi;
            } else {
                for (auto cfunc : profileMap.find(param->name->ToString())->second.back().cfunctions) {
                    if (cfunc.first.compare(func.first) != 0) {
                        auto function = funcSigCollection.functionSigMap.find(cfunc.first);
                        if (function != funcSigCollection.functionSigMap.end()) {
                            if (cfunc.first.compare(function->first) == 0 && visit_func.find(cfunc.first) == visit_func.end()) {
				                visit_func.insert(cfunc.first);
                                auto recursiveSpi = ArgumentProfile(*function, std::atoi(cfunc.second.first.c_str()) - 1, visit_func);
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
                profileMap.find(param->name->ToString())->second.back().visited = true;
            }
        }
        return Spi;
    }

    // void InsertSwitchData(SliceProfile& sliceProfile) {
    //     for (auto initDeclItem : sliceEventData->initDeclData) {
    //         // With a collection of data concerning where all variables are initially declared
    //         // check if the sliceProfile passed matches with a variable name within the collection
    //         if (sliceProfile.variableName != initDeclItem.first) continue;

    //         // Verify the correct slice by checking if the slice definition contains
    //         // the initial decl line number for the slice with the matching name
    //         if (sliceProfile.definitions.find(initDeclItem.second) == sliceProfile.definitions.end()) continue;

    //         // Store iterators of the functionSigMap in a vector
    //         std::vector<std::unordered_map<std::string, SignatureData>::iterator> funcSigMapItrs;
    //         for (auto it = sliceEventData->functionSigMap.begin(); it != sliceEventData->functionSigMap.end(); ++it) {
    //             funcSigMapItrs.push_back(it);
    //         }

    //         // Reverse Iterate through a collection of functions
    //         for (auto func = funcSigMapItrs.rbegin(); func != funcSigMapItrs.rend(); ++func) {
    //             // Check if there is a function after where we currently are
    //             auto nextFunc = std::next(func);
                
    //             // There is a function after the current
    //             if (nextFunc != funcSigMapItrs.rend()) {
    //                 // Using the initial variable decl line, check if it is within the scope of a function
    //                 // to prevent using variables with the same names in different functions, this works as
    //                 // another verify check
    //                 if (initDeclItem.second >= (*func)->second.lineNumber && initDeclItem.second <= (*nextFunc)->second.lineNumber - 1) {
    //                     for (auto data : *sliceEventData->switchUses) {
    //                         // When iterating through the collection of collected Switch Uses
    //                         // check if the name of the the variable the use list goes to
    //                         // matches with the slice profile variable name
    //                         if (data.first != initDeclItem.first) continue;

    //                         // While iterating the collection of use line numbers
    //                         // we want to verify that the uses are after the initial decl line
    //                         // and we want to check if the use number is not contained inside
    //                         // an outside function
    //                         for (auto nums : data.second) {
    //                             if (nums < initDeclItem.second) continue;
    //                             if (nums > (*nextFunc)->second.lineNumber - 1) continue;
    //                             sliceProfile.uses.insert(nums);
    //                         }
    //                     }

    //                     for (auto data : *sliceEventData->switchDefs) {
    //                         // When iterating through the collection of collected Switch Defs
    //                         // check if the name of the the variable the def list goes to
    //                         // matches with the slice profile variable name
    //                         if (data.first != initDeclItem.first) continue;

    //                         // While iterating the collection of def line numbers
    //                         // we want to verify that the defs are after the initial decl line
    //                         // and we want to check if the def number is not contained inside
    //                         // an outside function
    //                         for (auto nums : data.second) {
    //                             if (nums < initDeclItem.second) continue;
    //                             sliceProfile.definitions.insert(nums);
    //                         }
    //                     }
    //                 }
    //             } else { // We're on the last/only function
    //                 // Check if the variables initial declaration is within the final functions scope
    //                 if (initDeclItem.second >= (*func)->second.lineNumber) {
    //                     // Uses the same logic as up above
    //                     for (auto data : *sliceEventData->switchUses) {
    //                         if (data.first != initDeclItem.first) continue;
    //                         for (auto nums : data.second) {
    //                             if (nums < initDeclItem.second) continue;
    //                             sliceProfile.uses.insert(nums);
    //                         }
    //                     }

    //                     for (auto data : *sliceEventData->switchDefs) {
    //                         if (data.first != initDeclItem.first) continue;
    //                         for (auto nums : data.second) {
    //                             if (nums < initDeclItem.second) continue;
    //                             sliceProfile.definitions.insert(nums);
    //                         }
    //                     }
    //                 }
    //             }
    //         }   
    //     }
    // }

    // split into 3 readable functions
    // void PassOver() {
    //     // Create a set of data representing function scopes
    //     // in ascending order from line number
    //     std::map<std::string, unsigned int> functionBounds;
    //     for (auto funcSig : sliceEventData->functionSigMap) {
    //         functionBounds[funcSig.first] = funcSig.second.lineNumber;
    //     }

    //     // Pass Over to Update any errors from slices first run
    //     for (auto mapItr = profileMap.begin(); mapItr != profileMap.end(); ++mapItr) {
    //         for (auto sliceItr = mapItr->second.begin(); sliceItr != mapItr->second.end(); ++sliceItr) {
    //             if (sliceItr->containsDeclaration) {
    //                 // Variables that are reference variables should not carry aliases
    //                 if (sliceItr->isReference) {
    //                     sliceItr->aliases.clear();
    //                 }

    //                 // Remove def lines concerning lines where params are
    //                 // declared and moving those lines to use

    //                 /*
    //                 --------------------------------------
    //                 1    int bar (int x) {
    //                 2        return ++x; // def use
    //                 3    }
    //                 4    int main () {
    //                 5        int y = 0;
    //                 6        bar(y);
    //                 7        std::cout << y << std::endl;
    //                 8        return 0;
    //                 9    }
    //                 --------------------------------------
    //                 For the following, profile y should place
    //                 lines 1 and 2 as a use and not a def.

                    
    //                 --------------------------------------
    //                 1    int bar (int x) {
    //                 2        x = 5; // def
    //                 3        return x; // use
    //                 4    }
    //                 5    int main () {
    //                 6        int y = 0;
    //                 7        bar(y);
    //                 8        std::cout << y << std::endl;
    //                 9        return 0;
    //                 10   }
    //                 --------------------------------------
    //                 For the following, profile y should place
    //                 lines 1, 3. Should be uses but 2 concerning
    //                 redefining x we dont want to add this as a use
    //                 */

    //                 std::unordered_set <std::string> junkMap; // Need this to use the ArgumentProfile function

    //                 // Ensuring for called function resolution that the line
    //                 // number where the slice profile points to a function param
    //                 // is not treated as a definition but as a use of the slice
    //                 for (auto line : *sliceEventData->possibleDefinitions) {
    //                     // within the slice does the line exist within the def set
    //                     if ( sliceItr->definitions.find(line) != sliceItr->definitions.end() ) {
    //                         // Check the cfunctions to see if defs need switched
    //                         for (auto cfunctData : sliceItr->cfunctions) {
    //                             std::string name = cfunctData.first;
                                
    //                             auto funct = sliceEventData->funcDefMap.find(name);
    //                             if (funct != sliceEventData->funcDefMap.end()) {
    //                                 auto Spi = ArgumentProfile(*sliceEventData->functionSigMap.find(name), std::atoi(cfunctData.second.first.c_str()) - 1, junkMap);
    //                                 auto sliceParamItr = Spi->second.begin();

    //                                 for (auto lineNum : funct->second) {
    //                                     // ensure the defs contains the line number before swapping
    //                                     if (sliceItr->definitions.find(lineNum) != sliceItr->definitions.end()) {
    //                                         // Incase we run into a recursive function that the sliceParamItr points back to sliceItr
    //                                         // we dont want to remove its true definition
    //                                         if (sliceParamItr != sliceItr) {
    //                                             sliceItr->definitions.erase(lineNum);
    //                                         }
    //                                         sliceItr->uses.insert(lineNum);
    //                                     }
    //                                 }
    //                             }
    //                         }
    //                     }
    //                 }


    //                 // Iterate through called function data if the slice has any data
    //                 // and removing the same false definition data like above
    //                 for (auto cfunctData : sliceItr->cfunctions) {
    //                     std::string name = cfunctData.first;
    //                     auto funct = sliceEventData->functionSigMap.find(name);
    //                     if (funct != sliceEventData->functionSigMap.end()) {
    //                         // If we get a valid find on a function shown in a profiles called functions
    //                         // we want to extract the slice profile associated in the function params
    //                         auto Spi = ArgumentProfile(*funct, std::atoi(cfunctData.second.first.c_str()) - 1, junkMap);
    //                         auto sliceParamItr = Spi->second.begin();
    //                         std::string paramVarName = funct->second.parameters.at(std::atoi(cfunctData.second.first.c_str()) - 1).nameOfIdentifier;

    //                         // If we run into recursive algorithms we want to ensure
    //                         // we dont hit an infinite loop due to the sliceItr pointing
    //                         // to the same object as sliceParamItr
    //                         if (sliceParamItr == sliceItr) break;

    //                         for (auto sliceParamItr = Spi->second.begin(); sliceParamItr != Spi->second.end(); ++sliceParamItr) {
    //                             if (sliceParamItr->containsDeclaration) {
    //                                 if (sliceParamItr->function == name && *(sliceParamItr->definitions.begin()) == funct->second.lineNumber && sliceParamItr->variableName == paramVarName ) {
    //                                     // If the sliceParamItr is a pointer or a reference
    //                                     // we want to push the redefinitions of the sliceParamItr
    //                                     // and push the uses of the sliceParamItr to sliceItr

    //                                     if (sliceParamItr->isReference || sliceParamItr->isPointer) {
    //                                         sliceItr->uses.insert(sliceParamItr->uses.begin(), sliceParamItr->uses.end());
    //                                         sliceItr->definitions.insert(sliceParamItr->definitions.begin(), sliceParamItr->definitions.end());

    //                                         // we need to swap the initial decl line from sliceItrs def to a use
    //                                         for (auto initDeclItem : sliceEventData->initDeclData) {
    //                                             // With a collection of data concerning where all variables are initially declared
    //                                             // check if the sliceProfile passed matches with a variable name within the collection
    //                                             if (sliceParamItr->variableName != initDeclItem.first) continue;

    //                                             // Verify the correct slice by checking if the slice definition contains
    //                                             // the initial decl line number for the slice with the matching name
    //                                             if (sliceParamItr->definitions.find(initDeclItem.second) == sliceParamItr->definitions.end()) continue;

    //                                             // Ensure the defLine exists in the sliceItrs definitons list
    //                                             if (sliceItr->definitions.find(initDeclItem.second) != sliceItr->definitions.end()) {
    //                                                 sliceItr->definitions.erase(initDeclItem.second);
    //                                                 sliceItr->uses.insert(initDeclItem.second);
    //                                             }
    //                                         }
    //                                     } else {
    //                                         // Once we have the correct parameter slice, we need to compare its
    //                                         // uses and defs to whats present in sliceItr and remove some non-initial defs
    //                                         // from the slices as the use of sliceItr concerning params should only have the
    //                                         // line where the param was initially declared, we dont want to store pure redefinitons
    //                                         // at this point in time
    //                                         for (auto defLines : sliceParamItr->definitions) {
    //                                             // Ensure the defLine exists in the sliceItrs definitons list
    //                                             if (sliceItr->definitions.find(defLines) != sliceItr->definitions.end()) {
    //                                                 sliceItr->definitions.erase(defLines);
    //                                             }
    //                                         }
    //                                     }

    //                                     break;
    //                                 }
    //                             }
    //                         }
    //                     }
    //                 }

    //                 // Aliases are to be local within scope of their focused slice
    //                 for (auto alias = sliceItr->aliases.begin(); alias != sliceItr->aliases.end();) {
    //                     bool removedData = false;

    //                     if (sliceEventData->functionCallList.find(alias->second) != sliceEventData->functionCallList.end()) {
    //                         // remove aliases formed at the occurance of a function call
    //                         sliceItr->aliases.erase(alias++);
    //                     } else {
    //                         // check if the line where the alias is formed is within
    //                         // local scope of the sliceItr variable
    //                         unsigned int localScopeStart = functionBounds[sliceItr->function];

    //                         // if the alias was formed in a function above the local scope
    //                         // we can assume it is not within local scope
    //                         if (alias->second < localScopeStart) {
    //                             sliceItr->aliases.erase(alias++);
    //                             removedData = true;
    //                         } else {
    //                             // define the rough estimated end of local scope
    //                             unsigned int endOfLocalScope = 0;
    //                             for (auto data : functionBounds) {
    //                                 if (data.second > localScopeStart) {
    //                                     endOfLocalScope = data.second;
    //                                     break;
    //                                 }
    //                             }

    //                             // if the alias is formed farther down then the
    //                             // end of the local scope remove this alias
    //                             if (endOfLocalScope < alias->second) {
    //                                 if (endOfLocalScope == 0) break;
    //                                 sliceItr->aliases.erase(alias++);
    //                                 removedData = true;
    //                             }
    //                         }

    //                         if (!removedData) ++alias;
    //                     }
    //                 }

    //                 // Dvars are to be local within scope of their focused slice
    //                 for (auto dvar = sliceItr->dvars.begin(); dvar != sliceItr->dvars.end();) {
    //                     bool removedData = false;

    //                     if (sliceEventData->functionCallList.find(dvar->second) != sliceEventData->functionCallList.end()) {
    //                         // remove aliases formed at the occurance of a function call
    //                         sliceItr->dvars.erase(dvar++);
    //                     } else {
    //                         // check if the line where the alias is formed is within
    //                         // local scope of the sliceItr variable
    //                         unsigned int localScopeStart = functionBounds[sliceItr->function];

    //                         // if the alias was formed in a function above the local scope
    //                         // we can assume it is not within local scope
    //                         if (dvar->second < localScopeStart) {
    //                             sliceItr->dvars.erase(dvar++);
    //                             removedData = true;
    //                         } else {
    //                             // definte the rough estimated end of local scope
    //                             unsigned int endOfLocalScope = 0; // if no end of scope is found we are viewing the scope of the final function
    //                             for (auto data : functionBounds) {
    //                                 if (data.second > localScopeStart) {
    //                                     endOfLocalScope = data.second;
    //                                     break;
    //                                 }
    //                             }

    //                             // if the alias is formed farther down then the
    //                             // end of the local scope remove this alias
    //                             if (endOfLocalScope < dvar->second) {
    //                                 if (endOfLocalScope == 0) break;

    //                                 sliceItr->dvars.erase(dvar++);
    //                                 removedData = true;
    //                             }
    //                         }

    //                         if (!removedData) ++dvar;
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     // Update Dvars
    //     for (auto dvarData : *sliceEventData->possibleDvars) {
    //         for (auto slice : dvarData.dvars) {
    //             // by using the pair we can find the correct slice profile
    //             // we will insert dvarData.lhsName into, along with using
    //             // other data we have collected
    //             auto Spi = profileMap.find(slice.first);
    //             for (auto sliceParamItr = Spi->second.begin(); sliceParamItr != Spi->second.end(); ++sliceParamItr) {
    //                 if (sliceParamItr->containsDeclaration) {
    //                     if (sliceParamItr->function != dvarData.function) {
    //                         continue;
    //                     }
    //                     if (sliceParamItr->uses.find(dvarData.lhsDefLine) == sliceParamItr->uses.end()) {
    //                         continue;
    //                     }

    //                     sliceParamItr->dvars.insert(std::make_pair(dvarData.lhsName, dvarData.lhsDefLine));
    //                 }
    //             }
    //         }
    //     }
    // }

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;
	    for (std::pair<std::string, std::vector<SliceProfile>> var : profileMap) {
            // Need to watch the Slices we attempt to dig into because we are collecting slices we have no interest in
            if (!profileMap.find(var.first)->second.back().visited && (var.second.back().variableName != "*LITERAL*")) {
                if (!var.second.back().cfunctions.empty()) {
                    for (auto cfunc : var.second.back().cfunctions) {
                        auto funcIt = funcSigCollection.functionSigMap.find(cfunc.first);
                        if(funcIt != funcSigCollection.functionSigMap.end()) {
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

                                if (profileMap.find(var.first) != profileMap.end() && profileMap.find(Spi->first) != profileMap.end() && sliceItr != Spi->second.end()) {
                                    std::cout << std::boolalpha << "[*] " << sliceItr->variableName << " | Ref -> " << sliceItr->isReference << ", Ptr -> " << sliceItr->isPointer << std::endl;
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
                                    std::cout << std::boolalpha << (profileMap.find(var.first) != profileMap.end()) << " && " << (profileMap.find(Spi->first) != profileMap.end()) << " && " << (sliceItr != Spi->second.end()) << std::endl;
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

    /*
    void ComputeControlPaths() {
        for (std::pair<std::string, std::vector<SliceProfile>> var : profileMap) {
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
                            profileMap.find(var.first)->second.back().controlEdges.insert(
                                    std::make_pair(predecessor, trueSuccessor));
                        }
                    }

                    if (predecessor != falseSuccessor) {
                        profileMap.find(var.first)->second.back().controlEdges.insert(
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
                            profileMap.find(var.first)->second.back().controlEdges.insert(
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
                            profileMap.find(var.first)->second.back().controlEdges.insert(
                                    std::make_pair(prevSL, sLines[i]));
                        }

                        prevSL = 0;
                    }
                }
            }
        }
    }
    */

protected:
    std::any DataInner() const override {
        return (void *) 0; // export profile to listeners
    }

private:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    std::vector<std::shared_ptr<ClassData>> classInfo;
    std::vector<std::shared_ptr<FunctionData>> functionInfo;
    FunctionSignatureData funcSigCollection;

    // Process the class and function information collected
    void PrintCollection() {
        std::cout << "================= DEBUG ===================" << std::endl;
        
        std::cout << ":::: Class Info ::::" << std::endl;

        for (std::shared_ptr<ClassData> data : classInfo) {
            std::cout << "Class: " << data->name->SimpleName() << std::endl;
            std::cout << "Language: " << data->language << std::endl;
            std::cout << "Filename: " << data->filename << std::endl;
            std::cout << "Fields: " << std::endl;
            for (unsigned int j=0; j<data->fields[ClassData::PUBLIC].size(); ++j) {
                std::cout << " " << data->fields[ClassData::PUBLIC][j]->name->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->fields[ClassData::PROTECTED].size(); ++j) {
                std::cout << " " << data->fields[ClassData::PROTECTED][j]->name->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->fields[ClassData::PRIVATE].size(); ++j) {
                std::cout << " " << data->fields[ClassData::PRIVATE][j]->name->ToString() << std::endl;
            }
            std::cout << "Methods: " << std::endl;
            for (unsigned int j=0; j<data->methods[ClassData::PUBLIC].size(); ++j) {
                std::cout << " " << data->methods[ClassData::PUBLIC][j]->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->methods[ClassData::PROTECTED].size(); ++j) {
                std::cout << " " << data->methods[ClassData::PROTECTED][j]->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->methods[ClassData::PRIVATE].size(); ++j) {
                std::cout << " " << data->methods[ClassData::PRIVATE][j]->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->operators[ClassData::PUBLIC].size(); ++j) {
                std::cout << " " << data->operators[ClassData::PUBLIC][j]->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->operators[ClassData::PROTECTED].size(); ++j) {
                std::cout << " " << data->operators[ClassData::PROTECTED][j]->ToString() << std::endl;
            }
            for (unsigned int j=0; j<data->operators[ClassData::PRIVATE].size(); ++j) {
                std::cout << " " << data->operators[ClassData::PRIVATE][j]->ToString() << std::endl;
            }
            std::cout << std::endl;
        }

        std::cout << ":::: Function Info ::::" << std::endl;

        for (std::shared_ptr<FunctionData> data : functionInfo) {
            std::cout << "Function: " << *(data->name) << std::endl;
            std::cout << "Language: " << data->language << std::endl;
            std::cout << "Filename: " << data->filename << std::endl;
            std::cout << "  " << data->ToString() << std::endl;
            std::cout << "  Locals:" << std::endl;
            for(std::size_t pos = 0; pos < data->block->locals.size(); ++pos) {
                std::cout << "   " <<  *(data->block->locals[pos]) << std::endl;
            }
            std::cout << "  Returns: " << data->block->returns.size() << std::endl;
            for(std::size_t pos = 0; pos < data->block->returns.size(); ++pos) {
                std::cout << "   " << *(data->block->returns[pos]) << std::endl;
            }
            std::cout << "  Expressions: " << data->block->expr_stmts.size() << std::endl;
            for(std::size_t pos = 0; pos < data->block->expr_stmts.size(); ++pos) {
                std::cout << "   " << *(data->block->expr_stmts[pos]) << std::endl;
            }

            std::cout << "  Conditions: " << data->block->conditionals.size() << std::endl;
            for(std::size_t pos = 0; pos < data->block->conditionals.size(); ++pos) {
                // std::cout << "   " << *(data->block->conditionals[pos]) << std::endl;
            }

            std::cout << std::endl;
        }
        std::cout << "===========================================" << std::endl;
    }

    /*
    void SrcSliceFinalize() {
        for (auto it = profileMap.begin(); it != profileMap.end(); ++it) {
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
        for (auto& profile : profileMap) {
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
    }
    */
};


#endif