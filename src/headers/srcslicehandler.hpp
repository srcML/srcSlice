#ifndef SRCSLICEHANDLER
#define SRCSLICEHANDLER

#include <srcsliceprofile.hpp>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <CLI11.hpp>

#include <srcDispatcher.hpp>
#include <srcSAXHandler.hpp>

#include <IfStmtPolicy.hpp>
#include <SwitchPolicy.hpp>
#include <WhilePolicy.hpp>
#include <ForPolicy.hpp>
#include <DoPolicy.hpp>
#include <TryPolicy.hpp>
#include <FunctionPolicy.hpp>
#include <ClassPolicy.hpp>
#include <UnitPolicy.hpp>

class SrcSliceHandler : public srcDispatch::PolicyListener {
public:
    ~SrcSliceHandler(){};

    // Use literal string filename ctor of srcSAXController (srcslice cpp main)
    SrcSliceHandler(const char* filename, bool v, bool ce) : verboseMode(v), calculateControlEdges(ce) {
        srcSAXController control(filename);
        srcDispatch::srcDispatcher<srcDispatch::UnitPolicy> handler(this);
        control.parse(&handler); // Start parsing
        GenerateSlices();
    }

    // Use string srcml buffer ctor of srcSAXController
    SrcSliceHandler(const std::string& sourceCodeStr, bool ce) : verboseMode(false), calculateControlEdges(ce) {
        srcSAXController control(sourceCodeStr);
        srcDispatch::srcDispatcher<srcDispatch::UnitPolicy> handler(this);
        control.parse(&handler); // Start parsing
        GenerateSlices();
    }

    std::vector<std::shared_ptr<srcDispatch::ClassData>> GetClassInfo(std::shared_ptr<srcDispatch::UnitData>& unit) {
        std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ClassData>>>* deltaClasses = &(unit->classInfo);

        std::vector<std::shared_ptr<srcDispatch::ClassData>> classes;
        for (auto& deltaClass : *deltaClasses) {
            // extract the decldata shared_ptr
            if (deltaClass.GetElement()) {
                classes.push_back(deltaClass.GetElement());
            }
        }
        return classes;
    }

    std::vector<std::shared_ptr<srcDispatch::FunctionData>> GetFunctionInfo(std::shared_ptr<srcDispatch::UnitData>& unit) {
        std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>>* deltaFunctions = &(unit->functionInfo);

        std::vector<std::shared_ptr<srcDispatch::FunctionData>> functions;
        for (auto& deltaFunc : *deltaFunctions) {
            // extract the decldata shared_ptr
            if (deltaFunc.GetElement()) {
                functions.push_back(deltaFunc.GetElement());
            }
        }
        return functions;
    }

    std::vector<std::shared_ptr<srcDispatch::DeclData>> GetDeclInfo(std::shared_ptr<srcDispatch::UnitData>& unit) {
        // make a reference to potential large vector
        std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclStmtData>>>* deltaDeclStmts = &(unit->declStmtInfo);

        std::vector<std::shared_ptr<srcDispatch::DeclData>> decls;
        for (auto& deltaDeclStmt : *deltaDeclStmts) {
            // extract the decldata shared_ptr
            // and only push it if the element exists
            if (deltaDeclStmt.GetElement()) {
                for (auto& deltaDecl : deltaDeclStmt->decls) {
                    if (deltaDecl.GetElement())
                        decls.push_back(deltaDecl.GetElement());
                }
            }
        }
        return decls;
    }

    void ProcessUnits() {
        for (auto& unitPair : units) {
            // Process Global Decls | policy->Data<std::vector<std::shared_ptr<srcDispatch::DeclData>>>()
            ProcessDeclStmts(nullptr, nullptr, nullptr, "",
                std::make_shared<std::vector<std::shared_ptr<srcDispatch::DeclData>>>(GetDeclInfo(unitPair.second)), unitPair.first);
            
            for (auto& classData : GetClassInfo(unitPair.second)) {
                ProcessClassData(classData, unitPair.first);
            }
            for (auto& functionData : GetFunctionInfo(unitPair.second)) {
                ProcessFunctionData(functionData, "", functionData->namespaces, unitPair.first);
            }
        }
    }

    void Notify(const srcDispatch::PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) override {
        if (verboseMode) {
            std::cerr << "[*] " << __LINE__ << " | " << __FUNCTION__ << " : SRCSLICEHANDLER" << std::endl;
        }

        std::shared_ptr<srcDispatch::UnitData> unit = policy->Data<srcDispatch::UnitData>();
        if (unit) {
            if (verboseMode) {
                std::cerr << "[*] Unit Collected" << std::endl;
            }
            units.push_back(std::make_pair(SliceCtx(ctx), unit));
        }
    }

    void NotifyWrite(const srcDispatch::PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}

    void ProcessFunctionData(std::shared_ptr<srcDispatch::FunctionData> function_data, std::string className,
                            std::vector<std::string>& containingNamespaces, const SliceCtx &ctx) {
        if (verboseMode) {
            std::cerr << "[*] " << __LINE__  << " Processing Function Name: " << function_data->name.ToString() << std::endl;
        }
        ProcessFunctionSignature(function_data, className, containingNamespaces, ctx);
        ProcessDeclStmts(function_data, function_data->block.GetElement(), nullptr, className, nullptr, ctx);
        ProcessInitLists(function_data, className, ctx);
        ProcessExprStmts(function_data, function_data->block.GetElement(), className, ctx);
    }

    void ProcessClassData(std::shared_ptr<srcDispatch::ClassData> class_data, const SliceCtx &ctx) {
        if (class_data) {
            std::string className = "";

            if (class_data->name) {
                className = class_data->name.ToString();
            }

            // Process Class Member Variables
            ProcessDeclStmts(nullptr, nullptr, class_data, className, nullptr, ctx);

            // Process Class Contructors
            for (auto& deltaFuncElem : class_data->constructors) {
                ProcessFunctionData(deltaFuncElem.GetElement(), className, class_data->namespaces, ctx);
            }
            // Process Class Dtor
            if (class_data->destructor && class_data->destructor.GetElement()) {
                ProcessFunctionData(class_data->destructor.GetElement(), className, class_data->namespaces, ctx);
            }

            // Process Class Methods (Member Functions)
            for (auto& deltaFuncElem : class_data->methods) {
                ProcessFunctionData(deltaFuncElem.GetElement(), className, class_data->namespaces, ctx);
            }

            // Process Operator Overloading
            for (auto& deltaFuncElem : class_data->operators) {
                ProcessFunctionData(deltaFuncElem.GetElement(), className, class_data->namespaces, ctx);
            }

            // Process Nested Classes
            for (auto& deltaClassElem : class_data->innerClasses) {
                if (deltaClassElem.GetElement()) {
                    ProcessClassData(deltaClassElem.GetElement(), ctx);
                }
            }
        }
    }

