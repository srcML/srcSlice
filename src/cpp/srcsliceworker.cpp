#include <srcsliceworker.hpp>

SrcSliceWorker::SrcSliceWorker(std::shared_ptr<srcDispatch::UnitData> unitData, const srcDispatch::srcSAXEventContext &ctx,
                    bool verboseMode, bool calculateControlEdges) {
    // define logic flags
    data.verboseMode = verboseMode;
    data.calculateControlEdges = calculateControlEdges;
        
    sctx = SliceCtx(ctx);
    unit = unitData; // std::move(unitData);
    dispatched = false;
}
SrcSliceWorker::~SrcSliceWorker() {
    WaitForJob();
}

void SrcSliceWorker::Perform() {
    if (!unit) {
        std::cerr << "[-] Unit Information Invalid" << std::endl;
        jobFinished.store(true, std::memory_order_release);
        return;
    }

    // Process Global Decls
    SrcSliceOperations::ProcessDecls(data, sctx, unit->declStmts, "", true);
    // Capture Function Signatures
    SrcSliceOperations::ProcessSignatures(data, sctx, unit->functions, unit->classes);
    // Process Free Functions
    SrcSliceOperations::ProcessFunctions(data, sctx, unit->functions);
    // Process Classes
    SrcSliceOperations::ProcessClasses(data, sctx, unit->classes);

    jobFinished.store(true, std::memory_order_release);
}

void SrcSliceWorker::Start() {
    if (dispatched) return;
    dispatched = true;
    work = std::thread(&SrcSliceWorker::Perform, this);
}

// ensure the work thread has closed safely
void SrcSliceWorker::WaitForJob() {
    if (work.joinable())
        work.join();
}

bool SrcSliceWorker::Finished() {
    return jobFinished.load(std::memory_order_acquire);
}

//=======================================================================

std::string SrcSliceOperations::GenerateArrayType(std::string typeString, int dim) {
    for (int i = 0; i < dim;++i) {
        typeString += "[]";
    }
    return typeString;
};

bool SrcSliceOperations::StringContainsCharacters(std::string &str) {
    // test empty string
    if (str.empty())
        return false;

    // check first character
    if (!std::isalpha(str[0]) && str[0] != '_')
        return false;

    // check remaining characters
    for (char ch : str) {
        if (!std::isalnum(ch) && ch != '_')
            return false;
    }

    return true;
}

bool SrcSliceOperations::isAssignment(std::string& expr_op) {
    return (expr_op == "=") || isCompoundAssignment(expr_op);
}

bool SrcSliceOperations::isCompoundAssignment(std::string& expr_op) {
    return (expr_op == "+=") || (expr_op == "-=") || (expr_op == "*=") || (expr_op == "/=") || (expr_op == "%=");
}

bool SrcSliceOperations::isLogical(std::string& expr_op) {
    return ( (expr_op == "<") || (expr_op == ">") || (expr_op == "<=") || (expr_op == ">=") || (expr_op == "==")
            || (expr_op == "!=") || (expr_op == "&&" || (expr_op == "||")) );
}

bool SrcSliceOperations::isWhiteSpace(std::string& str) {
    return str.find_first_not_of(" \t\n\r") == std::string::npos;
}

