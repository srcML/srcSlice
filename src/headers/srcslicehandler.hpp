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
#include <FunctionPolicySingleEvent.hpp>
#include <ClassPolicySingleEvent.hpp>
#include <UnitPolicySingleEvent.hpp>
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
            std::shared_ptr<ClassData> class_data = policy->Data<ClassData>();
            classInfo.push_back( class_data );
        } else if(typeid(FunctionPolicy) == typeid(*policy)) {
            ProcessFunctionData(policy->Data<FunctionData>(), ctx);
        }
    }

    void NotifyWrite(const PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {}

    void ProcessFunctionData(std::shared_ptr<FunctionData> function_data, const srcDispatch::srcSAXEventContext& ctx) {
        ProcessFunctionSignature(function_data, ctx);
        ProcessDeclStmts(function_data->block->locals, ctx);
        ProcessExprStmts(function_data->block->expr_stmts, ctx);
    }

    void ProcessDeclStmts(std::vector<std::shared_ptr<DeclTypeData>>& decls, const srcDispatch::srcSAXEventContext& ctx) {
        // loop through all the expression statements within Decl Statements
        for (const auto& localVar : decls) {
            auto varDataGroup = ParseExpr(*localVar->initializer, localVar->initializer->lineNumber);

            // Collect pieces about the newly declared variable to use later when adding it into
            // our profileMap
            std::string declVarName = localVar->name->name;
            std::string declVarType = localVar->type->ToString();
            bool isPointer = false;
            bool isReference = false;

            for(std::size_t pos = 0; pos < localVar->type->types.size(); ++pos) {
                const std::pair<std::any, TypeData::TypeType> & type = localVar->type->types[pos];
                if (type.second == TypeData::POINTER) {
                    isPointer = true;
                } else if (type.second == TypeData::REFERENCE) {
                    isReference = true;
                } /* else if (type.second == TypeData::RVALUE) {
                } else if (type.second == TypeData::SPECIFIER) {
                } else if (type.second == TypeData::TYPENAME) {
                } */
            }

            auto sliceProfileItr = profileMap.find(declVarName);

            // Dumps out the variable names of variables
            // declared in a function body :: main(), ...

            //Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap.end()) {
                auto sliceProfile = SliceProfile(declVarName, localVar->lineNumber,
                                                 isPointer, true,
                                                 std::set<unsigned int>{localVar->lineNumber});

                sliceProfile.nameOfContainingClass = ctx.currentClassName;
                sliceProfile.containingNameSpaces = ctx.currentNamespaces;
                sliceProfile.language = ctx.currentFileLanguage;

                sliceProfileItr->second.push_back(sliceProfile);
                sliceProfileItr->second.back().containsDeclaration = true;
            } else {
                auto sliceProf = SliceProfile(declVarName, localVar->lineNumber,
                                              (isPointer), false,
                                              std::set<unsigned int>{localVar->lineNumber});

                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProf.language = ctx.currentFileLanguage;

                sliceProf.containsDeclaration = true;
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
            for (auto& varData : varDataGroup) {
                for (auto& dvarData : varData.rhsElems) {
                    std::string dvar = dvarData.GetNameOfIdentifier();

                    auto updateDvarAtThisLocation = profileMap.find(dvar);
                    if (updateDvarAtThisLocation != profileMap.end()) {
                        if (!StringContainsCharacters(declVarName)) continue;
                        if (sliceProfileItr != profileMap.end() && sliceProfileItr->second.back().potentialAlias) {
                            if ( declVarName != sliceProfileItr->second.back().variableName) {
                                updateDvarAtThisLocation->second.back().aliases.insert(std::make_pair(declVarName, ctx.currentLineNumber));
                            }
                            continue;
                        }
                        updateDvarAtThisLocation->second.back().dvars.insert(std::make_pair(declVarName, ctx.currentLineNumber));
                    } else {
                        auto sliceProf = SliceProfile(dvar, localVar->lineNumber, false, false, std::set<unsigned int>{},
                                                    std::set<unsigned int>{localVar->lineNumber});
                        sliceProf.nameOfContainingClass = ctx.currentClassName;
                        sliceProf.containingNameSpaces = ctx.currentNamespaces;
                        auto newSliceProfileFromDeclDvars = profileMap.insert(std::make_pair(dvar,
                                                                                            std::vector<SliceProfile>{
                                                                                                    std::move(sliceProf)
                                                                                            }));
                        if (!StringContainsCharacters(declVarName)) continue;
                        if (sliceProfileItr != profileMap.end() && sliceProfileItr->second.back().potentialAlias) {
                            if ( declVarName != sliceProfileItr->second.back().variableName ) {
                                newSliceProfileFromDeclDvars.first->second.back().aliases.insert(std::make_pair(declVarName, ctx.currentLineNumber));
                            }
                            continue;
                        }
                        newSliceProfileFromDeclDvars.first->second.back().dvars.insert(std::make_pair(declVarName, ctx.currentLineNumber));
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
            sliceProfileItr->second.back().function = ctx.currentFunctionName;
        }
    }

    void ProcessExprStmts(std::vector<std::shared_ptr<ExpressionData>>& exprStmts, const srcDispatch::srcSAXEventContext& ctx) {
        // loop through all the expression statements
        for (const auto& expr : exprStmts) {
            auto varDataGroup = ParseExpr(*expr, expr->lineNumber);

            for (auto& varData : varDataGroup) {
                for (auto& rhsVarData : varData.rhsElems) {
                    std::shared_ptr<ExpressionElement> lhsData = varData.lhsElem;
                    std::string lhsName = varData.GetNameOfIdentifier();
                    std::string rhsName = rhsVarData.GetNameOfIdentifier();

                    auto sliceProfileExprItr = profileMap.find(rhsName);
                    auto sliceProfileLHSItr = profileMap.find(lhsName);

                    //Just update definitions and uses if name already exists. Otherwise, add new name.
                    if (sliceProfileExprItr != profileMap.end()) {
                        sliceProfileExprItr->second.back().nameOfContainingClass = ctx.currentClassName;
                        sliceProfileExprItr->second.back().containingNameSpaces = ctx.currentNamespaces;
                        sliceProfileExprItr->second.back().uses.insert(rhsVarData.uses.begin(),
                                                                       rhsVarData.uses.end());
                        sliceProfileExprItr->second.back().definitions.insert(rhsVarData.definitions.begin(),
                                                                              rhsVarData.definitions.end());

                        if (!StringContainsCharacters(lhsName)) continue;
                        if (sliceProfileLHSItr != profileMap.end() && sliceProfileLHSItr->second.back().potentialAlias) {
                            if ( lhsName != sliceProfileExprItr->second.back().variableName ) {
                                sliceProfileExprItr->second.back().aliases.insert(std::make_pair(lhsName, ctx.currentLineNumber));
                            }
                            continue;
                        }
                        
                        // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                        // IE : abc = abc + i;
                        if (!StringContainsCharacters(lhsName)) continue;
                        if (!lhsName.empty() && sliceProfileExprItr->second.back().variableName != lhsName) {
                            sliceProfileExprItr->second.back().dvars.insert(std::make_pair(lhsName, ctx.currentLineNumber));
                            continue;
                        }

                    } else {
                        auto sliceProfileExprItr2 = profileMap.insert(std::make_pair(rhsName,
                                                                                        std::vector<SliceProfile>{
                                                                                                SliceProfile(
                                                                                                        rhsName,
                                                                                                        ctx.currentLineNumber,
                                                                                                        false, false,
                                                                                                        rhsVarData.definitions,
                                                                                                        rhsVarData.uses)
                                                                                        }));
                        sliceProfileExprItr2.first->second.back().nameOfContainingClass = ctx.currentClassName;
                        sliceProfileExprItr2.first->second.back().containingNameSpaces = ctx.currentNamespaces;

                        if (!StringContainsCharacters(lhsName)) continue;
                        if (sliceProfileLHSItr != profileMap.end() && sliceProfileLHSItr->second.back().potentialAlias) {
                            if ( lhsName != sliceProfileLHSItr->second.back().variableName ) {
                                sliceProfileExprItr2.first->second.back().aliases.insert(std::make_pair(lhsName, ctx.currentLineNumber));
                            }
                            continue;
                        }

                        // Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                        // IE : abc = abc + i;
                        if (!StringContainsCharacters(lhsName)) continue;
                        if (!lhsName.empty() && (lhsName != rhsName)) {
                            sliceProfileExprItr2.first->second.back().dvars.insert(std::make_pair(lhsName, ctx.currentLineNumber));
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

    std::vector<VariableData> ParseExpr(const ExpressionData& expr, const unsigned int& lineNumber) {
        /*
            - Need to implement logic to capture pre/postfix assignment occurences to a Vars Def set
            - Need also a way to seperate processing pre/postfix operators from assignment & compound assignment
        */

        std::vector<VariableData> varDataGroup;
        std::string expr_op = "";
        VariableData exprVariable; // LHS variable

        // loop through each element within a specific expression statement
        for (const auto& exprElem : expr.expr) {
            switch (exprElem->type) {
                case ExpressionElement::NAME: // 0
                    if (expr_op.empty()) {
                        exprVariable.lhsElem = exprElem;
                        exprVariable.uses.insert(lineNumber);
                    } else {
                        exprVariable.rhsElems.push_back( VariableData(exprElem) );
                        exprVariable.rhsElems.back().uses.insert(lineNumber);
                    }
                break;
                case ExpressionElement::OP: // 1
                    // Dont set the operator to a whitespace
                    if (!isWhiteSpace(exprElem->token->token)) {
                        expr_op = exprElem->token->token;

                        // Print the current LHS and RHS pair
                        if (!exprVariable.rhsElems.empty() && isAssignment(expr_op)) {
                            // Push back the lhs-rhs set
                            exprVariable.lhs = true;
                            varDataGroup.push_back(exprVariable);
                            exprVariable.clear();
                            // Set the new lhs for the potential next lhs-rhs pair
                            exprVariable = exprVariable.rhsElems.back();
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

        // Ensure we collect the final expr variable in the expr_stmt
        if (exprVariable.lhsElem != nullptr) {
            if (exprVariable.rhsElems.size() > 0) {
                exprVariable.lhs = true;
            }
            varDataGroup.push_back(exprVariable);
        }

        if (varDataGroup.size() > 1) {
            // Insert the vector collection from vectors
            // in-front of a specific vector to show all
            // RHS vars used against a LHS var
            for (auto pItr = varDataGroup.begin(); pItr != varDataGroup.end(); ++pItr) {
                auto* v = &(pItr->rhsElems);
                auto nextItr = std::next(pItr);
                if (nextItr != varDataGroup.end()) {
                    auto* v2 = &(nextItr->rhsElems);
                    v->insert(v->end(), v2->begin(), v2->end());
                }
            }
        }

        return varDataGroup;
    }

    void ProcessFunctionParameters(std::vector<std::shared_ptr<ParamTypeData>>& parameters, const srcDispatch::srcSAXEventContext& ctx) {
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
            }
            
            // Record parameter data-- this is done exact as it is done for decl_stmts except there's no initializer
            auto sliceProfileItr = profileMap.find(paramName);
            // Just add new slice profile if name already exists. Otherwise, add new entry in map.
            if (sliceProfileItr != profileMap.end()) {
                auto sliceProf = SliceProfile(paramName, parameter->lineNumber,
                                              isPointer, true,
                                              std::set<unsigned int>{parameter->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProf.containingNameSpaces = ctx.currentNamespaces;
                sliceProfileItr->second.push_back(std::move(sliceProf));
            } else {
                auto sliceProf = SliceProfile(paramName, parameter->lineNumber,
                                              isPointer, true,
                                              std::set<unsigned int>{parameter->lineNumber});
                sliceProf.containsDeclaration = true;
                sliceProf.nameOfContainingClass = ctx.currentClassName;
                sliceProf.containingNameSpaces = ctx.currentNamespaces;
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
            profileMap.find(paramName)->second.back().function = ctx.currentFunctionName;
        }
    }

    void ProcessFunctionSignature(std::shared_ptr<FunctionData> funcData, const srcDispatch::srcSAXEventContext& ctx) {
        std::string functionName = funcData->name->ToString();
        if (functionName.empty()) return;

        // Process the parameters in a separate function
        ProcessFunctionParameters(funcData->parameters, ctx);

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

    bool isWhiteSpace(const std::string& str) {
        return str.find_first_not_of(" \t\n\r") == std::string::npos;
    }

    std::unordered_map<std::string, std::vector<SliceProfile>>& GetProfileMap() {
        return profileMap;
    }
    
    // auto ArgumentProfile(std::pair<std::string, SignatureData> func, int paramIndex, std::unordered_set<std::string> visit_func) {
	//     auto Spi = profileMap.find(func.second.parameters.at(paramIndex).nameOfIdentifier);

    //     // Ensure the key exists in the map
    //     std::string functionName = func.first;
    //     if (sliceEventData->funcDefMap.find(functionName) == sliceEventData->funcDefMap.end()) {
    //         sliceEventData->funcDefMap[functionName] = std::vector<unsigned int>();
    //     }

    //     // Check for Duplicates before pushing a line number
    //     auto& lineVector = sliceEventData->funcDefMap[functionName];
    //     if (std::find(lineVector.begin(), lineVector.end(), func.second.lineNumber) == lineVector.end()) {
    //         lineVector.push_back(func.second.lineNumber);
    //         // std::cout << functionName << " | Line Num " << func.second.lineNumber << std::endl;
    //     }

    //     for (auto param : func.second.parameters) {
    //         if (profileMap.find(param.nameOfIdentifier)->second.back().visited) {
    //             return Spi;
    //         } else {
    //             for (auto cfunc : profileMap.find(param.nameOfIdentifier)->second.back().cfunctions) {
    //                 if (cfunc.first.compare(func.first) != 0) {
    //                     auto function = sliceEventData->functionSigMap.find(cfunc.first);
    //                     if (function != sliceEventData->functionSigMap.end()) {
    //                         if (cfunc.first.compare(function->first) == 0 && visit_func.find(cfunc.first) == visit_func.end()) {
	// 			                visit_func.insert(cfunc.first);
    //                             auto recursiveSpi = ArgumentProfile(*function, std::atoi(cfunc.second.first.c_str()) - 1, visit_func);
    //                             if (profileMap.find(param.nameOfIdentifier) != profileMap.end() &&
    //                                 profileMap.find(recursiveSpi->first) != profileMap.end()) {
    //                                 profileMap.find(param.nameOfIdentifier)->second.back().definitions.insert(
    //                                         recursiveSpi->second.back().definitions.begin(),
    //                                         recursiveSpi->second.back().definitions.end());
    //                                 profileMap.find(param.nameOfIdentifier)->second.back().uses.insert(
    //                                         recursiveSpi->second.back().uses.begin(),
    //                                         recursiveSpi->second.back().uses.end());
    //                                 profileMap.find(param.nameOfIdentifier)->second.back().cfunctions.insert(
    //                                         profileMap.find(
    //                                                 param.nameOfIdentifier)->second.back().cfunctions.begin(),
    //                                         recursiveSpi->second.back().cfunctions.begin(),
    //                                         recursiveSpi->second.back().cfunctions.end());
    //                                 profileMap.find(param.nameOfIdentifier)->second.back().aliases.insert(
    //                                         recursiveSpi->second.back().aliases.begin(),
    //                                         recursiveSpi->second.back().aliases.end());
    //                                 profileMap.find(param.nameOfIdentifier)->second.back().dvars.insert(
    //                                         recursiveSpi->second.back().dvars.begin(),
    //                                         recursiveSpi->second.back().dvars.end());
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //             profileMap.find(param.nameOfIdentifier)->second.back().visited = true;
    //         }
    //     }
    //     return Spi;
    // }

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

    // void ComputeInterprocedural() {
	//     std::unordered_set <std::string> visited_func;
	//     for (std::pair<std::string, std::vector<SliceProfile>> var : profileMap) {
    //         // Need to watch the Slices we attempt to dig into because we are collecting slices we have no interest in
    //         if (!profileMap.find(var.first)->second.back().visited && (var.second.back().variableName != "*LITERAL*")) {
    //             if (!var.second.back().cfunctions.empty()) {
    //                 for (auto cfunc : var.second.back().cfunctions) {
    //                     auto funcIt = sliceEventData->functionSigMap.find(cfunc.first);
    //                     if(funcIt != sliceEventData->functionSigMap.end()) {
    //                         if (cfunc.first.compare(funcIt->first) == 0) { //TODO fix for case: Overload
    //                             auto Spi = ArgumentProfile(*funcIt, std::atoi(cfunc.second.first.c_str()) - 1, visited_func);
    //                             auto sliceItr = Spi->second.begin();
    //                             std::string desiredVariableName = sliceItr->variableName;

    //                             for (sliceItr = Spi->second.begin(); sliceItr != Spi->second.end(); ++sliceItr) {
    //                                 if (sliceItr->containsDeclaration) {
    //                                     if (sliceItr->variableName != desiredVariableName) {
    //                                         continue;
    //                                     }
    //                                     if (sliceItr->function != cfunc.first.substr(0, cfunc.first.find('-'))) {
    //                                         continue;
    //                                     }
    //                                     if (sliceItr->lineNumber != std::stoi(cfunc.second.second)) {
    //                                         continue;
    //                                     }

    //                                     break;
    //                                 }
    //                             }

    //                             if (profileMap.find(var.first) != profileMap.end() && profileMap.find(Spi->first) != profileMap.end() && sliceItr != Spi->second.end()) {
    //                                 if (!sliceItr->isReference && !sliceItr->isPointer) {
    //                                     // pass by value
    //                                     profileMap.find(var.first)->second.back().uses.insert(
    //                                             sliceItr->definitions.begin(),
    //                                             sliceItr->definitions.end());
    //                                 } else
    //                                 {
    //                                     // pass by reference
    //                                     profileMap.find(var.first)->second.back().definitions.insert(
    //                                             sliceItr->definitions.begin(),
    //                                             sliceItr->definitions.end());
    //                                 }

    //                                 profileMap.find(var.first)->second.back().uses.insert(
    //                                         sliceItr->uses.begin(),
    //                                         sliceItr->uses.end());

    //                                 // By converting the cfunctions vector to a set, allows us to remove
    //                                 // duplicate entries, once those are removed we can convert this cleaned
    //                                 // set back into its vector form
    //                                 profileMap.find(var.first)->second.back().cfunctions.insert(
    //                                         profileMap.find(var.first)->second.back().cfunctions.begin(),
    //                                         sliceItr->cfunctions.begin(),
    //                                         sliceItr->cfunctions.end());
    //                                 auto oldCalledFunctions = profileMap.find(var.first)->second.back().cfunctions;
    //                                 std::set<std::pair<std::string, std::pair<std::string, std::string>>> calledFunctionSet(oldCalledFunctions.begin(), oldCalledFunctions.end());
    //                                 profileMap.find(var.first)->second.back().cfunctions = std::vector<std::pair<std::string, std::pair<std::string, std::string>>>(calledFunctionSet.begin(), calledFunctionSet.end());

    //                                 profileMap.find(var.first)->second.back().aliases.insert(
    //                                         sliceItr->aliases.begin(),
    //                                         sliceItr->aliases.end());
    //                                 profileMap.find(var.first)->second.back().dvars.insert(
    //                                         sliceItr->dvars.begin(),
    //                                         sliceItr->dvars.end());
    //                             } else {
    //                                 std::cout << std::boolalpha << (profileMap.find(var.first) != profileMap.end()) << " && " << (profileMap.find(Spi->first) != profileMap.end()) << " && " << (sliceItr != Spi->second.end()) << std::endl;
    //                                 std::cout << "Tried Accessing Slice Variable :: " << var.first << std::endl;
    //                                 std::cout << "[-] An Error has Occured in `ComputeInterprocedural`" << std::endl;
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //             profileMap.find(var.first)->second.back().visited = true;
    //         }
    //     }
    // }

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
    FunctionSignatureData funcSigCollection;

    // Process the class and function information collected
    void PrintCollection() {
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
        std::cout << std::endl;
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