    void ProcessDeclStmts(std::shared_ptr<srcDispatch::FunctionData> funcData, std::shared_ptr<srcDispatch::BlockData> block, const std::shared_ptr<srcDispatch::ClassData> classData,
                            std::string className, std::shared_ptr<std::vector<std::shared_ptr<srcDispatch::DeclData>>> potentialGlobals,
                            const SliceCtx &ctx) {
        std::vector<std::shared_ptr<srcDispatch::DeclData>> localGroup;
        std::vector<std::string> containingNamespaces;

        if (funcData) {
            // Get containing namespaces from functionData
            containingNamespaces = funcData->namespaces;

            if (block) {
                for (auto& stmt : block->statements) {
                    if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclStmtData>)) {
                        // locals
                        std::shared_ptr<srcDispatch::DeclStmtData> declStmtData = std::any_cast<std::shared_ptr<srcDispatch::DeclStmtData>>(stmt.GetElement());
                        if (declStmtData) {
                            for (auto& deltaDecl : declStmtData->decls) {
                                if (deltaDecl.GetElement()) {
                                    localGroup.push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(deltaDecl.GetElement()));
                                }
                            }
                        }
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
                        std::shared_ptr<srcDispatch::IfStmtData> ifStmtData = std::any_cast<std::shared_ptr<srcDispatch::IfStmtData>>(stmt.GetElement());
                        if (ifStmtData) {
                            for (const auto& clause : ifStmtData->clauses) {
                                if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfData>)) {
                                    std::shared_ptr<srcDispatch::IfData> ifData = std::any_cast<std::shared_ptr<srcDispatch::IfData>>(clause.GetElement());
                                    for (auto& elem : ifData->condition->conditions) {
                                        if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                            std::shared_ptr<srcDispatch::DeclData> initData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());
                                            if (initData) {
                                                localGroup.push_back(initData);
                                            }
                                        }
                                    }
                                } else if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ElseIfData>)) {
                                    std::shared_ptr<srcDispatch::ElseIfData> elseIfData = std::any_cast<std::shared_ptr<srcDispatch::ElseIfData>>(clause.GetElement());
                                    for (auto& elem : elseIfData->condition->conditions) {
                                        if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                            std::shared_ptr<srcDispatch::DeclData> initData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());
                                            if (initData) {
                                                localGroup.push_back(initData);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
                        std::shared_ptr<srcDispatch::ForData> forData = std::any_cast<std::shared_ptr<srcDispatch::ForData>>(stmt.GetElement());
                        for (auto& initData : forData->control->init->inits) {
                            if (initData.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                std::shared_ptr<srcDispatch::DeclData> initDeclData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(initData.GetElement());
                                if (initDeclData) {
                                    localGroup.push_back(initDeclData);
                                }
                            }
                        }
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
                        std::shared_ptr<srcDispatch::SwitchData> switchData = std::any_cast<std::shared_ptr<srcDispatch::SwitchData>>(stmt.GetElement());
                        for (const auto& elem : switchData->condition->conditions) {
                            if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                std::shared_ptr<srcDispatch::DeclData> initData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());
                                if (initData) {
                                    localGroup.push_back(initData);
                                }
                            }
                        }
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::TryData>)) {
                        // For Java we have try-with-resources, make slice profiles of the resources
                        std::shared_ptr<srcDispatch::TryData> tryData = std::any_cast<std::shared_ptr<srcDispatch::TryData>>(stmt.GetElement());
                    
                        // catches have parameter lists with decldata
                        for (auto& clause : tryData->clauses) {
                            if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CatchData>)) {
                                std::shared_ptr<srcDispatch::CatchData> catchData = std::any_cast<std::shared_ptr<srcDispatch::CatchData>>(clause.GetElement());
                                for (auto& parameter : catchData->parameters) {
                                    if (parameter.GetElement()) {
                                        localGroup.push_back(parameter.GetElement());
                                    }
                                }
                            }
                        }
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ThrowData>)) {
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::GotoData>)) {
                    } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::LabelData>)) {
                    } else {
                        if (verboseMode) {
                            std::cerr << "[-] " << __LINE__ << " : " << __FUNCTION__ << " | Unhandled Type -> " << stmt.GetElement().type().name() << std::endl;
                        }
                    }
                }
            }
        }

        /*
            enum AccessSpecifier {
            NONE      = 0,
            PUBLIC    = 1,
            PRIVATE   = 2,
            PROTECTED = 3 
        };
        */

        // Look at all declared members from each class field
        if (classData) {
            if (classData->name)
                className = classData->name.ToString();

            // Get containing namespaces from classData
            containingNamespaces = classData->namespaces;

            // Review decls of class fields
            for (auto& deltaClassFields : classData->fields) {
                for (auto& deltaDecl : deltaClassFields->decls) {
                    if (deltaDecl.GetElement()) {
                        localGroup.push_back(deltaDecl.GetElement());
                    }
                }
            }
        }

        // Look at potential globals that are captured
        if (potentialGlobals) {
            localGroup.insert(localGroup.end(), potentialGlobals->begin(), potentialGlobals->end());

            // capture containing namespaces for potential globals
            containingNamespaces = ctx.containingNamespaces;
        }

        // loop through all the expression statements within Decl Statements
        for (const auto localVar : localGroup) {
            if (!localVar) continue;
            if (!localVar->name) continue;

            std::vector<std::shared_ptr<VariableData>> varDataGroup;

            if (localVar->init) {
                varDataGroup = ParseExpr(*localVar->init.GetElement(), localVar->init.GetElement()->lineNumber);
            }

            // Collect pieces about the newly declared variable to use later when adding it into
            // our profileMap
            std::string declVarName = localVar->name.ToString();
            declVarName = declVarName.substr(0, declVarName.find('[')); // remove index operator characters from variable names
            std::string declVarType = "";

            bool isPointer = false;
            bool isReference = false;
            bool isArray = false;

            // Make a function to pull data-type
            declVarType = localVar->type.ToString();

            // Extract just the Data-Type name without extra data
            /*for (std::size_t pos = 0; pos < localVar->type->types.size(); ++pos) {
                const auto type = localVar->type->types[pos];

                if (type.second == Dispatch::TypeData::POINTER) {
                    isPointer = true;
                    declVarType += "*";
                } else if (type.second == Dispatch::TypeData::REFERENCE) {
                    isReference = true;
                    declVarType += "&";
                } else if (type.second == Dispatch::TypeData::TYPENAME) {
                    declVarType = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(type.first)->SimpleName();

                    // remove `std ` in `std string` if neccessary
                    declVarType = declVarType.substr(declVarType.find(' ')+1);

                    // attempt to mark raw-arrays for alias processing later
                    if (localVar->name && localVar->name->indices.size() > 0) {
                        isArray = true;
                        declVarType += "[]";
                    }
                }
            }*/

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
                    sliceProfile.isPotentialArray = isArray;

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
                sliceProf.isPotentialArray = isArray;

                // point the iterator to the newly inserted profile element
                sliceProfileItr = profileMap.insert(std::make_pair(declVarName, std::vector<SliceProfile>{ std::move(sliceProf) })).first;
            }

            sliceProfileItr->second.back().isReference = isReference;
            sliceProfileItr->second.back().isPointer = isPointer;
            sliceProfileItr->second.back().isPotentialArray = isArray;

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
                if (funcData->name) {
                    sliceProfileItr->second.back().function = funcData->name.ToString();
                }
            }

            // Link the class this slice is located in
            if (classData) {
                if (classData->name) {
                    sliceProfileItr->second.back().nameOfContainingClass = classData->name.ToString();
                }
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
                                        if (!sliceProfileItr->second.back().isPotentialArray) {
                                            sliceProfileItr->second.back().currentPointerReference = varData->GetNameOfIdentifier();
                                        }
                                    }
                                } else if (sliceProfileItr->second.back().isReference) {
                                    sliceProfileItr->second.back().aliases.insert(std::make_pair(varData->GetNameOfIdentifier(), varData->originLine));
                                    if (!sliceProfileItr->second.back().isPotentialArray) {
                                        sliceProfileItr->second.back().currentPointerReference = varData->GetNameOfIdentifier();
                                    }
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

    void ProcessInitLists(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::string className, const SliceCtx &ctx) {
        if (!funcData) return;

        // process C++ initializer lists
        for (auto& deltaCallData : funcData->memberInitList) {
            std::vector<std::shared_ptr<srcDispatch::ExpressionData>> exprStmts;

            // Insert def line for the valid slice used in the initList
            std::string varName = deltaCallData->name.ToString();
            VariableData data(varName);
            data.definitions.insert(deltaCallData->name->lineNumber);

            // Find the slice for the "call-target"
            auto spi = profileMap.find(varName);
            SliceProfile* sp = nullptr;

            if (spi != profileMap.end()) {
                sp = &(spi->second.back());
            }

            if (sp != nullptr) {
                sp->definitions.insert(data.definitions.begin(), data.definitions.end());
            }

            // extract and parse expressions within init list call
            for (auto& deltaArg : deltaCallData->arguments) {
                if (deltaArg.GetElement()) {
                    exprStmts.push_back(deltaArg.GetElement());

                    // there is data-dependency between the call-target and the args
                    if (sp != nullptr) {
                        for (auto& deltaExprElem : deltaArg->expr) {
                            if (deltaExprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                                std::shared_ptr<srcDispatch::NameData> nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(deltaExprElem.GetElement());
                                if (nameData->SimpleName().empty()) continue;

                                std::string varName = ExtractName(nameData->SimpleName());
                                sp->dvars.insert(std::make_pair(varName, deltaArg->lineNumber));
                            }
                        }
                    }
                }
            }

            for (auto& expr : exprStmts) {
                UpdateSlices(ParseExpr(*expr, expr->lineNumber), funcData, className, ctx);
            }
        }
    }

    void ProcessExprStmts(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::shared_ptr<srcDispatch::BlockData> block,
                            std::string className, const SliceCtx &ctx) {
        std::vector<std::shared_ptr<srcDispatch::ExpressionData>> exprStmts;
        std::vector<std::any> conditionals;
        std::vector<std::shared_ptr<srcDispatch::TryData>> tryBlocks;

        // Capture Conditional expressions
        if (block) {
            for (auto& stmt : block->statements) {
                if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExprStmtData>)) {
                    auto exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExprStmtData>>(stmt.GetElement());
                    // Capture general expressions
                    exprStmts.push_back(exprstmt->expr.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ReturnData>)) {
                    auto retstmt = std::any_cast<std::shared_ptr<srcDispatch::ReturnData>>(stmt.GetElement());
                    // Capture general Return expressions
                    exprStmts.push_back(retstmt->expr.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CaseData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::WhileData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DoData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::TryData>)) {
                    auto tryData = std::any_cast<std::shared_ptr<srcDispatch::TryData>>(stmt.GetElement());
                    tryBlocks.push_back(tryData);
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ThrowData>)) {
                    auto throwData = std::any_cast<std::shared_ptr<srcDispatch::ThrowData>>(stmt.GetElement());
                    exprStmts.push_back(throwData->expr.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::GotoData>)) {
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::LabelData>)) {
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclStmtData>)) {
                    // ignore this when handling expressions
                    continue;
                } else {
                    if (verboseMode) {
                        std::cerr << "[-] " << __LINE__ << " : " << __FUNCTION__ << " | Unhandled Type -> " << stmt.GetElement().type().name() << std::endl;
                    }
                }
            }

            if (conditionals.size() > 0) {
                CollectConditionalData(&exprStmts, nullptr, conditionals);
            }

            if (tryBlocks.size() > 0) {
                CollectTryBlockData(funcData, tryBlocks, className, ctx);
            }
        }

        // loop through all the expression statements
        for (auto itr = exprStmts.begin(); itr != exprStmts.end(); ++itr) {
            std::shared_ptr<srcDispatch::ExpressionData> expr = *itr;
            if (expr) {
                // Check if any NameData within an Express contains index operator expressions
                // and insert those into the exprStmts vector
                for (const auto& data : expr->expr) {
                    if (data.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                        std::shared_ptr<srcDispatch::NameData> nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(data.GetElement());
                        if (nameData && nameData->indices.size() > 0) {
                            // strip DeltaElement layer
                            std::vector<std::shared_ptr<srcDispatch::ExpressionData>> indiceExprs;
                            for (auto& deltaExpr : nameData->indices) {
                                if (deltaExpr.GetElement()) {
                                    indiceExprs.push_back(deltaExpr.GetElement());
                                }
                            }

                            itr = exprStmts.insert(itr+1, indiceExprs.begin(), indiceExprs.end()); // set to iterator of newly inserted data
                            --itr;
                        }
                    }
                }

                UpdateSlices(ParseExpr(*expr, expr->lineNumber), funcData, className, ctx);
            }
        }
    }

    void UpdateSlices(std::vector<std::shared_ptr<VariableData>> varDataGroup, srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData,
                        std::string className, const SliceCtx &ctx) {
        std::vector<std::string> containingNamespaces;
        if (funcData) containingNamespaces = funcData->namespaces;

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
                                    if (!sliceProfileLHSItr->isPotentialArray) {
                                        sliceProfileLHSItr->currentPointerReference = rhsName;
                                    }
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
                                            if (!sliceProfileLHSItr->isPotentialArray) {
                                                sliceProfileLHSItr->currentPointerReference = rhsName;
                                            }
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

    // Use collected function call data to push a new cfunctions entry into a referenced slice profile
    void CreateSliceCallData(std::string functionName, int argIndex, int functionDefLine, SliceProfile& sliceProfile, unsigned int functionCallLine) {
        FunctionCallData sliceCallData = FunctionCallData(
            functionName, // function call name
            argIndex, // arg index starting from 1 to n
            functionDefLine, // function definition line number
            functionCallLine // line where the function call occurs
        );

        if (sliceProfile.aliases.size() > 0) {
            // if this profile contains aliases we need to find the profile of the pointers
            // current reference to append this cfunc data
            auto aliasReferenceProfile = profileMap.find(sliceProfile.currentPointerReference);
            if (aliasReferenceProfile != profileMap.end()) {
                // Alias targets will inherit SOME of the cfunctions from their Alias representative
                aliasReferenceProfile->second.back().cfunctions.insert(sliceCallData);
            }
        }

        sliceProfile.cfunctions.insert(sliceCallData);
    }

    void ProcessFunctionCall(std::shared_ptr<srcDispatch::CallData> funcCallData) {
        if (!funcCallData) return;

        std::string functionName = funcCallData->name.ToString();

        // std::cerr << "[*] CALL-DATA --> " << *funcCallData << std::endl;
        // std::cerr << " |____ ARGC --> " << funcCallData->arguments.size() << std::endl;

        int argIndex = 0;
        for (auto& arg : funcCallData->arguments) {
            ++argIndex;

            // Extract the Variable Name from the expression contained within
            // the function call argument list index
            for (auto& exprElem : arg->expr) {    
                // std::cerr << "      |____ CALL-DATA ELEM --> " << exprElem.type().name() << std::endl;
                if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                    std::shared_ptr<srcDispatch::NameData> name = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(exprElem.GetElement());
                    unsigned int argUseLineNumber = funcCallData->lineNumber;

                    // Don't worry about exprElems with bad name ptrs
                    if (!name) continue;

                    // Update an existing slices Call data
                    auto sliceProfileItr = profileMap.find(name->SimpleName());
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
                            // if there is only one record of a function signature
                            if (funcSig->second.size() == 1) {
                                unsigned int funcLineDef = funcSig->second[0]->lineNumber;
                                CreateSliceCallData(simpleFunctionName, argIndex, funcLineDef, sliceProfileItr->second.back(), funcCallData->lineNumber);
                            } else {
                                // if a function is overloaded
                                size_t pos = 0;

                                // If we have a signature with predefined parameters and another signature where the data-type
                                // of the parameter differs, we need to check argc <= paramc AND dataType(arg[i]) == dataType(param[i])
                                // we will have to derive the arg[i] to its corresponding slice, param[i] is a decldata so we can fetch its type
                                for (pos; pos < funcSig->second.size(); ++pos) {
                                    bool argumentInBounds = (argIndex-1 < funcSig->second[pos]->parameters.size());
                                    if (!argumentInBounds) continue;

                                    bool validArgCount = (funcCallData->arguments.size() <= funcSig->second[pos]->parameters.size());
                                    if (!validArgCount) continue;

                                    std::string sliceDataType = sliceProfileItr->second.back().variableType;

                                    // Make a function to pull data-type
                                    std::string paramDataType = funcSig->second[pos]->parameters[argIndex-1]->type.ToString();

                                    std::string filteredSliceDataType = "";
                                    std::string filteredParamDataType = "";
                                    
                                    // For data-types like 'int *' or 'int &' only track everything before the space character from the type for comparison
                                    filteredSliceDataType = sliceDataType.substr(0, sliceDataType.find(' '));
                                    filteredParamDataType = paramDataType.substr(0, paramDataType.find(' '));

                                    // For data-types like 'int*' only track everything before the astrisks character from the type for comparison
                                    filteredSliceDataType = filteredSliceDataType.substr(0, filteredSliceDataType.find('*'));
                                    filteredParamDataType = filteredParamDataType.substr(0, filteredParamDataType.find('*'));
                                    // For data-types like 'int&' only track everything before the amp character from the type for comparison
                                    filteredSliceDataType = filteredSliceDataType.substr(0, filteredSliceDataType.find('&'));
                                    filteredParamDataType = filteredParamDataType.substr(0, filteredParamDataType.find('&'));

                                    bool matchingTypes = (filteredParamDataType == filteredSliceDataType);
                                    if (verboseMode) {
                                        std::cerr << "[-] " << __LINE__  << " | Parameter Filtered-Type -> " << filteredParamDataType << " | Argument Filtered-Type -> " << filteredSliceDataType << std::endl;
                                    }
                                    if (!matchingTypes) continue;

                                    // potentially valid function finger-print
                                    break;
                                }
    
                                if (pos < funcSig->second.size()) {
                                    unsigned int funcLineDef = funcSig->second[pos]->lineNumber;
                                    CreateSliceCallData(simpleFunctionName, argIndex, funcLineDef, sliceProfileItr->second.back(), funcCallData->lineNumber);
                                } else {
                                    if (verboseMode)
                                        std::cerr << "[-] " << __LINE__  << " | Fingerprint Not Found for -> " << simpleFunctionName << std::endl;
                                    CreateSliceCallData(simpleFunctionName, argIndex, 0, sliceProfileItr->second.back(), funcCallData->lineNumber);
                                }
                            }
                        } else {
                            if (verboseMode)
                                std::cerr << "[-] " << __LINE__  << " | No Function Signature Found for -> " << simpleFunctionName << std::endl;
                            CreateSliceCallData(simpleFunctionName, argIndex, 0, sliceProfileItr->second.back(), funcCallData->lineNumber);
                        }
                    }
                } else if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CallData>)) {
                    // std::cerr << "ARGUMENT IS A CALL-DATA" << std::endl;
                    std::shared_ptr<srcDispatch::CallData> nestedCallData = std::any_cast<std::shared_ptr<srcDispatch::CallData>>(exprElem.GetElement());
                    ProcessFunctionCall(nestedCallData);
                }
            }
        }
    }

    // try blocks contain both exprs and decls, need to extract those decls and create slice profiles
    // for them, along with capturing expressions to update collected slices
    void CollectTryBlockData(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::vector<std::shared_ptr<srcDispatch::TryData>>& tryBlocks,
                                std::string className, const SliceCtx &ctx) {
        for (auto& tryBlock : tryBlocks) {
            if (tryBlock->block) {
                // Collect Decls and Exprs within the block of this Try-Block
                ProcessDeclStmts(funcData.GetElement(), tryBlock->block.GetElement(), nullptr, className, nullptr, ctx);
                ProcessExprStmts(funcData, tryBlock->block.GetElement(), className, ctx);
            }

            // Collect Decls and Exprs within the catch block
            for (auto& clause : tryBlock->clauses) {
                if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CatchData>)) {
                    std::shared_ptr<srcDispatch::CatchData> catchData = std::any_cast<std::shared_ptr<srcDispatch::CatchData>>(clause.GetElement());
                    if (catchData->block) {
                        ProcessDeclStmts(funcData.GetElement(), catchData->block.GetElement(), nullptr, className, nullptr, ctx);
                        ProcessExprStmts(funcData, catchData->block.GetElement(), className, ctx);
                    }
                }
            }
        }
    }

    void CollectConditionalData(std::vector<std::shared_ptr<srcDispatch::ExpressionData>>* exprStmts, std::vector<std::shared_ptr<srcDispatch::DeclData>>* declStmts,
                                std::vector<std::any>& conditionals) {
        std::vector<std::shared_ptr<srcDispatch::BlockData>> cntlBlocks;

        // identify what conditional type we are viewing and handle logic accordingly
        for (const auto& cntl : conditionals) {
            if (cntl.type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
                // Extract all of the block data from if statements
                std::shared_ptr<srcDispatch::IfStmtData> ifcntl = std::any_cast<std::shared_ptr<srcDispatch::IfStmtData>>(cntl);

                // ifstmts have three potential clauses (if-elseif-else)
                for (const auto& clause : ifcntl->clauses) {
                    if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfData>)) {
                        std::shared_ptr<srcDispatch::IfData> ifData = std::any_cast<std::shared_ptr<srcDispatch::IfData>>(clause.GetElement());

                        for (auto& elem : ifData->condition->conditions) {
                            if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                if (declStmts) {
                                    declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement()));
                                }
                            } else if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                                if (exprStmts) {
                                    exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement()));
                                }
                            }
                        }

                        // minimize duplicate entries
                        if (ifdata.size() == 0 || ifdata.back() != std::make_pair((int)(ifData->startLineNumber), (int)(ifData->endLineNumber)))
                            ifdata.push_back(std::make_pair(ifData->startLineNumber, ifData->endLineNumber));

                        cntlBlocks.push_back(ifData->block.GetElement());
                    } else if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ElseIfData>)) {
                        std::shared_ptr<srcDispatch::ElseIfData> elseIfData = std::any_cast<std::shared_ptr<srcDispatch::ElseIfData>>(clause.GetElement());

                        for (auto& elem : elseIfData->condition->conditions) {
                            if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                if (declStmts) {
                                    declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement()));
                                }
                            } else if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                                if (exprStmts) {
                                    exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement()));
                                }
                            }
                        }

                        // minimize duplicate entries, pushed into ifdata for the ComputeControlPaths Algorithm
                        if (ifdata.size() == 0 || ifdata.back() != std::make_pair((int)(elseIfData->startLineNumber), (int)(elseIfData->endLineNumber)))
                            ifdata.push_back(std::make_pair(elseIfData->startLineNumber, elseIfData->endLineNumber));
                        
                        // track elseif block data for later usage
                        if (elseifdata.size() == 0 || elseifdata.back() != std::make_pair((int)(elseIfData->startLineNumber), (int)(elseIfData->endLineNumber)))
                            elseifdata.push_back(std::make_pair(elseIfData->startLineNumber, elseIfData->endLineNumber));

                        cntlBlocks.push_back(elseIfData->block.GetElement());
                    } else if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ElseData>)) {
                        std::shared_ptr<srcDispatch::ElseData> elseData = std::any_cast<std::shared_ptr<srcDispatch::ElseData>>(clause.GetElement());

                        // minimize duplicate entries
                        if (elsedata.size() == 0 || elsedata.back() != std::make_pair((int)(elseData->startLineNumber), (int)(elseData->endLineNumber)))
                            elsedata.push_back(std::make_pair(elseData->startLineNumber, elseData->endLineNumber));
                        cntlBlocks.push_back(elseData->block.GetElement());
                    }
                }
            } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
                // Extract all of the block data from Switch statements
                std::shared_ptr<srcDispatch::SwitchData> switchData = std::any_cast<std::shared_ptr<srcDispatch::SwitchData>>(cntl);

                /*
                    Ensure we are getting the uses from the case lines
                    Ensure we capture data from the case blocks as well
                */

                // Connect the use lines of switch cases to their corresponding control variables
                std::vector<std::shared_ptr<srcDispatch::NameData>> controlVariables;
                // std::cerr << "[*] Switch Conditions Size: " << switchData->condition->conditions.size() << std::endl;
                for (const auto& elem : switchData->condition->conditions) {
                    // std::cerr << "[*] Switch Condition Element: " << elem.GetElement().type().name() << std::endl;

                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        if (exprStmts) {
                            exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement()));
                        }
                        
                        std::shared_ptr<srcDispatch::ExpressionData> expr = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                        for (const auto& exprElem : expr->expr) {
                            if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                                controlVariables.push_back(std::any_cast<std::shared_ptr<srcDispatch::NameData>>(exprElem.GetElement()));
                            }
                        }
                    } else if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                        if (declStmts) {
                            declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement()));
                        }
                    }
                }

                for (const auto& switchCase : switchData->block->cases) {
                    // std::cerr << "[*] Iterating over Switch Data Cases. . ." << std::endl;
                    // std::cerr << "[*] Number of Switch Controls : " << controlVariables.size() << std::endl;
                    for (auto& ctrlVar : controlVariables) {
                        // std::cerr << "[*] Control Variable --> " << ctrlVar->SimpleName() << std::endl;

                        // locate the slice profile of the ctrlVar and insert the uses
                        auto sliceProfileItr = profileMap.find(ctrlVar->SimpleName());

                        // might need to add finger-printing to minimize potential issue
                        // of inserting data into the wrong slice
                        if (sliceProfileItr != profileMap.end()) {
                            // std::cerr << "[*] Adding Use for Switch Control Variable --> " << ctrlVar->SimpleName() << " | " << switchCase->expr->lineNumber << std::endl;
                            sliceProfileItr->second.back().uses.insert(switchCase->expr->lineNumber);
                        } else {
                            if (verboseMode) {
                                std::cerr << "[-] Could not find Slice Profile of: " << ctrlVar->SimpleName() << std::endl;
                            }
                        }
                    }
                }

                cntlBlocks.push_back(switchData->block.GetElement());
            } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
                // Extract all of the block data from For Loops
                std::shared_ptr<srcDispatch::ForData> forData = std::any_cast<std::shared_ptr<srcDispatch::ForData>>(cntl);

                for (auto& initData : forData->control->init->inits) {
                    if (initData.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                        if (declStmts) {
                            declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(initData.GetElement()));
                        }
                    } else if (initData.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        if (exprStmts) {
                            exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(initData.GetElement()));
                        }
                    }
                }

                for (const auto& elem : forData->control->condition->conditions) {
                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        if (exprStmts) {
                            exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement()));
                        }
                    }
                }

                loopdata.push_back(std::make_pair(forData->startLineNumber, forData->endLineNumber));
                forloopdata.push_back(std::make_pair(forData->startLineNumber, forData->endLineNumber));

                cntlBlocks.push_back(forData->block.GetElement());
            } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::WhileData>)) {
                // Extract all of the block data from While Loops
                std::shared_ptr<srcDispatch::WhileData> whileData = std::any_cast<std::shared_ptr<srcDispatch::WhileData>>(cntl);

                // C++ do-while does not support decl-stmts within the conditional "()"

                if (exprStmts) {
                    for (const auto& elem : whileData->condition->conditions) {
                        if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                            if (exprStmts) {
                                exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement()));
                            }
                        }
                    }
                }

                loopdata.push_back(std::make_pair(whileData->startLineNumber, whileData->endLineNumber));
                whileloopdata.push_back(std::make_pair(whileData->startLineNumber, whileData->endLineNumber));

                cntlBlocks.push_back(whileData->block.GetElement());
            } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::DoData>)) {
                // Extract all of the block data from Do-While Loops
                std::shared_ptr<srcDispatch::DoData> doWhileData = std::any_cast<std::shared_ptr<srcDispatch::DoData>>(cntl);

                // C++ do-while does not support decl-stmts within the conditional "()"

                if (exprStmts) {
                    for (const auto& elem : doWhileData->condition->conditions) {
                        if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                            if (exprStmts) {
                                exprStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement()));
                            }
                        }
                    }
                }

                loopdata.push_back(std::make_pair(doWhileData->startLineNumber, doWhileData->endLineNumber));
                dowhileloopdata.push_back(std::make_pair(doWhileData->startLineNumber, doWhileData->endLineNumber));

                cntlBlocks.push_back(doWhileData->block.GetElement());
            }
        }

        // Capture the Expressions and Decl-Stmts from the conditonal blocks
        for (const auto& block : cntlBlocks) {
            if (!block) continue;
            std::vector<std::any> conditionals;

            for (auto& stmt : block->statements) {
                if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExprStmtData>)) {
                    if (exprStmts) {
                        auto exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExprStmtData>>(stmt.GetElement());
                        exprStmts->push_back(exprstmt->expr.GetElement());
                    }
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclStmtData>)) {
                    // locals
                    std::shared_ptr<srcDispatch::DeclStmtData> declStmtData = std::any_cast<std::shared_ptr<srcDispatch::DeclStmtData>>(stmt.GetElement());

                    if (declStmts && declStmtData) {
                        for (auto& deltaDecl : declStmtData->decls) {
                            if (deltaDecl.GetElement()) {
                                declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(deltaDecl.GetElement()));
                            }
                        }
                    }
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ReturnData>)) {
                    // type of expression statement
                    if (exprStmts) {
                        auto retstmt = std::any_cast<std::shared_ptr<srcDispatch::ReturnData>>(stmt.GetElement());
                        if (retstmt) {
                            if (retstmt->expr && retstmt->expr.GetElement()) {
                                exprStmts->push_back(retstmt->expr.GetElement());
                            }
                        }
                    }
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CaseData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::WhileData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DoData>)) {
                    conditionals.push_back(stmt.GetElement());
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::TryData>)) {
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ThrowData>)) {
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::GotoData>)) {
                } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::LabelData>)) {
                } else {
                    if (verboseMode) {
                        std::cerr << "[-] " << __LINE__ << " : " << __FUNCTION__ << " | Unhandled Type -> " << stmt.GetElement().type().name() << std::endl;
                    }
                }
            }

            // Recursive call to step into nested conditionals
            if (conditionals.size() > 0) {
                CollectConditionalData(exprStmts, declStmts, conditionals);
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

    bool IsPointerDereferenced(std::shared_ptr<srcDispatch::NameData>& varNameElem) {
        std::string opStr;
        bool nameFound = false;

        for(const auto& deltaNameElem : varNameElem->names) {
            if(deltaNameElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                // *ptr | ptr->
                if (opStr == "*" || opStr == "->") {
                    return true;
                }

                // we do not need to iterate all the name_elements
                if (!nameFound) {
                    nameFound = true;
                } else {
                    // ptr->data | we are focusing on ptr (first var name) and whether it gets dereferenced
                    return false;
                }
            } else {
                opStr = std::any_cast<std::shared_ptr<srcDispatch::OperatorData>>(deltaNameElem.GetElement())->op;
            }
        }
        return false;
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

    std::vector<std::shared_ptr<VariableData>> ParseExpr(const srcDispatch::ExpressionData& expr, const unsigned int& lineNumber) {
        std::vector<std::shared_ptr<VariableData>> varDataGroup;
        std::string expr_op = "";
        std::shared_ptr<VariableData> lhsVar = std::make_shared<VariableData>();

        std::vector<std::shared_ptr<VariableData>> lhsStack;
        bool groupCollect = false, trailingPrefix = false, trailingExtraction = false;

        // loop through each element within a specific expression statement
        for (const auto& exprElem : expr.expr) {
            if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                std::shared_ptr<srcDispatch::NameData> name = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(exprElem.GetElement());
                if (name->SimpleName().empty()) continue;
                std::string varName = ExtractName(name->SimpleName());

                if (!lhsVar->isInitialized()) {
                    lhsVar->InitializeLHS(varName, lineNumber);
                    // any variable in an expression is a use of said variable
                    // unless the expression follows the sequence of a redefinition
                    // `VARIABLE_NAME =`
                    // lhsVar->uses.insert(lineNumber);

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

                    if (IsPointerDereferenced(name)) {
                        lhsVar->dereferenced = true;
                    }

                    if (expr_op == ">>" || trailingExtraction) {
                        lhsVar->definitions.insert(lineNumber);
                        lhsVar->userModified = true;
                        trailingExtraction = false;
                    }

                    if (name && !name->indices.empty()) {
                        // Extract the expression within the index operator and push all
                        // of the variable names as rhs of the lhs
                        for (const auto& indexExpr : name->indices) {
                            for (const auto& indexElem : indexExpr->expr) {
                                if (indexElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                                    // Create the new index data reference
                                    std::shared_ptr<srcDispatch::NameData> indexVar = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(indexElem.GetElement());
                                    std::string indexVarName = ExtractName(indexVar->SimpleName());

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

                    if (IsPointerDereferenced(name)) {
                        newRHSVar->dereferenced = true;
                    }

                    lhsVar->lhs = true;
                    lhsVar->AddRHS(newRHSVar);

                    // Ensure that tracked lhs variables get assigned all of their
                    // rhs vars in the expression
                    for (auto& lhs : lhsStack)
                        lhs->AddRHS(newRHSVar);

                    // Ensure the std::optional has a value before moving forward
                    if (name && !name->indices.empty()) {
                        // Extract the expression within the index operator and push all
                        // of the variable names as rhs of the lhs
                        for (const auto& indexExpr : name->indices) {
                            for (const auto& indexElem : indexExpr->expr) {
                                if (indexElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                                    // Create the new index data reference
                                    std::shared_ptr<srcDispatch::NameData> indexVar = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(indexElem.GetElement());
                                    std::string indexVarName = ExtractName(indexVar->SimpleName());

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
            } else if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::OperatorData>)) {
                std::shared_ptr<srcDispatch::OperatorData> opData = std::any_cast<std::shared_ptr<srcDispatch::OperatorData>>(exprElem.GetElement());
                expr_op = opData->op.ToString();

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
            } else if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CallData>)) {
                // This will read through the Call Args and attempt
                // to find the slice profile for the extracted arg
                // and will attempt to insert potential:
                // use/def/call data
                std::shared_ptr<srcDispatch::CallData> callData = std::any_cast<std::shared_ptr<srcDispatch::CallData>>(exprElem.GetElement());

                if (verboseMode) {
                    std::cerr << "[*] " << __LINE__  << " | Parsing For Targets: " << callData->name.ToString() << std::endl;
                }

                size_t dotPosition = callData->name.ToString().find('.');
                size_t hyphenPosition = callData->name.ToString().find('-');

                // Choose the first occurring delimiter (if any)
                size_t endPosition = std::min(dotPosition, hyphenPosition);

                if (endPosition == std::string::npos) {
                    endPosition = std::max(dotPosition, hyphenPosition);
                }
                
                if (endPosition == std::string::npos) {
                    endPosition = callData->name.ToString().length();  // No delimiter found
                } else {
                    // Create VariableData to use later to update slice of callTarget
                    // when delimiter if found
                    std::string callTarget = callData->name.ToString().substr(0, endPosition);
                    std::shared_ptr<VariableData> sourceVar = std::make_shared<VariableData>(callTarget);
                    sourceVar->uses.insert(callData->lineNumber);
                    sourceVar->SetOriginLine(callData->lineNumber);
                    varDataGroup.push_back(sourceVar);
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

        return varDataGroup;
    }

    void ProcessFunctionParameters(std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>>& parameters, const std::string& currentFunctionName,
                                    std::string className, std::vector<std::string>& containingNamespaces,
                                    const SliceCtx &ctx) {
        for (auto& parameter : parameters) {
            if (!parameter->name) continue;
            std::string paramName = parameter->name.ToString();
            paramName = paramName.substr(0, paramName.find('[')); // remove index operator from name of parameter

            // the Type string also includes the symbols along with data-type name
            // so this needs to be parsed out of the ToString() output
            std::string paramType = "";

            bool isPointer = false;
            bool isReference = false;
            bool isArray = false;

            // Make a function to pull data-type
            paramType = parameter->type.ToString();

            /*for(std::size_t pos = 0; pos < parameter->type->types.size(); ++pos) {
                const auto& type = (parameter->type->types[pos]);
                if (type.second == Dispatch::TypeData::POINTER) {
                    isPointer = true;
                    paramType += "*";
                } else if (type.second == Dispatch::TypeData::REFERENCE) {
                    isReference = true;
                    paramType += "&";
                } else if (type.second == Dispatch::TypeData::TYPENAME) {
                    std::shared_ptr<srcDispatch::NameData> nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(type.first);
                    paramType = nameData->SimpleName();

                    // remove `std ` in `std string` if neccessary
                    paramType = paramType.substr(paramType.find(' ')+1);

                    // attempt to mark raw-arrays for alias processing later
                    if (parameter->name && !parameter->name->indices.empty()) {
                        isArray = true;
                        paramType += "[]";
                    }
                }
            }*/

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
                sliceProf.isPotentialArray = isArray;

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
                sliceProf.isPotentialArray = isArray;

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

    void ProcessFunctionSignature(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::string className, std::vector<std::string>& containingNamespaces,
                                    const SliceCtx &ctx) {
        std::string functionName = funcData->name.ToString();
        
        if (functionName.empty()) return;
        bool updateSignature = (functionName.find("::") != std::string::npos);
        // std::string scopeName = "";

        // Update a Signature Entry due to out-of-line definition
        if (updateSignature) {
            // scopeName = functionName.substr(0, functionName.find("::"));
            functionName = functionName.substr(functionName.find_last_of("::")+1, -1);
        }

        // Process the parameters in a separate function
        auto& functionParameters = funcData->parameters;
        ProcessFunctionParameters(functionParameters, funcData->name.ToString(), className, containingNamespaces, ctx);
        auto funcSig = funcSigCollection.functionSigMap.find(functionName);

        if (funcSig != funcSigCollection.functionSigMap.end()) {
            if (updateSignature) {
                for (auto& func : funcSig->second) {
                    if (func->parameters.size() == functionParameters.size()) {
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
                    std::make_pair(functionName, std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>>{funcData})
                    );
        }
    }

    // Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
    // Logic constructed for use BEFORE InterProcedural
    SliceProfile* FetchSliceProfile(std::string profileName, const std::shared_ptr<VariableData>& vd, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData,
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
                    if (profile.function != funcData->name.ToString()) continue;
                if (profile.nameOfContainingClass != className) continue;
                if (profile.containingNameSpaces != containingNameSpaces) continue;

                if (vd) {
                    // Check if the numbers collected are less than the lineNumber (init def line)
                    if (!vd->uses.empty()) {
                        if (profile.lineNumber > *(vd->uses.begin())) continue;
                    }
                    if (!vd->definitions.empty()) {
                        if (*(vd->definitions.begin()) > profile.lineNumber) continue;
                    }
                }

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

            if (varData->dereferenced) {
                auto aliasReferenceItr = profileMap.find(sliceProfileItr->second.back().currentPointerReference);

                // Uses of the alias are uses of its reference
                if (aliasReferenceItr != profileMap.end()) {
                    // find the slice profile of the alias reference if one exists
                    aliasReferenceItr->second.back().definitions.insert(varData->definitions.begin(),
                                                                    varData->definitions.end());

                    /*
                    If in an expression an alias is dereferenced the alias's current reference
                    is being used if there are no redefinitions occuring.

                    If in an expression the alias appears in the lhs and rhs of an expression
                    where a variable is redefined this is a use-def chain.
                    */
                    bool lhsIsInRhs = false;
                    for (const auto& rhsElem : varData->rhsElems) {
                        if (rhsElem->GetNameOfIdentifier() == varData->GetNameOfIdentifier()) {
                            lhsIsInRhs = true;
                            break;
                        }
                    }
                    /*
                        cout << *ptr << endl; // use only
                        ptr->x = 23 // def only (state change of reference)
                        ptr->x = ptr->y + z; // def-use chain
                    */
                    if (lhsIsInRhs || varData->definitions.size() == 0) {
                        aliasReferenceItr->second.back().uses.insert(varData->uses.begin(),
                                                       varData->uses.end());
                    }
                }
            } else {
                sliceProfileItr->second.back().definitions.insert(varData->definitions.begin(),
                                                                varData->definitions.end());
            }
        } else {
            if (verboseMode)
                std::cerr << "[*] " << __LINE__ << " : " << __FUNCTION__ << " | There is no Slice of --> '" << varData->GetNameOfIdentifier() << "'" << std::endl;
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

    auto ArgumentProfile(std::pair<std::string, srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>> func, int paramIndex, std::unordered_set<std::string>& visit_func) {
        auto Spi = profileMap.find(func.second->parameters.at(paramIndex)->name.ToString());

        for (auto& param : func.second->parameters) {
            if (param->name) {
                if (profileMap.find(param->name.ToString())->second.back().visited) {
                    return Spi;
                } else {
                    if (profileMap.find(param->name.ToString())->second.back().cfunctions.size() > 0) {
                        for (auto& cfunc : profileMap.find(param->name.ToString())->second.back().cfunctions) {
                            if (cfunc.functionName == func.first) {
                                auto funcGroup = funcSigCollection.functionSigMap.find(cfunc.functionName);
                                if (funcGroup != funcSigCollection.functionSigMap.end()) {
                                    size_t pos = 0;
                                    srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> func = funcGroup->second[pos];

                                    // Attempt to fingerprint the right signature based on function call definition line and called function
                                    // def line data
                                    while (cfunc.functionDefinition != funcGroup->second[pos]->lineNumber) {
                                        if (++pos >= funcGroup->second.size()) break;
                                    }

                                    if (cfunc.functionName == func->name.ToString() && visit_func.find(cfunc.functionName) == visit_func.end()) {
                                        visit_func.insert(cfunc.functionName);
                                        // Ensure before we run ArgumentProfile that parameters has non-zero size and can be indexed safely
                                        if (cfunc.functionName == func->name.ToString() && func->parameters.size() > 0 &&
                                        cfunc.parameterIndex < func->parameters.size()) {
                                            if (func->parameters[cfunc.parameterIndex]->name) {
                                                // Only run this section if the parameter name can be extracted
                                                auto recursiveSpi = ArgumentProfile(std::make_pair(cfunc.functionName, func), cfunc.parameterIndex - 1, visit_func);
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
                                                            recursiveSpi->second.back().cfunctions.begin(),
                                                            recursiveSpi->second.back().cfunctions.end()
                                                    );
                                                    // ensure dependencies and aliases are within local scope
                                                    if (recursiveSpi->second.back().function == profileMap.find(param->name.ToString())->second.back().function) {
                                                        profileMap.find(param->name.ToString())->second.back().aliases.insert(
                                                                recursiveSpi->second.back().aliases.begin(),
                                                                recursiveSpi->second.back().aliases.end()
                                                        );
                                                        profileMap.find(param->name.ToString())->second.back().dvars.insert(
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
    void ComputeAliasInterprocedural() {
	    std::unordered_set <std::string> visited_alias;

	    for (auto& sliceGroup : profileMap) {
            // expand list of potential targets (aliases)
            for (auto& sp : sliceGroup.second) {
                for (auto& alias : sp.aliases) {
                    // view aliases of the slice profile
                    auto spi = profileMap.find(alias.first);
                    if (spi != profileMap.end()) {
                        // fingerprint the profile based on contained use
                        for (auto& aspi : spi->second) {
                            auto usesItr = std::find(aspi.uses.begin(), aspi.uses.end(), alias.second);
                            if (usesItr != aspi.uses.end()) {
                                // determine if the potential target is a pointer or reference
                                if (aspi.isPointer || aspi.isReference) {
                                    // check if the alias has been visited
                                    if (visited_alias.find(aspi.variableName) == visited_alias.end()) {
                                        // mark alias as visited so we dont review this alias entry again (circular dependence protection)
                                        visited_alias.insert(aspi.variableName);

                                        // push_back alias slice profile's aliases into the source slice aliases
                                        sp.aliases.insert(aspi.aliases.begin(), aspi.aliases.end());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void ComputeInterprocedural() {
	    std::unordered_set <std::string> visited_func;

	    for (auto& var : profileMap) {
            // Need to watch the Slices we attempt to dig into because we are collecting slices we have no interest in
            if (!profileMap.find(var.first)->second.back().visited && (var.second.back().variableName != "*LITERAL*")) {
                if (!var.second.back().cfunctions.empty()) {
                    for (auto& cfunc : var.second.back().cfunctions) {
                        auto funcGroup = funcSigCollection.functionSigMap.find(cfunc.functionName);

                        if(funcGroup != funcSigCollection.functionSigMap.end()) {
                            size_t pos = 0;
                            srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> func = funcGroup->second[pos];

                            // Attempt to fingerprint the right signature based on
                            // function call definition line and called function
                            // def line data
                            for (pos = 0; pos < funcGroup->second.size(); ++pos) {
                                if (cfunc.functionDefinition == funcGroup->second[pos]->lineNumber) {
                                    func = funcGroup->second[pos];
                                    break;
                                }
                            }

                            std::string simpleFunctionName = GetSimpleFunctionName(func->name.ToString());
                            unsigned int ArgProfParam = cfunc.parameterIndex - 1;

                            // Ensure before we run ArgumentProfile that parameters has non-zero size and can be indexed safely
                            if (cfunc.functionName == simpleFunctionName && func->parameters.size() > 0 &&
                                ArgProfParam < func->parameters.size() && pos < funcGroup->second.size()) { //TODO fix for case: Overload
                                if (func->parameters[ArgProfParam]->name) {
                                    // Only run this section if the parameter name can be extracted
                                    auto Spi = ArgumentProfile(std::make_pair(cfunc.functionName, func), ArgProfParam, visited_func);
                                    auto sliceItr = Spi->second.begin();
                                    std::string desiredVariableName = sliceItr->variableName;

                                    for (sliceItr = Spi->second.begin(); sliceItr != Spi->second.end(); ++sliceItr) {
                                        if (sliceItr->containsDeclaration) {
                                            if (sliceItr->variableName != desiredVariableName) {
                                                continue;
                                            }
                                            if (GetSimpleFunctionName(sliceItr->function) != cfunc.functionName) {
                                                continue;
                                            }
                                            std::string parameterDeclLine = std::to_string(func->parameters[ArgProfParam]->lineNumber);
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

                                            // set the line where the function call occurs to show as a def instead of a use
                                            profileMap.find(var.first)->second.back().uses.erase(cfunc.lineOfInvoke);
                                            profileMap.find(var.first)->second.back().definitions.insert(cfunc.lineOfInvoke);
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
                                            std::cerr << "[-] " << __LINE__  << " | An Error has Occured in `ComputeInterprocedural`" << std::endl;
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

private:
    void GenerateSlices() {
        // after collecting all units we parse them
        ProcessUnits();
        
        // Handles Collecting Control-Edges
        if (calculateControlEdges) ComputeControlPaths();
        
        // populates Aliases attribute in slice profiles and
        // performs crude interprocedural to connect use/def data
        ComputeAliasInterprocedural();

        ComputeInterprocedural();
    }

    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    std::vector<std::shared_ptr<srcDispatch::ClassData>> classInfo;
    std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>> functionInfo;

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> forloopdata;
    std::vector<std::pair<int, int>> whileloopdata;
    std::vector<std::pair<int, int>> dowhileloopdata;

    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elseifdata;
    std::vector<std::pair<int, int>> elsedata;

    std::vector<std::pair<SliceCtx, std::shared_ptr<srcDispatch::UnitData>>> units;
    FunctionSignatureData funcSigCollection;
    bool verboseMode, calculateControlEdges;
};

#endif