// Extract the function name within either a call or a complex function name
std::string SrcSliceOperations::GetSimpleFunctionName(std::string funcName) {
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

bool SrcSliceOperations::IsPointerDereferenced(std::shared_ptr<srcDispatch::NameData>& varNameElem) {
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

// Take large name strings and extract the root variable name
std::string SrcSliceOperations::ExtractName(std::string elementName) {
    std::string target = "std::";

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

std::string SrcSliceOperations::GetTypeDetails(const DeclInfo& localVar, bool& isPointer, bool& isReference, bool& isArray) {
    std::string t;

    // Extract just the Data-Type name without extra data
    for (std::size_t i = 0; i < localVar->type->types.size(); ++i) {
        const auto type = localVar->type->types[i];

        if(type.second.GetElement() == srcDispatch::TypeData::POINTER) {
            t += '*';
        } else if(type.second.GetElement() == srcDispatch::TypeData::REFERENCE) {
            t += '&';
        } else if(type.second.GetElement() == srcDispatch::TypeData::RVALUE) {
            t += "&&";
        } else if(type.second.GetElement() == srcDispatch::TypeData::TYPENAME) {
            t += type.first.ToString<std::shared_ptr<srcDispatch::NameData>>(srcDispatch::DiffOperation::NONE);
        }

        if (type.second.GetElement() == srcDispatch::TypeData::POINTER) {
            isPointer = true;
        } else if (type.second.GetElement() == srcDispatch::TypeData::REFERENCE) {
            isReference = true;
        } else if (type.second.GetElement() == srcDispatch::TypeData::TYPENAME) {
            // attempt to mark raw-arrays for alias processing later
            if (localVar->name && localVar->name->indices.size() > 0) {
                isArray = true;
            }
        }
    }

    return t;
}

//=======================================================================

void SrcSliceOperations::ProcessDecls(Blob& data, const SliceCtx& sctx, DeclStmts& deltaDeclStmts, std::string className, bool globalDecls) {
    // iterate the declstmts and review decldata
    for (const auto& deltaDeclStmt : deltaDeclStmts) {
        // validate data is safe to access
        if (deltaDeclStmt) {
            // iterate decldata collection
            for (const auto& deltaDecl : deltaDeclStmt->decls) {
                // validate data is safe to access
                if (deltaDecl) {
                    if (!deltaDecl->name) continue;

                    // Collect pieces about the newly declared variable to use later when adding it into
                    // our profileMap
                    std::string declVarName = deltaDecl->name.ToString();
                    declVarName = declVarName.substr(0, declVarName.find('[')); // remove index operator characters from variable names
                    std::string declVarType = "";

                    bool isPointer = false;
                    bool isReference = false;
                    bool isArray = false;

                    // Make a function to pull data-type
                    declVarType = GetTypeDetails(deltaDecl.GetElement(), isPointer, isReference, isArray);
                    // remove all spaces from type string
                    declVarType.erase(std::remove(declVarType.begin(), declVarType.end(), ' '), declVarType.end());

                    auto sliceProfileItr = data.profileMap.find(declVarName);
                    SlicePosition namePos(deltaDecl->name->startPosition, deltaDecl->name->endPosition, sctx.currentFilePath);
                    
                    auto sliceProfile = SliceProfile(declVarName, namePos, isPointer, true, {namePos});
                    sliceProfile.variableType = isArray ? SrcSliceOperations::GenerateArrayType(declVarType, deltaDecl->name->indices.size()) : declVarType;
                    sliceProfile.nameOfContainingClass = className;
                    sliceProfile.classMemberVar = className.empty() ? false : true;

                    sliceProfile.containingNameSpaces = {}; /*** @todo find a link for namespaces on globals */
                    sliceProfile.language = sctx.currentFileLanguage;
                    sliceProfile.file = sctx.currentFilePath;

                    sliceProfile.showControlEdges = data.calculateControlEdges;

                    sliceProfile.isPointer = isPointer;
                    sliceProfile.isReference = isReference;
                    sliceProfile.isPotentialArray = isArray;

                    sliceProfile.containsDeclaration = true;

                    //Just add new slice profile if name already exists. Otherwise, add new entry in map.
                    if (sliceProfileItr != data.profileMap.end()) {
                        // Check if the new slice we potentially try to create has not already been made
                        // (we dont want to have duplicates of the same slice)
                        sliceProfile.isGlobal = true;

                        // We may have variables of the same name, but each slice of the same name
                        // must be initially declared on different lines
                        if (sliceProfileItr->second.back() != sliceProfile) {
                            sliceProfileItr->second.push_back(sliceProfile);
                        }
                    } else {
                        sliceProfile.isGlobal = false;

                        // point the iterator to the newly inserted profile element
                        sliceProfileItr = data.profileMap.insert(
                            std::make_pair(declVarName, std::vector<SliceProfile>{ std::move(sliceProfile) })
                        ).first;
                    }

                    if (deltaDecl->init) {
                        SrcSliceOperations::ParseExpr(
                            data,
                            sctx,
                            deltaDecl->init,
                            EXPRESSION_TYPE::NORMAL,
                            {declVarName}
                        );
                    }
                }
            }
        }
    }
}

void SrcSliceOperations::ProcessSignatures(Blob& data, const SliceCtx& sctx, Functions& funcs, Classes& classes) {
    if (data.verboseMode) {
        std::cout << "[*] " << __LINE__  << " Processing Signatures" << std::endl;
    }

    // Fetch Signatures from Free-Functions
    for (auto& func : funcs) {
        ProcessFunctionSignature(data, sctx, func, "");
    }

    // Fetch Signatures from Classes
    for (auto& classData : classes) {
        // Process Class Contructors
        for (auto& deltaFuncElem : classData->constructors) {
            ProcessFunctionSignature(data, sctx, deltaFuncElem, classData->name.ToString());
        }

        // Process Class Dtor
        if (classData->destructor && classData->destructor.GetElement()) {
            ProcessFunctionSignature(data, sctx, classData->destructor, classData->name.ToString());
        }

        // Process Class Methods (Member Functions)
        for (auto& deltaFuncElem : classData->methods) {
            ProcessFunctionSignature(data, sctx, deltaFuncElem, classData->name.ToString());
        }

        // Process Operator Overloading
        for (auto& deltaFuncElem : classData->operators) {
            ProcessFunctionSignature(data, sctx, deltaFuncElem, classData->name.ToString());
        }

        // Process Nested Classes
        ProcessClasses(data, sctx, classData->innerClasses);
    }
}

void SrcSliceOperations::ProcessFunctions(Blob& data, const SliceCtx& sctx, Functions& funcs) {
    for (auto& func : funcs) {
        if (data.verboseMode) {
            std::cout << "[*] " << __LINE__  << " Processing Function Name: " << func->name.ToString() << std::endl;
        }
        ProcessInitLists(data, sctx, func, "");
        if (func->block) ProcessStmts(data, sctx, func, func->block, "");
    }
}

void SrcSliceOperations::ProcessClasses(Blob& data, const SliceCtx& sctx, Classes& classes) {
    if (classes.empty()) return;
    for (auto& classData : classes) {
        if (data.verboseMode) {
            std::cout << "[*] " << __LINE__  << " Processing Class Name: " << classData->name.ToString() << std::endl;
        }

        // Process Class Member Variables
        ProcessDecls(data, sctx, classData->fields, classData->name.ToString());

        // Process Class Contructors
        for (auto& deltaFuncElem : classData->constructors) {
            ProcessInitLists(data, sctx, deltaFuncElem, classData->name.ToString());
            if (deltaFuncElem->block) ProcessStmts(data, sctx, deltaFuncElem, deltaFuncElem->block, classData->name.ToString());
        }
        // Process Class Dtor
        if (classData->destructor && classData->destructor.GetElement()) {
            if (classData->destructor->block) ProcessStmts(data, sctx, classData->destructor, classData->destructor->block, classData->name.ToString());
        }

        // Process Class Methods (Member Functions)
        for (auto& deltaFuncElem : classData->methods) {
            if (deltaFuncElem->block) ProcessStmts(data, sctx, deltaFuncElem, deltaFuncElem->block, classData->name.ToString());
        }

        // Process Operator Overloading
        for (auto& deltaFuncElem : classData->operators) {
            if (deltaFuncElem->block) ProcessStmts(data, sctx, deltaFuncElem, deltaFuncElem->block, classData->name.ToString());
        }

        // Process Nested Classes
        ProcessClasses(data, sctx, classData->innerClasses);
    }
}

void SrcSliceOperations::CreateSliceProfile(Blob& data, const SliceCtx& sctx, const DeclInfo& deltaDeclData, const FunctionInfo& funcData,
                                            std::string className) {
    if (!deltaDeclData) return;
    if (!deltaDeclData->name) return;
    if (deltaDeclData->name.ToString().empty()) return;

    // Collect pieces about the newly declared variable to use later when adding it into
    // our profileMap
    std::string declVarName = deltaDeclData->name.ToString();
    declVarName = declVarName.substr(0, declVarName.find('[')); // remove index operator characters from variable names
    std::string declVarType = "";

    bool isPointer = false;
    bool isReference = false;
    bool isArray = false;

    SlicePosition namePos(deltaDeclData->name->startPosition, deltaDeclData->name->endPosition, sctx.currentFilePath);
    
    // Make a function to pull data-type
    declVarType = GetTypeDetails(deltaDeclData, isPointer, isReference, isArray);
    // remove all spaces from type string
    declVarType.erase(std::remove(declVarType.begin(), declVarType.end(), ' '), declVarType.end());

    auto sliceProfileItr = data.profileMap.find(declVarName);

    SliceProfile sliceProfile = SliceProfile(declVarName, namePos, isPointer, true, {namePos});

    sliceProfile.nameOfContainingClass = className;

    sliceProfile.language = sctx.currentFileLanguage;
    sliceProfile.file = sctx.currentFilePath;
    sliceProfile.checksum = sctx.currentFileChecksum;

    sliceProfile.showControlEdges = data.calculateControlEdges;
    sliceProfile.variableType = isArray ? SrcSliceOperations::GenerateArrayType(declVarType, deltaDeclData->name->indices.size()) : declVarType;
    
    sliceProfile.isPointer = isPointer;
    sliceProfile.isReference = isReference;
    sliceProfile.isPotentialArray = isArray;

    //Just add new slice profile if name already exists. Otherwise, add new entry in map.
    if (sliceProfileItr != data.profileMap.end()) {
        // Check if the new slice we potentially try to create has not already been made
        // (we dont want to have duplicates of the same slice)

        // We may have variables of the same name, but each slice of the same name
        // must be initially declared on different lines
        if (sliceProfileItr->second.back() != sliceProfile) {
            sliceProfile.containsDeclaration = true;
            sliceProfileItr->second.push_back(sliceProfile);
        }
    } else {
        sliceProfile.containsDeclaration = true;
        // point the iterator to the newly inserted profile element
        sliceProfileItr = data.profileMap.insert(
            std::make_pair(declVarName, std::vector<SliceProfile>{ std::move(sliceProfile) })
        ).first;
    }

    if (funcData) {
        // Link the function this slice is located in
        if (funcData->name) {
            sliceProfileItr->second.back().function = funcData->name.ToString();
        }

        // Use function data to attach contained namespaces
        sliceProfileItr->second.back().containingNameSpaces = funcData->namespaces;
    }

    if (deltaDeclData->init) {
        ParseExpr(
            data,
            sctx,
            deltaDeclData->init,
            EXPRESSION_TYPE::NORMAL,
            {declVarName}
        );
    }
}


void SrcSliceOperations::ProcessInitLists(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::string className) {
    if (!funcData) return;

    // process C++ initializer lists
    for (const auto& deltaCallData : funcData->memberInitList) {
        std::vector<ExprInfo> exprStmts;
        
        SlicePosition callPos(deltaCallData->startPosition, deltaCallData->endPosition, sctx.currentFilePath);

        // Insert def line for the valid slice used in the initList
        std::string varName = deltaCallData->name.ToString();

        // Find the slice for the "call-target"
        auto spi = data.profileMap.find(varName);

        if (spi != data.profileMap.end()) {
            spi->second.back().definitions.insert(callPos);
        }

        // extract and parse expressions within init list call
        for (const auto& deltaArg : deltaCallData->arguments) {
            std::vector<std::string> lhsStack = {};
            if (spi != data.profileMap.end()) lhsStack.push_back(varName);

            // process the argument of the initializer-list
            ParseExpr(data, sctx, deltaArg, EXPRESSION_TYPE::NORMAL, lhsStack);
        }
    }
}

void SrcSliceOperations::ProcessStmts(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, const BlockInfo& block,
                                    std::string className) {
    std::vector<std::string> containingNamespaces;

    // Capture Conditional expressions
    if (funcData && block) {
        containingNamespaces = funcData->namespaces;

        for (auto& stmt : block->statements) {
            if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExprStmtData>)) {
                auto exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExprStmtData>>(stmt.GetElement());
                if (exprstmt && exprstmt->expr && exprstmt->expr.GetElement()) {
                    // Capture general expressions
                    ProcessExprStmt(data, sctx, exprstmt->expr, funcData, className);
                }
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ReturnData>)) {
                auto retstmt = std::any_cast<std::shared_ptr<srcDispatch::ReturnData>>(stmt.GetElement());
                if (retstmt && retstmt->expr && retstmt->expr.GetElement()) {
                    // Capture general Return expressions
                    ProcessExprStmt(data, sctx, retstmt->expr, funcData, className);
                }
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
                // capture decls
                auto ifStmtData = std::any_cast<std::shared_ptr<srcDispatch::IfStmtData>>(stmt.GetElement());
                if (ifStmtData) {
                    for (const auto& clause : ifStmtData->clauses) {
                        if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfData>)) {
                            auto ifData = std::any_cast<std::shared_ptr<srcDispatch::IfData>>(clause.GetElement());
                            if (ifData && ifData->condition) {
                                for (auto& elem : ifData->condition->conditions) {
                                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                        auto initData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());
                                        if (initData) {
                                            CreateSliceProfile(data, sctx, initData, funcData, className);
                                        }
                                    }
                                }
                            }
                        } else if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ElseIfData>)) {
                            auto elseIfData = std::any_cast<std::shared_ptr<srcDispatch::ElseIfData>>(clause.GetElement());
                            if (elseIfData && elseIfData->condition) {
                                for (auto& elem : elseIfData->condition->conditions) {
                                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                        auto initData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());
                                        if (initData) {
                                            CreateSliceProfile(data, sctx, initData, funcData, className);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // capture expressions
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
                // capture decls
                auto switchData = std::any_cast<std::shared_ptr<srcDispatch::SwitchData>>(stmt.GetElement());
                if (switchData && switchData->condition) {
                    for (const auto& elem : switchData->condition->conditions) {
                        if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                            auto initData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());
                            if (initData) {
                                CreateSliceProfile(data, sctx, initData, funcData, className);
                            }
                        }
                    }
                }

                // capture expressions
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CaseData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::WhileData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
                // capture decls
                auto forData = std::any_cast<std::shared_ptr<srcDispatch::ForData>>(stmt.GetElement());
                if (forData && forData->control && forData->control->init) {
                    for (auto& initData : forData->control->init->inits) {
                        if (initData.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                            auto initDeclData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(initData.GetElement());
                            if (initDeclData) {
                                CreateSliceProfile(data, sctx, initDeclData, funcData, className);
                            }
                        }
                    }
                }

                // capture expressions
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DoData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::TryData>)) {
                // For Java we have try-with-resources, make slice profiles of the resources
                auto tryData = std::any_cast<std::shared_ptr<srcDispatch::TryData>>(stmt.GetElement());

                if (tryData) {
                    // catches have parameter lists with decldata (capture decls)
                    for (auto& clause : tryData->clauses) {
                        if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CatchData>)) {
                            auto catchData = std::any_cast<std::shared_ptr<srcDispatch::CatchData>>(clause.GetElement());
                            if (catchData) {
                                for (auto& parameter : catchData->parameters) {
                                    if (parameter) {
                                        CreateSliceProfile(data, sctx, parameter, funcData, className);
                                    }
                                }
                            }
                        }
                    }

                    // capture expressions
                    CollectTryBlockData(data, sctx, funcData, tryData, className);
                }
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ThrowData>)) {
                auto throwData = std::any_cast<std::shared_ptr<srcDispatch::ThrowData>>(stmt.GetElement());
                if (throwData && throwData->expr && throwData->expr.GetElement()) {
                    ProcessExprStmt(data, sctx, throwData->expr, funcData, className);
                }
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclStmtData>)) {
                // local decls
                auto declStmtData = std::any_cast<std::shared_ptr<srcDispatch::DeclStmtData>>(stmt.GetElement());
                if (declStmtData) {
                    for (auto& deltaDecl : declStmtData->decls) {
                        if (deltaDecl) {
                            CreateSliceProfile(data, sctx, deltaDecl, funcData, className);
                        }
                    }
                }
            } else {
                if (data.verboseMode) {
                    std::cout << "[-] " << __LINE__ << " : " << __FUNCTION__ << " | Unhandled Type -> " << stmt.GetElement().type().name() << std::endl;
                }
            }
        }
    }
}

