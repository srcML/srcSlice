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

    // Use literal string filename ctor of srcSAXController (srcslice cpp main)
    SrcSliceHandler(const char* filename, bool v, bool ce, std::initializer_list<srcDispatch::PolicyListener *> listeners = {})
            : srcDispatch::PolicyDispatcher(listeners), verboseMode(v), calculateControlEdges(ce) {
        srcSAXController control(filename);
        srcDispatch::srcDispatcherSingleEvent<UnitPolicy> handler(this);
        control.parse(&handler); // Start parsing
        
        // Handles Collecting Control-Edges
        if (calculateControlEdges) ComputeControlPaths();

        ComputeInterprocedural();
    }

    // Use string srcml buffer ctor of srcSAXController
    SrcSliceHandler(const std::string& sourceCodeStr, std::initializer_list<srcDispatch::PolicyListener *> listeners = {})
            : srcDispatch::PolicyDispatcher(listeners), verboseMode(false) {
        srcSAXController control(sourceCodeStr);
        srcDispatch::srcDispatcherSingleEvent<UnitPolicy> handler(this);
        control.parse(&handler); // Start parsing
        
        // Handles Collecting Control-Edges
        ComputeControlPaths();

        ComputeInterprocedural();
    }

    void Notify(const PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) override {
        if(typeid(ClassPolicy) == typeid(*policy)) {
            ProcessClassData(policy->Data<ClassData>(), ctx);
        } else if(typeid(FunctionPolicy) == typeid(*policy)) {
            ProcessFunctionData(policy->Data<FunctionData>(), "", policy->Data<FunctionData>()->namespaces, ctx);
        } else if (typeid(DeclTypePolicy) == typeid(*policy)) {
            ProcessDeclStmts(nullptr, nullptr, "", policy->Data<std::vector<std::shared_ptr<DeclData>>>(), ctx);
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}

    void ProcessFunctionData(std::shared_ptr<FunctionData> function_data, std::string className,
                            std::vector<std::string>& containingNamespaces, const srcDispatch::srcSAXEventContext& ctx) {
        ProcessFunctionSignature(function_data, className, containingNamespaces, ctx);
        ProcessDeclStmts(function_data, nullptr, className, nullptr, ctx);
        ProcessExprStmts(function_data, className, ctx);
    }

    void ProcessClassData(std::shared_ptr<ClassData> class_data, const srcDispatch::srcSAXEventContext& ctx) {
        if (class_data) {
            std::string className = "";

            if (class_data->name) {
                className = class_data->name->ToString();
            }

            // Process Class Member Variables
            ProcessDeclStmts(nullptr, class_data, className, nullptr, ctx);

            // Process Class Contructors
            for (auto& funcVec : class_data->constructors) {// [ vect<func>, vect<func>, vect<func> ]
                for (auto& func : funcVec)
                    ProcessFunctionData(func, className, class_data->namespaces, ctx);
            }

            // Process Class Methods (Member Functions)
            for (auto& funcVec : class_data->methods) {// [ vect<func>, vect<func>, vect<func> ]
                for (auto& func : funcVec)
                    ProcessFunctionData(func, className, class_data->namespaces, ctx);
            }

            // Process Operator Overloading
            for (auto& funcVec : class_data->operators) {// [ vect<func>, vect<func>, vect<func> ]
                for (auto& func : funcVec)
                    ProcessFunctionData(func, className, class_data->namespaces, ctx);
            }
        }
    }

    void ProcessDeclStmts(std::shared_ptr<FunctionData> funcData, const std::shared_ptr<ClassData> classData,
                            std::string className, std::shared_ptr<std::vector<std::shared_ptr<DeclData>>> potentialGlobals,
                            const srcDispatch::srcSAXEventContext& ctx) {
        std::vector<std::shared_ptr<DeclData>> localGroup;
        std::vector<std::string> containingNamespaces;

        if (funcData) {
            // Capture general locals (decls)
            if (funcData->block) {
                if (funcData->block->locals.size() > 0) {
                    localGroup.insert(localGroup.end(), funcData->block->locals.begin(), funcData->block->locals.end());
                }

                // Capture Conditional locals (decls)
                if (funcData->block->conditionals.size() > 0) {
                    CollectConditionalData(nullptr, &localGroup, funcData->block->conditionals);
                }
            }

            // Get containing namespaces from functionData
            containingNamespaces = funcData->namespaces;
        }

        // Look at all declared members from each class field
        if (classData) {
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

            if (classData->name)
                className = classData->name->ToString();

            // Get containing namespaces from classData
            containingNamespaces = classData->namespaces;
        }

        // Look at potential globals that are captured
        if (potentialGlobals) {
            localGroup.insert(localGroup.end(), potentialGlobals->begin(), potentialGlobals->end());

            // capture containing namespaces for potential globals
            containingNamespaces = ctx.currentNamespaces;
        }

        // loop through all the expression statements within Decl Statements
        for (const auto localVar : localGroup) {
            if (!localVar) continue;
            if (!localVar->name) continue;

            std::vector<std::shared_ptr<VariableData>> varDataGroup;

            if (localVar->init) {
                varDataGroup = ParseExpr(*localVar->init, localVar->init->lineNumber);
            }

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
                    sliceProfile.containingNameSpaces = containingNamespaces;
                    sliceProfile.language = ctx.currentFileLanguage;

                    sliceProfile.showControlEdges = calculateControlEdges;

                    sliceProfile.isPointer = isPointer;
                    sliceProfile.isReference = isReference;

                    sliceProfileItr->second.push_back(sliceProfile);
                    sliceProfileItr->second.back().containsDeclaration = true;
                }
            } else {
                auto sliceProf = SliceProfile(declVarName, localVar->lineNumber, (isPointer), false, std::set<unsigned int>{localVar->lineNumber});

                sliceProf.nameOfContainingClass = className;
                sliceProf.containingNameSpaces = containingNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.showControlEdges = calculateControlEdges;

                sliceProf.containsDeclaration = true;

                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                // point the iterator to the newly inserted profile element
                sliceProfileItr = profileMap.insert(std::make_pair(declVarName, std::vector<SliceProfile>{ std::move(sliceProf) })).first;
            }

            sliceProfileItr->second.back().isReference = isReference;
            sliceProfileItr->second.back().isPointer = isPointer;

            // This allows me to set the data type of the variable in its slice
            // after its been set up from the logic above here
            // Set the data-type of sliceprofile for decl vars inside of function bodies

            sliceProfileItr->second.back().variableType = declVarType;

            // Link the filepath the XML Originates
            sliceProfileItr->second.back().file = ctx.currentFilePath;

            // Link the file hash attribute
            sliceProfileItr->second.back().checksum = ctx.currentFileChecksum;

            // Link the function this slice is located in
            if (funcData) {
                if (funcData->name)
                    sliceProfileItr->second.back().function = funcData->name->ToString();
            }

            // Link the class this slice is located in
            if (classData) {
                if (classData->name)
                    sliceProfileItr->second.back().nameOfContainingClass = classData->name->ToString();
            }

            // Look at the dvars and add this current variable to their dvar's lists.
            // If we haven't seen this name before, add its slice profile
            for (auto& varData : varDataGroup) {
                UpdateLHSSlices(varData);
                // sliceProfileItr->second.back() is the LHS
                if (sliceProfileItr != profileMap.end()) {
                    SliceProfile* initRHS = FetchSliceProfile(varData->GetNameOfIdentifier(), varData, funcData, className, containingNamespaces);

                    if (initRHS == nullptr) {
                        // std::cerr << "Slice Fetch Unsuccessful!" << std::endl;
                        continue;
                    }

                    if (sliceProfileItr->second.back().potentialAlias) {
                        // Aliases are formed from pointer-to-pointer or pointer-to-address assignments
                        if (!initRHS->isPointer && !varData->isAddrOf) {
                            // ensure dependencies are within local-scope
                            if (initRHS->function == sliceProfileItr->second.back().function) {
                                initRHS->dvars.insert(std::make_pair(declVarName, localVar->lineNumber));
                            }
                            continue;
                        }
                        if (varData->GetNameOfIdentifier() != sliceProfileItr->second.back().variableName) {
                            // ensure aliases are within local-scope
                            if (initRHS->function == sliceProfileItr->second.back().function) {
                                if (sliceProfileItr->second.back().isPointer) {
                                    // Check for pointer-2-pointer assignment or points-to-address assignment
                                    if (initRHS->isPointer || varData->isAddrOf) {
                                        sliceProfileItr->second.back().aliases.insert(std::make_pair(varData->GetNameOfIdentifier(), varData->originLine));
                                    }
                                } else if (sliceProfileItr->second.back().isReference) {
                                    sliceProfileItr->second.back().aliases.insert(std::make_pair(varData->GetNameOfIdentifier(), varData->originLine));
                                }
                            }
                        }
                    } else {
                        // ensure dependencies are within local-scope
                        initRHS->dvars.insert(std::make_pair(declVarName, localVar->lineNumber));
                    }
                }
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
                        sliceProf.containingNameSpaces = containingNamespaces;
                        sliceProf.language = ctx.currentFileLanguage;

                        sliceProf.showControlEdges = calculateControlEdges;

                        auto newSliceProfileFromDeclDvars = profileMap.insert(std::make_pair(dvar,
                                                                                            std::vector<SliceProfile>{
                                                                                                    std::move(sliceProf)
                                                                                            }));
                        if (!StringContainsCharacters(declVarName)) continue;

                        // Ensure dependencies and aliases are within local-scope
                        auto spi = profileMap.find(declVarName);
                        if ( spi != profileMap.end() && (spi->second.back().function == newSliceProfileFromDeclDvars.first->second.back().function) ) {
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
            }
        }
    }

    void ProcessExprStmts(std::shared_ptr<FunctionData> funcData, std::string className, const srcDispatch::srcSAXEventContext& ctx) {
        std::vector<std::shared_ptr<ExpressionData>> exprStmts;
        std::vector<std::string> containingNamespaces;

        // Capture general expressions
        if (funcData->block) {
            if (funcData->block->expr_stmts.size() > 0) {
                exprStmts.insert(exprStmts.end(), funcData->block->expr_stmts.begin(), funcData->block->expr_stmts.end());
            }
        }

        // Capture general Return expressions
        if (funcData->block) {
            if (funcData->block->returns.size() > 0) {
                exprStmts.insert(exprStmts.end(), funcData->block->returns.begin(), funcData->block->returns.end());
            }
        }

        // Capture Conditional expressions
        if (funcData->block) {
            if (funcData->block->conditionals.size() > 0) {
                CollectConditionalData(&exprStmts, nullptr, funcData->block->conditionals);
            }
        }

        if (funcData) containingNamespaces = funcData->namespaces;

        // loop through all the expression statements
        for (auto itr = exprStmts.begin(); itr != exprStmts.end(); ++itr) {
            std::shared_ptr<ExpressionData> expr = *itr;
            if (!expr) continue;

            // Check if any NameData within an Express contains index operator expressions
            // and insert those into the exprStmts vector
            for (const auto& data : expr->expr) {
                if (data.type() == typeid(std::shared_ptr<NameData>)) {
                    std::shared_ptr<NameData> nameData = std::any_cast<std::shared_ptr<NameData>>(data);
                    if (!nameData->indices.empty()) {
                        itr = exprStmts.insert(itr+1, nameData->indices.begin(), nameData->indices.end()); // set to iterator of newly inserted data
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

                    SliceProfile* sliceProfileExprItr = FetchSliceProfile(rhsName, rhsVarData, funcData, className, containingNamespaces);
                    SliceProfile* sliceProfileLHSItr = FetchSliceProfile(lhsName, rhsVarData, funcData, className, containingNamespaces);

                    //Just update definitions and uses if name already exists. Otherwise, add new name.
                    if (sliceProfileExprItr != nullptr) {
                        sliceProfileExprItr->nameOfContainingClass = className;
                        sliceProfileExprItr->containingNameSpaces = containingNamespaces;
                        sliceProfileExprItr->language = ctx.currentFileLanguage;

                        sliceProfileExprItr->uses.insert(rhsVarData->uses.begin(),
                                                                       rhsVarData->uses.end());
                        sliceProfileExprItr->definitions.insert(rhsVarData->definitions.begin(),
                                                                              rhsVarData->definitions.end());

                        if (!StringContainsCharacters(lhsName)) continue;

                        // ensure dependencies and aliases are within local-scope
                        if ( sliceProfileLHSItr != nullptr &&
                            (sliceProfileExprItr->function == sliceProfileLHSItr->function) ) {
                            // Check for pointer-2-pointer assignment or points-to-address assignment
                            if ( (sliceProfileLHSItr->potentialAlias && !varData->dereferenced) &&
                                (sliceProfileExprItr->isPointer || rhsVarData->isAddrOf) ) {
                                if ( lhsName != sliceProfileExprItr->variableName ) {
                                    if (sliceProfileLHSItr->isPointer) {
                                        sliceProfileLHSItr->aliases.insert(std::make_pair(rhsName, rhsVarData->originLine));
                                    }
                                }
                                continue;
                            }

                            // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                            // IE : abc = abc + i;
                            if (!StringContainsCharacters(lhsName)) continue;
                            if (!lhsName.empty() && sliceProfileExprItr->variableName != lhsName) {
                                sliceProfileExprItr->dvars.insert(std::make_pair(lhsName, varData->originLine));
                                continue;
                            }
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
                        sliceProfileExprItr2.first->second.back().containingNameSpaces = containingNamespaces;
                        sliceProfileExprItr2.first->second.back().language = ctx.currentFileLanguage;

                        sliceProfileExprItr2.first->second.back().showControlEdges = calculateControlEdges;

                        if (!StringContainsCharacters(lhsName)) continue;

                        if (sliceProfileLHSItr != nullptr && sliceProfileExprItr != nullptr) {
                            // ensure dependencies and aliases are within local-scope
                            if (sliceProfileExprItr->function == sliceProfileLHSItr->function) {
                                if (sliceProfileLHSItr->potentialAlias) {
                                    if ( lhsName != sliceProfileLHSItr->variableName ) {
                                        if (sliceProfileLHSItr->isPointer) {
                                            // Check for pointer-2-pointer assignment or points-to-address assignment
                                            if (sliceProfileExprItr->isPointer || rhsVarData->isAddrOf) {
                                                sliceProfileLHSItr->aliases.insert(std::make_pair(rhsName, rhsVarData->originLine));
                                            }
                                        }
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

        sliceProfile.cfunctions.insert(sliceCallData);
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
                    if (!name) continue;

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
                                if (verboseMode)
                                    std::cout << "[-] Fingerprint Not Found for -> " << simpleFunctionName << std::endl;
                                CreateSliceCallData(simpleFunctionName, argIndex, 0, sliceProfileItr->second.back());
                            }
                        } else {
                            if (verboseMode)
                                std::cout << "[-] No Function Signature Found for -> " << simpleFunctionName << std::endl;
                            CreateSliceCallData(simpleFunctionName, argIndex, 0, sliceProfileItr->second.back());
                        }
                    }
                }
            }
        }
    }

    void CollectConditionalData(std::vector<std::shared_ptr<ExpressionData>>* exprStmts, std::vector<std::shared_ptr<DeclData>>* declStmts,
                                std::vector<std::any>& conditionals) {
        std::vector<std::shared_ptr<BlockData>> cntlBlocks;

        // identify what conditional type we are viewing and handle logic accordingly
        for (const auto& cntl : conditionals) {
            if (cntl.type() == typeid(std::shared_ptr<IfStmtData>)) {
                // Extract all of the block data from if statements
                std::shared_ptr<IfStmtData> ifcntl = std::any_cast<std::shared_ptr<IfStmtData>>(cntl);

                // ifstmts have three potential clauses (if-elseif-else)
                for (const auto& clause : ifcntl->clauses) {
                    if (clause.type() == typeid(std::shared_ptr<IfData>)) {
                        std::shared_ptr<IfData> ifData = std::any_cast<std::shared_ptr<IfData>>(clause);

                        if (declStmts) {
                            declStmts->insert(declStmts->end(), ifData->condition->decls.begin(), ifData->condition->decls.end());
                        }

                        if (exprStmts) {
                            exprStmts->push_back(ifData->condition->expr);
                        }

                        // minimize duplicate entries
                        if (ifdata.size() == 0 || ifdata.back() != std::make_pair((int)(ifData->startLineNumber), (int)(ifData->endLineNumber)))
                            ifdata.push_back(std::make_pair(ifData->startLineNumber, ifData->endLineNumber));

                        cntlBlocks.push_back(ifData->block);
                    } else if (clause.type() == typeid(std::shared_ptr<ElseIfData>)) {
                        std::shared_ptr<ElseIfData> elseIfData = std::any_cast<std::shared_ptr<ElseIfData>>(clause);

                        if (declStmts) {
                            declStmts->insert(declStmts->end(), elseIfData->condition->decls.begin(), elseIfData->condition->decls.end());
                        }

                        if (exprStmts) {
                            exprStmts->push_back(elseIfData->condition->expr);
                        }

                        // minimize duplicate entries, pushed into ifdata for the ComputeControlPaths Algorithm
                        if (ifdata.size() == 0 || ifdata.back() != std::make_pair((int)(elseIfData->startLineNumber), (int)(elseIfData->endLineNumber)))
                            ifdata.push_back(std::make_pair(elseIfData->startLineNumber, elseIfData->endLineNumber));
                        
                        // track elseif block data for later usage
                        if (elseifdata.size() == 0 || elseifdata.back() != std::make_pair((int)(elseIfData->startLineNumber), (int)(elseIfData->endLineNumber)))
                            elseifdata.push_back(std::make_pair(elseIfData->startLineNumber, elseIfData->endLineNumber));

                        cntlBlocks.push_back(elseIfData->block);
                    } else if (clause.type() == typeid(std::shared_ptr<ElseData>)) {
                        std::shared_ptr<ElseData> elseData = std::any_cast<std::shared_ptr<ElseData>>(clause);

                        // minimize duplicate entries
                        if (elsedata.size() == 0 || elsedata.back() != std::make_pair((int)(elseData->startLineNumber), (int)(elseData->endLineNumber)))
                            elsedata.push_back(std::make_pair(elseData->startLineNumber, elseData->endLineNumber));
                        cntlBlocks.push_back(elseData->block);
                    }
                }
            } else if (cntl.type() == typeid(std::shared_ptr<SwitchData>)) {
                // Extract all of the block data from Switch statements
                std::shared_ptr<SwitchData> switchData = std::any_cast<std::shared_ptr<SwitchData>>(cntl);

                /*
                    Ensure we are getting the uses from the case lines
                    Ensure we capture data from the case blocks as well
                */

                // Connect the use lines of switch cases to their corresponding control variables
                std::vector<std::shared_ptr<NameData>> controlVariables;
                for (const auto& exprElem : switchData->condition->expr->expr) {
                    if (exprElem.type() == typeid(std::shared_ptr<NameData>)) {
                        controlVariables.push_back(std::any_cast<std::shared_ptr<NameData>>(exprElem));
                    }
                }

                for (const auto& switchCase : switchData->block->cases) {
                    for (auto& ctrlVar : controlVariables) {
                        // locate the slice profile of the ctrlVar and insert the uses
                        auto sliceProfileItr = profileMap.find(ctrlVar->ToString());

                        // might need to add finger-printing to minimize potential issue
                        // of inserting data into the wrong slice
                        if (sliceProfileItr != profileMap.end()) {
                            sliceProfileItr->second.back().uses.insert(switchCase->lineNumber);
                        }
                    }
                }

                if (declStmts) {
                    declStmts->insert(declStmts->end(), switchData->condition->decls.begin(), switchData->condition->decls.end());
                }

                if (exprStmts) {
                    exprStmts->push_back(switchData->condition->expr);
                }

                cntlBlocks.push_back(switchData->block);
            } else if (cntl.type() == typeid(std::shared_ptr<ForData>)) {
                // Extract all of the block data from For Loops
                std::shared_ptr<ForData> forData = std::any_cast<std::shared_ptr<ForData>>(cntl);

                if (declStmts != nullptr) {
                    for (auto& initData : forData->control->init) {
                        if (initData.type() == typeid(std::shared_ptr<DeclData>)) {
                            std::shared_ptr<DeclData> forInitDecl = std::any_cast<std::shared_ptr<DeclData>>(initData);
                            declStmts->push_back(forInitDecl);
                        }
                    }
                }

                if (exprStmts) {
                    exprStmts->push_back(forData->control->condition->expr);
                }

                loopdata.push_back(std::make_pair(forData->startLineNumber, forData->endLineNumber));
                forloopdata.push_back(std::make_pair(forData->startLineNumber, forData->endLineNumber));

                cntlBlocks.push_back(forData->block);
            }  else if (cntl.type() == typeid(std::shared_ptr<WhileData>)) {
                // Extract all of the block data from While Loops
                std::shared_ptr<WhileData> whileData = std::any_cast<std::shared_ptr<WhileData>>(cntl);

                // C++ do-while does not support decl-stmts within the conditional "()"

                if (exprStmts) {
                    exprStmts->push_back(whileData->condition->expr);
                }

                loopdata.push_back(std::make_pair(whileData->startLineNumber, whileData->endLineNumber));
                whileloopdata.push_back(std::make_pair(whileData->startLineNumber, whileData->endLineNumber));

                cntlBlocks.push_back(whileData->block);
            } else if (cntl.type() == typeid(std::shared_ptr<DoData>)) {
                // Extract all of the block data from Do-While Loops
                std::shared_ptr<DoData> doWhileData = std::any_cast<std::shared_ptr<DoData>>(cntl);

                // C++ do-while does not support decl-stmts within the conditional "()"

                if (exprStmts) {
                    exprStmts->push_back(doWhileData->condition->expr);
                }

                loopdata.push_back(std::make_pair(doWhileData->startLineNumber, doWhileData->endLineNumber));
                dowhileloopdata.push_back(std::make_pair(doWhileData->startLineNumber, doWhileData->endLineNumber));

                cntlBlocks.push_back(doWhileData->block);
            }
        }

        // Capture the Expressions and Decl-Stmts from the conditonal blocks
        for (const auto& block : cntlBlocks) {
            if (!block) continue;

            if (declStmts) {
                if (block->locals.size() > 0)
                    declStmts->insert(declStmts->end(), block->locals.begin(), block->locals.end());
            }

            if (exprStmts) {
                if (block->expr_stmts.size() > 0)
                    exprStmts->insert(exprStmts->end(), block->expr_stmts.begin(), block->expr_stmts.end());
                if (block->returns.size() > 0)
                    exprStmts->insert(exprStmts->end(), block->returns.begin(), block->returns.end());
            }

            // Recursive call to step into nested conditionals
            if (block->conditionals.size() > 0) {
                CollectConditionalData(exprStmts, declStmts, block->conditionals);
            }
        }
    }

    // Take large name strings and extract the root variable name
    std::string ExtractName(std::string elementName) {
        const std::string target = "std::";

        // Check if the string starts with "std::"
        if (elementName.rfind(target, 0) == 0) {
            // Remove the prefix and set it to the result
            elementName = elementName.substr(target.size());
        }

        std::string varName;
        /*
            *itr.vec.size(); (*root).left
            root->right; obj.data.type

            'a' to 'z': 97 to 122
            'A' to 'Z': 65 to 90
        */
        bool readIn = false;
        for (const auto& c : elementName) {
            bool isLower = (c >= 97 && c <= 122);
            bool isUpper = (c >= 65 && c <= 90);
            bool isNumber = (c >= 48 && c <= 57);
            bool isSpecial = (c == '_') || (c == ':');
            if (isLower || isUpper || isNumber || isSpecial) { // myStr, mystr, MyStr2, my_str_2, myNameSpace::varName
                if (!readIn) readIn = true; // signal we want to read in the first name found
                varName += c;
            } else {
                if (readIn) break; // signal end of first name and break from the loop
            }
        }
        return varName;
    }

    // Attempt to Recursively dig into potential nested indices in a RHS to form dependency relations with a LHS variable
    void AppendIndices(std::shared_ptr<VariableData>& lhs, std::shared_ptr<VariableData>& varData) {
        for (auto& rhs : varData->rhsElems) {
            if (!rhs->indices.empty()) {
                AppendIndices(lhs, rhs);
                lhs->rhsElems.insert(lhs->rhsElems.end(), rhs->indices.begin(), rhs->indices.end());
            }
        }
        if (!varData->indices.empty()) {
            lhs->rhsElems.insert(lhs->rhsElems.end(), varData->indices.begin(), varData->indices.end());
        }
    }

    std::vector<std::shared_ptr<VariableData>> ParseExpr(const ExpressionData& expr, const unsigned int& lineNumber) {
        std::vector<std::shared_ptr<VariableData>> varDataGroup;
        std::string expr_op = "";
        std::shared_ptr<VariableData> lhsVar = std::make_shared<VariableData>();

        std::vector<std::shared_ptr<VariableData>> lhsStack;
        bool groupCollect = false, trailingPrefix = false, trailingExtraction = false;

        // loop through each element within a specific expression statement
        for (const auto& exprElem : expr.expr) {
            if (exprElem.type() == typeid(std::shared_ptr<NameData>)) {
                std::shared_ptr<NameData> name = std::any_cast<std::shared_ptr<NameData>>(exprElem);
                if (name->ToString().empty()) continue;

                std::string varName = ExtractName(name->ToString());

                if (!lhsVar->isInitialized()) {
                    lhsVar->InitializeLHS(varName, lineNumber);

                    // capture use-def chains for single statements such as: ++i
                    if (expr_op == "++" || expr_op == "--" || trailingPrefix) {
                        lhsVar->definitions.insert(lineNumber);
                        lhsVar->uses.insert(lineNumber);
                        trailingPrefix = false;
                    }

                    if (expr_op == "&") {
                        lhsVar->isAddrOf = true;
                    } else if (expr_op == "*") {
                        auto spi = profileMap.find(lhsVar->GetNameOfIdentifier());
                        if (spi != profileMap.end() && spi->second.back().isPointer) {
                            lhsVar->dereferenced = true;
                            if (trailingExtraction) { // redefining a dereferenced ptr does not always use itself when redefining
                                lhsVar->uses.erase(lineNumber);
                            }
                        }
                    }

                    if (expr_op == ">>" || trailingExtraction) {
                        lhsVar->definitions.insert(lineNumber);
                        lhsVar->userModified = true;
                        trailingExtraction = false;
                    }

                    if (!name->indices.empty()) {
                        // Extract the expression within the index operator and push all
                        // of the variable names as rhs of the lhs
                        for (const auto& indexExpr : name->indices) {
                            for (const auto& indexElem : indexExpr->expr) {
                                if (indexElem.type() == typeid(std::shared_ptr<NameData>)) {
                                    // Create the new index data reference
                                    std::shared_ptr<NameData> indexVar = std::any_cast<std::shared_ptr<NameData>>(indexElem);
                                    std::string indexVarName = ExtractName(indexVar->ToString());

                                    std::shared_ptr<VariableData> newFalseRHS = std::make_shared<VariableData>(indexVarName);

                                    // Set the meta-data
                                    newFalseRHS->uses.insert(lineNumber);
                                    newFalseRHS->SetOriginLine(lineNumber);

                                    // Push for later handling
                                    lhsVar->indices.push_back(newFalseRHS);
                                }
                            }
                        }
                    }
                } else {
                    std::shared_ptr<VariableData> newRHSVar = std::make_shared<VariableData>(varName);
                    newRHSVar->uses.insert(lineNumber);
                    newRHSVar->SetOriginLine(lineNumber);

                    // capture use-def chains for rhs var statements such as: a = ++i
                    if (expr_op == "++" || expr_op == "--" || trailingPrefix) {
                        newRHSVar->definitions.insert(lineNumber);
                        trailingPrefix = false;
                    }

                    if (expr_op == "&") {
                        newRHSVar->isAddrOf = true;
                    } else if (expr_op == "*") {
                        auto spi = profileMap.find(newRHSVar->GetNameOfIdentifier());
                        if (spi != profileMap.end() && spi->second.back().isPointer) {
                            newRHSVar->dereferenced = true;
                        }
                    }

                    lhsVar->lhs = true;
                    lhsVar->AddRHS(newRHSVar);

                    // Ensure that tracked lhs variables get assigned all of their
                    // rhs vars in the expression
                    for (auto& lhs : lhsStack)
                        lhs->AddRHS(newRHSVar);

                    if (!name->indices.empty()) {
                        // Extract the expression within the index operator and push all
                        // of the variable names as rhs of the lhs
                        for (const auto& indexExpr : name->indices) {
                            for (const auto& indexElem : indexExpr->expr) {
                                if (indexElem.type() == typeid(std::shared_ptr<NameData>)) {
                                    // Create the new index data reference
                                    std::shared_ptr<NameData> indexVar = std::any_cast<std::shared_ptr<NameData>>(indexElem);
                                    std::string indexVarName = ExtractName(indexVar->ToString());

                                    std::shared_ptr<VariableData> newFalseRHS = std::make_shared<VariableData>(indexVarName);

                                    // Set the meta-data
                                    newFalseRHS->uses.insert(lineNumber);
                                    newFalseRHS->SetOriginLine(lineNumber);

                                    // Push for later handling
                                    lhsVar->indices.push_back(newFalseRHS);
                                    for (auto& lhs : lhsStack)
                                        lhs->indices.push_back(newFalseRHS);
                                }
                            }
                        }
                    }
                }
            } else if (exprElem.type() == typeid(std::shared_ptr<OperatorData>)) {
                std::shared_ptr<OperatorData> opData = std::any_cast<std::shared_ptr<OperatorData>>(exprElem);
                expr_op = opData->op;

                // Establish bool-switch to track previous operators for later
                if (expr_op == ">>") {
                    trailingExtraction = true;
                } else if (expr_op == "++" || expr_op == "--" ) {
                    trailingPrefix = true;
                }

                // Setup group handling
                if (expr_op == "(") {
                    groupCollect = true;
                } else if (expr_op == ")") {
                    groupCollect = false;
                    if (lhsStack.size() > 0) {
                        varDataGroup.push_back(lhsVar);
                        lhsVar = lhsStack.back();
                        lhsStack.pop_back();
                    }
                } else if (!lhsVar->rhsElems.empty() || lhsVar->lhs) { // standard handling
                    if (lhsVar->isInitialized()) {
                        // LHS HAS RHS MEMBERS OR LHS HAS BEEN MARKED AS A LHS
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
                                trailingPrefix = true;
                                if (prevRHSPtr != nullptr) {
                                    prevRHSPtr->uses.insert(lineNumber);
                                    prevRHSPtr->definitions.insert(lineNumber);
                                }
                            }

                        }
                    }
                } else {
                    if (lhsVar->isInitialized()) {
                        // IF LHS HAS NO RHS MEMBERS
                        if (isAssignment(expr_op)) {
                            // when LHS hits assignment it can start storing
                            // RHS elements
                            lhsVar->lhs = true;
                            lhsVar->definitions.insert(lineNumber);

                            // Coumpound Assignment is a classic Use-Def Chain
                            // ie: n += 2;
                            if (isCompoundAssignment(expr_op)) {
                                lhsVar->uses.insert(lineNumber);
                                lhsVar->definitions.insert(lineNumber);
                            }
                        } else {
                            // when a LHS has no rhs elems and hits a non-assignment
                            // it gets pushed back into the group and a new LHS gets
                            // created ie. return a + b;
                            if (expr_op == "++" || expr_op == "--" ) {
                                trailingPrefix = true;
                                lhsVar->uses.insert(lineNumber);
                                lhsVar->definitions.insert(lineNumber);
                            } else if (expr_op == "+" || expr_op == "-" || expr_op == "*" || expr_op == "/" || expr_op == "%") {
                                lhsVar->uses.insert(lineNumber);
                            } else if (isLogical(expr_op)) {
                                // anything within logical conditionals are uses
                                // we also will need to redeclare the lhs variable
                                lhsVar->uses.insert(lineNumber);
                            } else if (expr_op == "<<") {
                                lhsVar->uses.insert(lineNumber);
                            }
                            if (lhsVar->isInitialized() && expr_op != "&") {
                                varDataGroup.push_back(lhsVar);
                                lhsVar = std::make_shared<VariableData>();
                            }
                        }
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
            if (lhsVar->rhsElems.size() == 0 && !lhsVar->lhs) {
                if (expr_op != ">>" && !lhsVar->userModified) {
                    // dont add a use for exprs ie cin >> num;
                    lhsVar->uses.insert(lineNumber);
                }
            }

            // Ensure the final potential LHS-RHS pair is pushed into the
            // collection we return
            if (lhsVar->rhsElems.size() > 0) {
                lhsVar->lhs = true;
            }
            varDataGroup.push_back(lhsVar);
        }

        // Iterate all the collected variable data and push their indices into their rhsElems to form dependencies or aliases relations
        for (auto& potentialLHS : varDataGroup) {
            // iterate the rhs elems
            for (auto& rhs : potentialLHS->rhsElems) {
                // if the rhs elem contains indices we need to
                // attempt a recursive search for all of them
                if (!rhs->indices.empty()) {
                    AppendIndices(potentialLHS, rhs);
                }
            }

            if (!potentialLHS->indices.empty()) {
                // push indices of the LHS
                potentialLHS->rhsElems.insert(potentialLHS->rhsElems.end(), potentialLHS->indices.begin(), potentialLHS->indices.end());
            }
        }

        // if (!varDataGroup.empty()) {
        //     std::cerr << "DEBUG PARSE EXPR OUTPUT" << std::endl;
        //     std::cerr << expr << std::endl;
        // }

        // // Debug use/def marking
        // for (const auto& v : varDataGroup) {
        //     std::cerr << v->GetNameOfIdentifier() << " USE { ";
        //     for (const auto& line : v->uses) {
        //         std::cerr << line << " ";
        //     }
        //     std::cerr << " } " << std::endl;

        //     std::cerr << " DEF { ";
        //     for (const auto& line : v->definitions) {
        //         std::cerr << line << " ";
        //     }
        //     std::cerr << " } " << std::endl;
        // }
        // Debug RHS elem assignment
        // for (const auto& v : varDataGroup) {
        //     std::cerr << v->GetNameOfIdentifier() << " { ";
        //     for (const auto& r : v->rhsElems) {
        //         std::cerr << r->GetNameOfIdentifier() << ", ";
        //     }
        //     std::cerr << " } " << std::endl;
        // }

        return varDataGroup;
    }

    void ProcessFunctionParameters(std::vector<std::shared_ptr<DeclData>>& parameters, const std::string& currentFunctionName,
                                    std::string className, std::vector<std::string>& containingNamespaces,
                                    const srcDispatch::srcSAXEventContext& ctx) {
        for (auto& parameter : parameters) {
            if (!parameter->name) continue;
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
                sliceProf.containingNameSpaces = containingNamespaces;
                sliceProf.language = ctx.currentFileLanguage;
                
                sliceProf.showControlEdges = calculateControlEdges;

                sliceProf.isPointer = isPointer;
                sliceProf.isReference = isReference;

                sliceProfileItr->second.push_back(std::move(sliceProf));
            } else {
                auto sliceProf = SliceProfile(paramName, parameter->lineNumber,
                                              isPointer, true,
                                              std::set<unsigned int>{parameter->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = className;
                sliceProf.containingNameSpaces = containingNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.showControlEdges = calculateControlEdges;

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

    void ProcessFunctionSignature(std::shared_ptr<FunctionData> funcData, std::string className, std::vector<std::string>& containingNamespaces,
                                    const srcDispatch::srcSAXEventContext& ctx) {
        std::string functionName = funcData->name->ToString();
        if (functionName.empty()) return;
        bool updateSignature = (functionName.find("::") != std::string::npos);
        // std::string scopeName = "";

        // Update a Signature Entry due to out-of-line definition
        if (updateSignature) {
            // scopeName = functionName.substr(0, functionName.find("::"));
            functionName = functionName.substr(functionName.find_last_of("::")+1, -1);
        }

        // Process the parameters in a separate function
        ProcessFunctionParameters(funcData->parameters, funcData->name->ToString(), className, containingNamespaces, ctx);
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

    // Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
    // Logic constructed for use BEFORE InterProcedural
    SliceProfile* FetchSliceProfile(std::string profileName, const std::shared_ptr<VariableData>& vd, const std::shared_ptr<FunctionData>& funcData,
                                    const std::string& className = "", const std::vector<std::string>& containingNameSpaces = {}) {
        auto spi = profileMap.find(profileName);
        SliceProfile* potentialGlobal = nullptr;

        if (spi != profileMap.end()) {
            // iterate the SliceProfile Vector and perform comparisons
            // When performing comparisons we prioritize local-scope variables over globals
            for (auto& profile : spi->second) {
                if (!profile.containsDeclaration) continue;

                // check if we marked a global, process profile if we are searching for a global
                if (potentialGlobal == nullptr) {
                    // globals do not hold data about source function/class/namespace
                    if (profile.function.empty() && profile.nameOfContainingClass.empty() && profile.containingNameSpaces.empty()) {
                        potentialGlobal = &profile;
                    }
                }

                // perform standard finger printing checks
                if (funcData)
                    if (profile.function != funcData->name->ToString()) continue;
                if (profile.nameOfContainingClass != className) continue;
                if (profile.containingNameSpaces != containingNameSpaces) continue;

                // Check if the numbers collected are less than the lineNumber (init def line)
                if (profile.lineNumber > *(vd->uses.begin())) continue;
                if (*(vd->definitions.begin()) > profile.lineNumber) continue;

                return &profile;
            }

            // if we found a global but no local function variables return the global
            if (potentialGlobal != nullptr)
                return &(*potentialGlobal);
        }
        return nullptr;
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

    // Used for inserting Uses and Defs for Slices in the LHS of an Expression Statement
    // perform a map find and update the slice
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
            if (verboseMode)
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

    // Component of function FindOtherPaths
    void ComputeOuterPaths(std::set<std::pair<int,int>>& otherPaths, const std::vector<int>& sLines) {
        std::set<std::pair<int,int>> ifGroup;

        // Find all valid connections between if,else-if,and else blocks
        for (const auto& ifblock : ifdata) {
            bool isSingleIf = true;
            
            // ensure we dont try indexing non-existing items
            for (const auto& elseifblock : elseifdata) {
                if (ifblock.second == elseifblock.first) {
                    ifGroup.insert(std::make_pair(ifblock.first, elseifblock.first));
                    isSingleIf = false;
                }
            }

            // ensure we dont try indexing non-existing items
            for (const auto& elseblock : elsedata) {
                if (ifblock.second == elseblock.first) {
                    ifGroup.insert(std::make_pair(ifblock.first, elseblock.first));
                    isSingleIf = false;
                }
            }

            // occurs when we have a single if statement that does not connect
            // to any else-if or else statements
            if (isSingleIf)
                ifGroup.insert(std::make_pair(ifblock.first, ifblock.second));
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
                        // std::cerr << "Potential Outter-Path --> " << sLines[i] << "," << sLines[k] << std::endl;
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
    }

    // Component of function FindOtherPaths
    void ComputeExitPaths(std::set<std::pair<int,int>>& otherPaths, const std::vector<int>& sLines, const std::set<int>& ignoreLines) {
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
                        // std::cerr << sLines[i] << " contained in -> (" << ifblock.first << "," << ifblock.second << ")" << std::endl;

                        // focus on the block with the smallest gap
                        int containedBlockSize = containedBlock.second - containedBlock.first;
                        int blockSize = ifblock.second - ifblock.first;
                        // update the block we suspect the current sline is contained in if
                        // either we have not initially updated the block, or we found a block
                        // of smaller size containing sLines[i]
                        if (containedBlock.first == 0 || (blockSize < containedBlockSize)) {
                            containedBlock = ifblock;

                            //std::cerr << "[*] Focusing on block -> (" <<
                            //containedBlock.first << "," << containedBlock.second <<
                            //") => " << sLines[i] << std::endl;
                        }
                    }
                }

                // if we have marked a ifdata block attempt to form a connection
                if (containedBlock.first != 0) {
                    // std::cerr << "[*] Block of Interest -> (" <<
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
                            // std::cerr << "Potential Exit-Path --> " << sLines[i] << "," << sLines[k] << std::endl;
                            otherPaths.insert(std::make_pair(sLines[i], sLines[k]));
                            break;
                        }
                    }
                }
            }
        }
    }

    // Attempt to find other Forward Control-Flow paths | ComputeControlPaths Helper Function
    std::set<std::pair<int,int>> FindOtherPaths(const std::vector<int>& sLines, const std::set<int>& ignoreLines) {
        std::set<std::pair<int,int>> otherPaths;

        // For each path we need to compute, there is a specific function for it
        ComputeExitPaths(otherPaths, sLines, ignoreLines);
        ComputeOuterPaths(otherPaths, sLines);

        return otherPaths;
    }

    // srcSlice focuses on Forward-Slicing, therefor our Control-Flows are going to be forward-flowing
    // we are not focusing on backwards-flows.
    void ComputeControlPaths() {
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
                            // std::cerr << "[*] " << __LINE__ << " | Normal Path --> " << sLines[i] << "," << sLines[i+1] << std::endl;
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
                                // std::cerr << "[*] " << __LINE__ << " | Normal Path --> " << sLines[i] << "," << sLines[i+1] << std::endl;
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
    }

    auto ArgumentProfile(std::pair<std::string, std::shared_ptr<FunctionData>> func, int paramIndex, std::unordered_set<std::string>& visit_func) {
        auto Spi = profileMap.find(func.second->parameters.at(paramIndex)->name->ToString());

        for (auto& param : func.second->parameters) {
            if (param->name) {
                if (profileMap.find(param->name->ToString())->second.back().visited) {
                    return Spi;
                } else {
                    if (profileMap.find(param->name->ToString())->second.back().cfunctions.size() > 0) {
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
                                            if (func->parameters[std::atoi(cfunc.second.first.c_str()) - 1]->name != nullptr) {
                                                // Only run this section if the parameter name can be extracted
                                                auto recursiveSpi = ArgumentProfile(std::make_pair(cfunc.first, func), std::atoi(cfunc.second.first.c_str()) - 1, visit_func);
                                                if (profileMap.find(param->name->ToString()) != profileMap.end() &&
                                                    profileMap.find(recursiveSpi->first) != profileMap.end()) {
                                                    // Uses and Defs need to reflect based on whether its pass by reference or pass by value
                                                    if (!recursiveSpi->second.back().isReference && !recursiveSpi->second.back().isPointer) {
                                                        // pass by value
                                                        profileMap.find(param->name->ToString())->second.back().uses.insert(
                                                            recursiveSpi->second.back().definitions.begin(),
                                                            recursiveSpi->second.back().definitions.end()
                                                        );
                                                        profileMap.find(param->name->ToString())->second.back().uses.insert(
                                                                recursiveSpi->second.back().uses.begin(),
                                                                recursiveSpi->second.back().uses.end()
                                                        );
                                                    } else {
                                                        // pass by reference
                                                        profileMap.find(param->name->ToString())->second.back().definitions.insert(
                                                            recursiveSpi->second.back().definitions.begin(),
                                                            recursiveSpi->second.back().definitions.end()
                                                        );
                                                        profileMap.find(param->name->ToString())->second.back().uses.insert(
                                                                recursiveSpi->second.back().uses.begin(),
                                                                recursiveSpi->second.back().uses.end()
                                                        );
                                                    }

                                                    profileMap.find(param->name->ToString())->second.back().cfunctions.insert(
                                                            recursiveSpi->second.back().cfunctions.begin(),
                                                            recursiveSpi->second.back().cfunctions.end()
                                                    );
                                                    // ensure dependencies and aliases are within local scope
                                                    if (recursiveSpi->second.back().function == profileMap.find(param->name->ToString())->second.back().function) {
                                                        profileMap.find(param->name->ToString())->second.back().aliases.insert(
                                                                recursiveSpi->second.back().aliases.begin(),
                                                                recursiveSpi->second.back().aliases.end()
                                                        );
                                                        profileMap.find(param->name->ToString())->second.back().dvars.insert(
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
                        profileMap.find(param->name->ToString())->second.back().visited = true;
                    }
                }
            }
        }

        return Spi;
    }

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;

	    for (auto& var : profileMap) {
            // expand list of potential targets (aliases)
            for (auto& sp : var.second) {
                for (auto& alias : sp.aliases) {
                    // Find the slice profile of the alias marked
                    auto spi = profileMap.find(alias.first);
                    if (spi != profileMap.end()) {
                        // fingerprint the profile based on contained use
                        for (auto aspi : spi->second) {
                            auto usesItr = std::find(aspi.uses.begin(), aspi.uses.end(), alias.second);
                            if (usesItr != aspi.uses.end()) {
                                // determine if the potential target is a pointer or reference
                                if (aspi.isPointer || aspi.isReference) {
                                    // push_back alias slice profile's aliases into the source slice aliases
                                    sp.aliases.insert(aspi.aliases.begin(), aspi.aliases.end());
                                }
                            }
                        }
                    }
                }
            }

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
                                if (func->parameters[std::atoi(cfunc.second.first.c_str()) - 1]->name != nullptr) {
                                    // Only run this section if the parameter name can be extracted
                                    auto Spi = ArgumentProfile(std::make_pair(cfunc.first, func), std::atoi(cfunc.second.first.c_str()) - 1, visited_func);
                                    auto sliceItr = Spi->second.begin();
                                    std::string desiredVariableName = sliceItr->variableName;

                                    for (sliceItr = Spi->second.begin(); sliceItr != Spi->second.end(); ++sliceItr) {
                                        if (sliceItr->containsDeclaration) {
                                            if (sliceItr->variableName != desiredVariableName) {
                                                continue;
                                            }
                                            if (GetSimpleFunctionName(sliceItr->function) != cfunc.first) {
                                                continue;
                                            }
                                            std::string parameterDeclLine = std::to_string(func->parameters[std::stoi(cfunc.second.first) - 1]->lineNumber);
                                            if (std::to_string(sliceItr->lineNumber) != parameterDeclLine) {
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
                                        } else {
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
                                                sliceItr->uses.end()
                                        );

                                        // ensure dependencies and aliases are within local-scope
                                        if (profileMap.find(var.first)->second.back().function == sliceItr->function) {
                                            profileMap.find(var.first)->second.back().aliases.insert(
                                                    sliceItr->aliases.begin(),
                                                    sliceItr->aliases.end()
                                            );
                                            profileMap.find(var.first)->second.back().dvars.insert(
                                                    sliceItr->dvars.begin(),
                                                    sliceItr->dvars.end()
                                            );
                                        }
                                    } else {
                                        if (verboseMode) {
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

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> forloopdata;
    std::vector<std::pair<int, int>> whileloopdata;
    std::vector<std::pair<int, int>> dowhileloopdata;

    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elseifdata;
    std::vector<std::pair<int, int>> elsedata;

    FunctionSignatureData funcSigCollection;
    bool verboseMode, calculateControlEdges;
};


#endif