void SrcSliceOperations::ProcessExprStmt(Blob& data, const SliceCtx& sctx, const ExprInfo& expr, const FunctionInfo& funcData,
                                        std::string className, EXPRESSION_TYPE expr_type) {
    if (expr) {
        ParseExpr(data, sctx, expr, expr_type);
    }
}

// Use collected function call data to push a new cfunctions entry into a referenced slice profile
// call data gets passed into aliases if needed
void SrcSliceOperations::CreateSliceCallData(Blob& data, const SliceCtx& sctx, std::string functionName, int argIndex, SlicePosition functionPosition,
                                            SliceProfile& sliceProfile, SlicePosition invokePosition) {
    FunctionCallData sliceCallData = FunctionCallData(
        functionName, // function call name
        argIndex, // arg index starting from 1 to n
        functionPosition, // function definition position
        invokePosition // line:col scope where the function call occurs
    );

    // push the cfunc data into the argument SliceProfile
    sliceProfile.insertCfunction(sliceCallData);

    // attempt to mark the cfunc towards the currentPointerReference

    std::vector<std::string> visited; // ensure we do not enter circular dependence
    std::string referenceName = sliceProfile.currentPointerReference;
    SliceProfileIterator aspi = data.profileMap.find(referenceName);

    while (aspi != data.profileMap.end()) {
        if (std::find(visited.begin(), visited.end(), referenceName) != visited.end()) break;
        visited.push_back(referenceName);
        
        // hidden metadata to tell InterProcedural to not process this cfunctions element
        sliceCallData.ignore = true;
        aspi->second.back().insertCfunction(sliceCallData);

        // Alias targets are used within function calls by default as it is an expression
        aspi->second.back().uses.insert(invokePosition);

        // iterate down the reference chain
        referenceName = aspi->second.back().currentPointerReference;
        aspi = data.profileMap.find(referenceName);
    }

    // Once a pointer is passed into a function-call there is no way to determine
    // if the object its pointing to has changed
    if (sliceProfile.isPointer) sliceProfile.ignorePtrRef = true;
}

// try blocks contain both exprs and decls, need to extract those decls and create slice profiles
// for them, along with capturing expressions to update collected slices
void SrcSliceOperations::CollectTryBlockData(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::shared_ptr<srcDispatch::TryData>& tryBlock,
                            std::string className) {
    if (tryBlock->block) {
        // Collect Decls and Exprs within the block of this Try-Block
        ProcessStmts(data, sctx, funcData, tryBlock->block, className);
    }

    // Collect Decls and Exprs within the catch block
    for (auto& clause : tryBlock->clauses) {
        if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CatchData>)) {
            auto catchData = std::any_cast<std::shared_ptr<srcDispatch::CatchData>>(clause.GetElement());
            if (catchData->block) {
                ProcessStmts(data, sctx, funcData, catchData->block, className);
            }
        }
    }
}

void SrcSliceOperations::CollectConditionalData(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::any& cntl, const std::string& className) {
    std::vector<std::shared_ptr<srcDispatch::BlockData>> cntlBlocks;

    // identify what conditional type we are viewing and handle logic accordingly
    if (cntl.type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
        // Extract all of the block data from if statements
        std::shared_ptr<srcDispatch::IfStmtData> ifcntl = std::any_cast<std::shared_ptr<srcDispatch::IfStmtData>>(cntl);
        if (ifcntl) {
            SlicePosition ifStmtPos(ifcntl->startPosition, ifcntl->endPosition, sctx.currentFilePath);
            if (data.ifStmts.size() == 0 || data.ifStmts.back() != ifStmtPos)
                data.ifStmts.push_back(ifStmtPos);

            // ifstmts have three potential clauses (if-elseif-else)
            for (const auto& clause : ifcntl->clauses) {
                if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfData>)) {
                    std::shared_ptr<srcDispatch::IfData> ifData = std::any_cast<std::shared_ptr<srcDispatch::IfData>>(clause.GetElement());
                    if (ifData) {
                        SlicePosition ifPos(ifData->startPosition, ifData->endPosition, sctx.currentFilePath);

                        if (ifData && ifData->condition) {
                            for (auto& elem : ifData->condition->conditions) {
                                if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                    // if (declStmts) {
                                    //     declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement()));
                                    // }
                                } else if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                                    std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                                    ProcessExprStmt(data, sctx, exprstmt, funcData, className, EXPRESSION_TYPE::IF_CONDITION);
                                }
                            }
                        }

                        // minimize duplicate entries
                        if (data.ifdata.size() == 0 || data.ifdata.back() != ifPos)
                            data.ifdata.push_back(ifPos);

                        // track the block so we can view nesting
                        if (ifData->block && ifData->block.GetElement())
                            cntlBlocks.push_back(ifData->block.GetElement());
                    }
                } else if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ElseIfData>)) {
                    std::shared_ptr<srcDispatch::ElseIfData> elseIfData = std::any_cast<std::shared_ptr<srcDispatch::ElseIfData>>(clause.GetElement());
                    if (elseIfData) {
                        SlicePosition elifPos(elseIfData->startPosition, elseIfData->endPosition, sctx.currentFilePath);

                        if (elseIfData && elseIfData->condition) {
                            for (auto& elem : elseIfData->condition->conditions) {
                                if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                                    // if (declStmts) {
                                    //     declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement()));
                                    // }
                                } else if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                                    std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                                    ProcessExprStmt(data, sctx, exprstmt, funcData, className, EXPRESSION_TYPE::ELIF_CONDITION);
                                }
                            }
                        }
                        
                        // track elseif block data for later usage
                        if (data.elseifdata.size() == 0 || data.elseifdata.back() != elifPos)
                            data.elseifdata.push_back(elifPos);

                        // track blocks for checking nesting
                        if (elseIfData->block && elseIfData->block.GetElement())
                            cntlBlocks.push_back(elseIfData->block.GetElement());
                    }
                } else if (clause.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ElseData>)) {
                    std::shared_ptr<srcDispatch::ElseData> elseData = std::any_cast<std::shared_ptr<srcDispatch::ElseData>>(clause.GetElement());
                    if (elseData) {
                        SlicePosition elsePos(elseData->startPosition, elseData->endPosition, sctx.currentFilePath);

                        // minimize duplicate entries
                        if (data.elsedata.size() == 0 || data.elsedata.back() != elsePos)
                            data.elsedata.push_back(elsePos);

                        // track blocks for checking nesting
                        if (elseData->block && elseData->block.GetElement())
                            cntlBlocks.push_back(elseData->block.GetElement());
                    }
                }
            }
        }
    } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
        // Extract all of the block data from Switch statements
        std::shared_ptr<srcDispatch::SwitchData> switchData = std::any_cast<std::shared_ptr<srcDispatch::SwitchData>>(cntl);
        if (switchData) {
            /*
                Ensure we are getting the uses from the case lines
                Ensure we capture data from the case blocks as well
            */

            // Connect the use lines of switch cases to their corresponding control variables
            std::vector<std::shared_ptr<srcDispatch::NameData>> controlVariables;

            if (switchData->condition) {
                for (const auto& elem : switchData->condition->conditions) {
                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                        ProcessExprStmt(data, sctx, exprstmt, funcData, className);
                        
                        std::shared_ptr<srcDispatch::ExpressionData> expr = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                        for (const auto& exprElem : expr->expr) {
                            if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                                controlVariables.push_back(std::any_cast<std::shared_ptr<srcDispatch::NameData>>(exprElem.GetElement()));
                            }
                        }
                    } else if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                        std::shared_ptr<srcDispatch::DeclData> declData = std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(elem.GetElement());

                        /***
                            @todo
                            - ensure switch condition following init-statement; expression, the init is not a control-variable
                            - ensure switch condition following expression; init-statement, the init is a control-variable and the expression is not
                        */

                        // add the name of the decl var as a control variable of the switch
                        // and add a use to the slice of the control-var
                        if (controlVariables.empty()) {
                            if (declData->name) {
                                controlVariables.push_back(declData->name.GetElement());
                            }

                            auto switchCtrlSlice = data.profileMap.find(declData->name.ToString());
                            if (switchCtrlSlice != data.profileMap.end()) {
                                switchCtrlSlice->second.back().uses.insert(
                                    SlicePosition(declData->name->startPosition, declData->name->endPosition, sctx.currentFilePath)
                                );
                            } else {
                                if (data.verboseMode) {
                                    std::cout << "[-] " << __LINE__  << " | Could not find Slice Profile of: " << declData->name.ToString() << std::endl;
                                }
                            }
                        }
                    }
                }
            }

            if (switchData->block) {
                for (const auto& switchCase : switchData->block->cases) {
                    for (auto& ctrlVar : controlVariables) {
                        srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::NameData>> deltaName(ctrlVar);
                        // locate the slice profile of the ctrlVar and insert the uses
                        auto sliceProfileItr = data.profileMap.find(deltaName.ToString());

                        // might need to add finger-printing to minimize potential issue
                        // of inserting data into the wrong slice
                        if (sliceProfileItr != data.profileMap.end()) {
                            /*** @todo iterate the expr and grab the name data matching deltaName.ToString() ***/
                            sliceProfileItr->second.back().uses.insert(
                                SlicePosition(switchCase->expr->startPosition, switchCase->expr->endPosition, sctx.currentFilePath)
                            );
                        } else {
                            if (data.verboseMode) {
                                std::cout << "[-] " << __LINE__  << " | Could not find Slice Profile of: " << deltaName.ToString() << std::endl;
                            }
                        }
                    }
                }

                if (switchData->block.GetElement()) cntlBlocks.push_back(switchData->block.GetElement());
            }

        }
    } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
        // Extract all of the block data from For Loops
        std::shared_ptr<srcDispatch::ForData> forData = std::any_cast<std::shared_ptr<srcDispatch::ForData>>(cntl);
        if (forData && forData->control && forData->control) {
            if (forData->control->init) {
                for (auto& initData : forData->control->init->inits) {
                    if (initData.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclData>)) {
                        // if (declStmts) {
                        //     declStmts->push_back(std::any_cast<std::shared_ptr<srcDispatch::DeclData>>(initData.GetElement()));
                        // }
                    } else if (initData.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(initData.GetElement());
                        ProcessExprStmt(data, sctx, exprstmt, funcData, className);
                    }
                }
            }

            if (forData->control->condition) {
                for (const auto& elem : forData->control->condition->conditions) {
                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                        ProcessExprStmt(data, sctx, exprstmt, funcData, className);
                    }
                }
            }

            if (forData->control->incr) {
                for (const auto& expr : forData->control->incr->exprs) {
                    ProcessExprStmt(data, sctx, expr, funcData, className);
                }
            }
        }

        SlicePosition forPos(forData->startPosition, forData->endPosition, sctx.currentFilePath);

        data.loopdata.push_back(forPos);
        data.forloopdata.push_back(forPos);

        cntlBlocks.push_back(forData->block.GetElement());
    } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::WhileData>)) {
        // Extract all of the block data from While Loops
        std::shared_ptr<srcDispatch::WhileData> whileData = std::any_cast<std::shared_ptr<srcDispatch::WhileData>>(cntl);
        if (whileData) {
            // C++ do-while does not support decl-stmts within the conditional "()"

            if (whileData->condition) {
                for (const auto& elem : whileData->condition->conditions) {
                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                        ProcessExprStmt(data, sctx, exprstmt, funcData, className);
                    }
                }
            }

            SlicePosition whilePos(whileData->startPosition, whileData->endPosition, sctx.currentFilePath);

            data.loopdata.push_back(whilePos);
            data.whileloopdata.push_back(whilePos);

            if (whileData->block && whileData->block.GetElement()) cntlBlocks.push_back(whileData->block.GetElement());
        }
    } else if (cntl.type() == typeid(std::shared_ptr<srcDispatch::DoData>)) {
        // Extract all of the block data from Do-While Loops
        std::shared_ptr<srcDispatch::DoData> doWhileData = std::any_cast<std::shared_ptr<srcDispatch::DoData>>(cntl);
        if (doWhileData) {
            // C++ do-while does not support decl-stmts within the conditional "()"

            if (doWhileData->condition) {
                for (const auto& elem : doWhileData->condition->conditions) {
                    if (elem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExpressionData>)) {
                        std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(elem.GetElement());
                        ProcessExprStmt(data, sctx, exprstmt, funcData, className);
                    }
                }
            }

            SlicePosition whilePos(doWhileData->startPosition, doWhileData->endPosition, sctx.currentFilePath);

            // data.loopdata.push_back(whilePos);
            data.dowhileloopdata.push_back(whilePos);

            if (doWhileData->block && doWhileData->block.GetElement()) cntlBlocks.push_back(doWhileData->block.GetElement());
        }
    } else {
        if (data.verboseMode) {
            if (data.verboseMode) {
                std::cout << "[-] " << __LINE__ << " : " << __FUNCTION__ << " | Unhandled Type -> " << cntl.type().name() << std::endl;
            }
        }
    }

    // Capture the Expressions and Decl-Stmts from the conditonal blocks
    for (const auto& block : cntlBlocks) {
        if (!block) continue;
        std::vector<std::any> conditionals;

        // capture decls nested within blocks of conditionals before parsing expressions
        // within the block
        ProcessStmts(data, sctx, funcData, block, "");

        for (auto& stmt : block->statements) {
            if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ExprStmtData>)) {
                std::shared_ptr<srcDispatch::ExprStmtData> exprstmtdata = std::any_cast<std::shared_ptr<srcDispatch::ExprStmtData>>(stmt.GetElement());
                ProcessExprStmt(data, sctx, exprstmtdata->expr, funcData, className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DeclStmtData>)) {
                // locals
                std::shared_ptr<srcDispatch::DeclStmtData> declStmtData = std::any_cast<std::shared_ptr<srcDispatch::DeclStmtData>>(stmt.GetElement());
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ReturnData>)) {
                // type of expression statement
                auto retstmt = std::any_cast<std::shared_ptr<srcDispatch::ReturnData>>(stmt.GetElement());
                if (retstmt) {
                    if (retstmt->expr && retstmt->expr.GetElement()) {
                        std::shared_ptr<srcDispatch::ExpressionData> exprstmt = std::any_cast<std::shared_ptr<srcDispatch::ExpressionData>>(retstmt->expr.GetElement());
                        ProcessExprStmt(data, sctx, exprstmt, funcData, className);
                    }
                }
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::IfStmtData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::SwitchData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CaseData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::WhileData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::ForData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else if (stmt.GetElement().type() == typeid(std::shared_ptr<srcDispatch::DoData>)) {
                CollectConditionalData(data, sctx, funcData, stmt.GetElement(), className);
            } else {
                if (data.verboseMode) {
                    std::cout << "[-] " << __LINE__ << " : " << __FUNCTION__ << " | Unhandled Type -> " << stmt.GetElement().type().name() << std::endl;
                }
            }
        }
    }
}

void SrcSliceOperations::ParseExpr(Blob& data, const SliceCtx& sctx, const ExprInfo& expr, EXPRESSION_TYPE expr_type,
                                std::vector<std::string> lhsStack, bool isArg,
                                srcDispatch::CallData* funcCallData, int argIndex) {
    ExprParse::ExprCtx ectx(data.profileMap, &lhsStack);

    // lhsStack -> transparent stack where begin() is outter LHS and end() is inner LHS
    std::string expr_op = ""; // most recently encountered operator token
    std::string recent_name = ""; // most recently encountered name token

    // loop through each element within a specific expression statement
    for (const auto& exprElem : expr->expr) {
        if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
            try {
                std::shared_ptr<srcDispatch::NameData> nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(exprElem.GetElement());
                if (!nameData) continue;
                
                if (ectx.firstNameData == nullptr) ectx.firstNameData = &(*nameData);

                ectx.namePos = SlicePosition(nameData->startPosition, nameData->endPosition, sctx.currentFilePath);

                // apply meta data about the current position
                ectx.namePos.GetData().isIfCondition = expr_type == EXPRESSION_TYPE::IF_CONDITION;
                ectx.namePos.GetData().isElifCondition = expr_type == EXPRESSION_TYPE::ELIF_CONDITION;
                
                bool simpleName = nameData->indices.empty() && nameData->names.empty();
                std::string name = simpleName ? nameData->name.ToString() : ExprParse::FindName(nameData->names, ectx.namePos);

                if (name.empty()) continue;

                // parse the expressions of indices
                for (const ExprInfo& exprInfo : nameData->indices) {
                    ParseExpr(data, sctx, exprInfo, expr_type, {});
                }


                recent_name = SrcSliceOperations::ExtractName(name);
                ectx.spi = data.profileMap.find(recent_name);

                // ensure all LHS are given their dvar/alias data
                ExprParse::updateLHS(data.profileMap, ectx, recent_name);

                // attempt to iterate over the ptr-reference chain
                if (ectx.spi != data.profileMap.end() && !ectx.spi->second.back().ignorePtrRef && ectx.dereferenced) {
                    std::vector<std::string> visited; // ensure we do not enter circular dependence
                    std::string referenceName = ectx.spi->second.back().currentPointerReference;
                    SliceProfileIterator prevAspi = data.profileMap.end();
                    SliceProfileIterator aspi = data.profileMap.find(referenceName);
                    
                    for (int i = 0; i < ectx.dLength; ++i) {
                        if (aspi == data.profileMap.end()) break;
                        if (std::find(visited.begin(), visited.end(), referenceName) != visited.end()) break;

                        // ensure all LHS are given their dvar/alias data
                        
                        // pointer is dereferenced on the rhs of a redefinition
                        // meaning the lhs is data dependent of SOME of the reference chain
                        ExprParse::updateLHS(data.profileMap, ectx, referenceName, i, ectx.dLength);

                        ExprParse::pushUse(data.profileMap, aspi, ectx.namePos);

                        // handle pre/post fix incr/decr
                        if (ectx.prefixed || (expr_op == "++" || expr_op == "--")) {
                            if (i+1 == ectx.dLength) ExprParse::pushDef(data.profileMap, aspi, ectx.namePos);
                        }

                        visited.push_back(referenceName);

                        referenceName = aspi->second.back().currentPointerReference;
                        prevAspi = aspi;
                        aspi = data.profileMap.find(referenceName);
                    }

                    // apply the definition to the end of the chain
                    if (ectx.cppInput) {
                        ExprParse::pushDef(data.profileMap, (aspi == data.profileMap.end()) ? prevAspi : aspi, ectx.namePos);
                        ExprParse::popUse(data.profileMap, (aspi == data.profileMap.end()) ? prevAspi : aspi, ectx.namePos);
                    }
                }
                
                ExprParse::pushUse(data.profileMap, ectx.spi, ectx.namePos);

                if (ectx.cppInput && !ectx.dereferenced) {
                    ExprParse::pushDef(data.profileMap, ectx.spi, ectx.namePos);
                    ExprParse::popUse(data.profileMap, ectx.spi, ectx.namePos);
                }
                
                // handle pre/post fix incr/decr
                if (ectx.prefixed || (expr_op == "++" || expr_op == "--")) {
                    if (!ectx.dereferenced) ExprParse::pushDef(data.profileMap, ectx.spi, ectx.namePos);
                    ectx.prefixed = false;
                }
                
                ectx.lastToken.type = ExprParse::TokenType::NAME;
            } catch (std::logic_error& e) {
                if (data.verboseMode) std::cout << "[-] Caught Logic Error: " << e.what() << std::endl;
            }

            // Collect cfunc data
            if (isArg) {
                try {
                    std::string functionName = funcCallData->name.ToString();

                    // Update an existing slices Call data
                    SlicePosition argumentPosition(ectx.namePos);
                    SlicePosition invokePosition(
                        funcCallData->startPosition,
                        funcCallData->endPosition,
                        sctx.currentFilePath
                    );

                    auto sliceProfileItr = data.profileMap.find(recent_name);
                    if (sliceProfileItr != data.profileMap.end()) {
                        // variable is used within a function call, even if a signature or fingerprint
                        // cannot be located

                        // Need to also potentially add definition line numbers incase there are
                        // increment or decrement operators with the argument expression
                        sliceProfileItr->second.back().uses.insert(argumentPosition);

                        std::string simpleFunctionName = SrcSliceOperations::GetSimpleFunctionName(functionName);
                        std::string potentialContainingClass = "";

                        // function accessed from class
                        if ((functionName.find_last_of(".") != std::string::npos)) {
                            std::string callSrc = functionName.substr(0, functionName.find_last_of("."));
                            auto spi = data.profileMap.find(callSrc);
                            if (spi != data.profileMap.end()) {
                                // types are classes
                                potentialContainingClass = spi->second.back().variableType;
                            }
                        } else {
                            // function calls can be invoked via myObj.foo() or ptr->bar()
                            if ((functionName.find_last_of("->") != std::string::npos)) {
                                std::string callSrc = functionName.substr(0, functionName.find_last_of("."));
                                auto spi = data.profileMap.find(callSrc);
                                if (spi != data.profileMap.end()) {
                                    // strip the * char(s) from the type
                                    std::string ptrType = spi->second.back().variableType;
                                    
                                    potentialContainingClass = ptrType.substr(0, ptrType.find_first_of("*"));
                                }
                            }
                        }

                        // Get the collection of functions by name
                        auto funcSig = data.functionSigMap.find(simpleFunctionName);
                        if (funcSig != data.functionSigMap.end()) {
                            // if there is only one record of a function signature
                            if (funcSig->second.size() == 1) {
                                SlicePosition funcPos = funcSig->second[0].position;
                                CreateSliceCallData(data, sctx, simpleFunctionName, argIndex, funcPos, sliceProfileItr->second.back(), invokePosition);
                            } else {
                                // if a function is overloaded
                                size_t sigIndex = 0;

                                // If we have a signature with predefined parameters and another signature where the data-type
                                // of the parameter differs, we need to check argc <= paramc AND dataType(arg[i]) == dataType(param[i])
                                // we will have to derive the arg[i] to its corresponding slice, param[i] is a decldata so we can fetch its type
                                for (sigIndex; sigIndex < funcSig->second.size(); ++sigIndex) {
                                    bool argumentInBounds = (argIndex-1 < funcSig->second[sigIndex].parameters.size());
                                    if (!argumentInBounds) continue;

                                    bool validArgCount = (funcCallData->arguments.size() <= funcSig->second[sigIndex].parameters.size());
                                    if (!validArgCount) continue;

                                    if (!potentialContainingClass.empty()) {
                                        // check if the call data has the matching class name
                                        bool matchingClass = (funcSig->second[sigIndex].containingClass == potentialContainingClass);
                                        if (!matchingClass) continue;
                                    }

                                    std::string sliceDataType = sliceProfileItr->second.back().variableType;

                                    // Make a function to pull data-type
                                    std::string paramDataType = funcSig->second[sigIndex].parameters[argIndex-1]->type.ToString();
                                    // remove all spaces from type string
                                    paramDataType.erase(std::remove(paramDataType.begin(), paramDataType.end(), ' '), paramDataType.end());

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
                                    if (data.verboseMode) {
                                        std::cout << "[-] " << __LINE__  << " | Parameter Filtered-Type -> "
                                            << filteredParamDataType << " | Argument Filtered-Type -> " << filteredSliceDataType << std::endl;
                                    }
                                    if (!matchingTypes) continue;

                                    // potentially valid function finger-print
                                    break;
                                }

                                if (sigIndex < funcSig->second.size()) {
                                    SlicePosition funcPos = funcSig->second[sigIndex].position;
                                    CreateSliceCallData(data, sctx, simpleFunctionName, argIndex, funcPos, sliceProfileItr->second.back(), invokePosition);
                                } else {
                                    if (data.verboseMode)
                                        std::cout << "[-] " << __LINE__  << " | Fingerprint Not Found for -> " << simpleFunctionName << std::endl;
                                    CreateSliceCallData(data, sctx, simpleFunctionName, argIndex, SlicePosition(), sliceProfileItr->second.back(), invokePosition);
                                }
                            }
                        } else {
                            if (data.verboseMode)
                                std::cout << "[-] " << __LINE__  << " | No Function Signature Found for -> " << simpleFunctionName << std::endl;
                            CreateSliceCallData(data, sctx, simpleFunctionName, argIndex, SlicePosition(), sliceProfileItr->second.back(), invokePosition);
                        }
                    }
                }  catch (std::logic_error& e) {
                    if (data.verboseMode) std::cout << "[-] " << __LINE__  << " | Caught Logic Error: " << e.what() << std::endl;
                }
            }
        } else if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::OperatorData>)) {
            std::shared_ptr<srcDispatch::OperatorData> opData = std::any_cast<std::shared_ptr<srcDispatch::OperatorData>>(exprElem.GetElement());
            std::string current_op_token = opData->op.ToString();

            expr_op = (current_op_token == "(" || current_op_token == ")") ? expr_op : current_op_token;

            ectx.lastToken.prevToken = ectx.lastToken.token;
            
            // used to ensure if we just set dereferenced to true we do not instantly reset it
            bool reset = true;

            if (current_op_token != "(") {
                if (SrcSliceOperations::isAssignment(expr_op)) {
                    std::string name = (ectx.spi == data.profileMap.end()) ? "" : ectx.spi->second.back().currentPointerReference;
                    auto aspi = data.profileMap.find(name);

                    if (!ectx.dereferenced) {
                        // redefining a normal variable
                        ExprParse::pushDef(data.profileMap, ectx.spi, ectx.namePos);
                        
                        ExprParse::popUse(data.profileMap, ectx.spi, ectx.namePos);
                        // if spi is a pointer then when we redefine a pointer's reference
                        // we are not using the current reference
                        ExprParse::popUse(data.profileMap, aspi, ectx.namePos);

                        lhsStack.push_back(recent_name);
                    } else {
                        // redefining a pointer's reference
                        ExprParse::pushDef(data.profileMap, aspi, ectx.namePos);
                        ExprParse::popUse(data.profileMap, aspi, ectx.namePos);

                        if (aspi != data.profileMap.end())
                            lhsStack.push_back(aspi->second.back().variableName);
                    }
    
                    
                    if (SrcSliceOperations::isCompoundAssignment(expr_op)) {
                        // recent_name (new lhs) is used and redefined
                        ExprParse::pushUse(data.profileMap, ectx.spi, ectx.namePos);
    
                        if (ectx.dereferenced) {
                            // redefining a pointer's reference
                            ExprParse::pushUse(data.profileMap, aspi, ectx.namePos);
                        }
                    }
                } else if (current_op_token == "<<" || current_op_token == ">>") {
                    if (sctx.currentFileLanguage == "C++") {
                        if (current_op_token == "<<") {
                            if (ExprParse::IsIO(sctx, ectx.firstNameData) == 0 || ExprParse::IsIO(sctx, ectx.firstNameData) == 1) {
                                ectx.cppOutput = true;
                            }
                        } else if (current_op_token == ">>") {
                            if (ExprParse::IsIO(sctx, ectx.firstNameData) == 2) {
                                ectx.cppInput = true;
                            }
                        }
                    }
                } else if ((ectx.lastToken.type != ExprParse::TokenType::NAME && ectx.lastToken.prevToken != ")") || current_op_token == ")") {
                    // a & b is a bitwise operation
                    if (expr_op == "&") {
                        ectx.addressOf = true;
                    } else if (expr_op == "*") {
                        reset = false;
                        ectx.dereferenced = true;
                        
                        if (current_op_token != ")") {
                            ++ectx.dLength;
                        }
                    } else if (expr_op == "++" || expr_op == "--") {
                        ectx.prefixed = true;
                    }
                } else if (expr_op == "++" || expr_op == "--") {
                    // postfixing
                    if (ectx.lastToken.type == ExprParse::TokenType::NAME || ectx.lastToken.prevToken == ")") {
                        if (ectx.dereferenced) {
                            // redefining a pointer's reference
                            std::string name = (ectx.spi == data.profileMap.end()) ? "" : ectx.spi->second.back().currentPointerReference;
                            auto aspi = data.profileMap.find(name);
                            
                            ExprParse::pushDef(data.profileMap, aspi, ectx.namePos);
                            ExprParse::pushUse(data.profileMap, aspi, ectx.namePos);
                        } else {
                            // normal variable
                            ExprParse::pushDef(data.profileMap, ectx.spi, ectx.namePos);
                            ExprParse::pushUse(data.profileMap, ectx.spi, ectx.namePos);
                        }
                    }
                }
            }

            ectx.lastToken.type = ExprParse::TokenType::OPERATOR;
            ectx.lastToken.token = current_op_token;
            if (reset) ectx.resetDereferenceCtx();
        } else if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::CallData>)) {
            // This will read through the Call Args and attempt
            // to find the slice profile for the extracted arg
            // and will attempt to insert potential:
            // use/def/call data
            std::shared_ptr<srcDispatch::CallData> callData = std::any_cast<std::shared_ptr<srcDispatch::CallData>>(exprElem.GetElement());

            if (data.verboseMode) {
                std::cout << "[*] " << __LINE__  << " | Parsing For Targets: " << callData->name.ToString() << std::endl;
            }

            // find root name if needed
            for (const auto& nameElem : callData->name->names) {
                if (nameElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
                    std::shared_ptr<srcDispatch::NameData> nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(nameElem.GetElement());
                    if (nameData) {
                        std::string callTarget = nameData->name.ToString();
                        SlicePosition namePos(
                            nameData->startPosition,
                            nameData->endPosition,
                            sctx.currentFilePath
                        );
    
                        auto spi = data.profileMap.find(callTarget);
                        ExprParse::pushUse(data.profileMap, spi, namePos);
                    }
                    // terminate after reviewing first NameData
                    break;
                }
            }

            for (const ExprInfo& arg : callData->arguments) {
                // if there is nested calls the argIndex needs to be initially passed as 1
                // at each instance of parsing CallData
                int v = (isArg) ? 1 : ++argIndex;

                // parse the call-argument expression
                ParseExpr(data, sctx, arg, expr_type, lhsStack, true, &(*callData), v);
            }
        } else if (exprElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::LiteralData>)) {
            ectx.lastToken.type = ExprParse::TokenType::LITERAL;
        }
    } // end of looping elements
}

void SrcSliceOperations::ProcessFunctionParameters(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::vector<DeclInfo>& parameters,
                                                std::string currentFunctionName, std::string className) {
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
        paramType = GetTypeDetails(parameter.GetElement(), isPointer, isReference, isArray);
        // remove all spaces from type string
        paramType.erase(std::remove(paramType.begin(), paramType.end(), ' '), paramType.end());

        SlicePosition paramPos(parameter->name->startPosition, parameter->name->endPosition, sctx.currentFilePath);

        // Record parameter data-- this is done exact as it is done for decl_stmts except there's no initializer
        auto sliceProfileItr = data.profileMap.find(paramName);

        SliceProfile sliceProf = SliceProfile(paramName, paramPos,
                                        isPointer, true,
                                        {paramPos});

        sliceProf.containsDeclaration = true;
        sliceProf.nameOfContainingClass = className;
        sliceProf.containingNameSpaces = funcData->namespaces;
        sliceProf.language = sctx.currentFileLanguage;
        
        sliceProf.showControlEdges = data.calculateControlEdges;

        sliceProf.isPointer = isPointer;
        sliceProf.isReference = isReference;
        sliceProf.isPotentialArray = isArray;

        // Just add new slice profile if name already exists. Otherwise, add new entry in map.
        if (sliceProfileItr != data.profileMap.end()) {
            sliceProfileItr->second.push_back(std::move(sliceProf));
        } else {
            data.profileMap.insert(std::make_pair(paramName, std::vector<SliceProfile>{ std::move(sliceProf) }));
        }

        // Attempt to insert data-types for sliceprofiles found in function/ctor parameters
        data.profileMap.find(paramName)->second.back().variableType = isArray ? SrcSliceOperations::GenerateArrayType(paramType, parameter->name->indices.size()) : paramType;

        // Link the filepath this slice is located in
        data.profileMap.find(paramName)->second.back().file = sctx.currentFilePath;

        // Link the file hash attribute
        data.profileMap.find(paramName)->second.back().checksum = sctx.currentFileChecksum;

        // Link the function the XML Originates from
        data.profileMap.find(paramName)->second.back().function = currentFunctionName;
    }
}

void SrcSliceOperations::ProcessFunctionSignature(Blob& data, const SliceCtx& sctx, FunctionInfo& funcData, std::string className) {
    std::string functionName = funcData->name.ToString();
    if (functionName.empty()) return;

    bool updateSignature = (functionName.find("::") != std::string::npos);

    // Update a Signature Entry due to out-of-line definition
    if (updateSignature) {
        functionName = functionName.substr(functionName.find_last_of("::")+1, -1);
    }

    // Process the parameters in a separate function
    auto& functionParameters = funcData->parameters;
    ProcessFunctionParameters(data, sctx, funcData, functionParameters, funcData->name.ToString(), className);
    auto funcSig = data.functionSigMap.find(functionName);

    if (funcSig != data.functionSigMap.end()) {
        if (updateSignature) {
            for (auto& func : funcSig->second) {
                if (func.parameters.size() == functionParameters.size()) {
                    // Update marked signature
                    func = FunctionSignatureData(funcData, className, sctx);
                    break;
                }
            }
        } else {
            // overloaded function detected
            funcSig->second.push_back({funcData, className, sctx});
        }
    } else {
        // Insert a new signature
        data.functionSigMap[functionName].push_back({funcData, className, sctx});
    }
}

// Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
// Logic constructed for use BEFORE InterProcedural
SliceProfile* SrcSliceOperations::FetchSliceProfile(Blob& data, const SliceCtx& sctx, std::string profileName, const FunctionInfo& funcData,
                                std::string className, std::vector<std::string> containingNameSpaces) {
    auto spi = data.profileMap.find(profileName);
    SliceProfile* probableProfile = nullptr;

    if (spi != data.profileMap.end()) {
        // iterate the SliceProfile Vector and perform comparisons
        // When performing comparisons we prioritize local-scope variables over globals
        for (auto& profile : spi->second) {
            if (!profile.containsDeclaration) continue;

            // check if we marked a global, process profile if we are searching for a global
            // globals do not hold data about source function/class/namespace
            if (profile.function.empty() && profile.nameOfContainingClass.empty() && profile.containingNameSpaces.empty()) {
                probableProfile = &profile;
                continue;
            }
            
            if (profile.classMemberVar) {
                probableProfile = &profile;
                continue;
            }

            // perform standard finger printing checks
            if (funcData) {
                if (funcData->name && funcData->name.GetElement()) {
                    bool matchingFunction = profile.function == funcData->name.ToString();
                    if (!matchingFunction) continue;
                }
            }

            if (profile.nameOfContainingClass != className) continue;
            if (profile.containingNameSpaces != containingNameSpaces) continue;

            probableProfile = &profile;
        }
        
        return probableProfile;
    }
    return nullptr;
}