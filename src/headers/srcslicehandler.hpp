#ifndef SRCSLICEHANDLER
#define SRCSLICEHANDLER

#include <srcsliceprofile.hpp>
#include <srcsliceprogress.hpp>
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
    SrcSliceHandler(const char* filename, bool v, bool p, bool ce);

    // Use string srcml buffer ctor of srcSAXController
    SrcSliceHandler(std::string& sourceCodeStr, bool ce);

    void Notify(const srcDispatch::PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) override;

    void NotifyWrite(const srcDispatch::PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {};

    // Creates Initial SliceProfiles based off a list of decl statements
    void ProcessDecls(std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclStmtData>>>& declStmts, const SliceCtx& ctx);
    // Creates Initial SliceProfiles for Function Parameters and Variables Declared within the function definition
    void ProcessFunctions(std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>>& funcs, const SliceCtx& ctx);
    // Process Class Data and create slices of Class Member Variables and process Member Functions
    void ProcessClasses(std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ClassData>>>& classes, const SliceCtx& ctx);
    // Process Signatures from Free-Functions and Class Methods
    void ProcessSignatures(std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>>& funcs,
                            std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ClassData>>>& classes,
                            const SliceCtx& ctx);

    // Creates Initial SliceProfiles for Variables Declared within a specified Block within a Function Definition
    void ProcessDeclStmts(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::BlockData>>& block, std::string className, const SliceCtx& ctx);
    // Creates Initial SliceProfile based off DeclData
    void CreateSliceProfile(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>& deltaDeclData, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::string className, const SliceCtx& ctx);
    // Process Constructor Initializer Lists establishing connection between Class Members and Ctor Parameters
    void ProcessInitLists(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::string className, const SliceCtx& ctx);
    // Extract Expressions within a specified Block within a Function Definition
    void ProcessExprStmts(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::BlockData>>& block, std::string className, const SliceCtx& ctx);
    // Capture SliceProfile Data from a given Expression within a specified Block within a Function Definition
    void ProcessExprStmt(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ExpressionData>>& expr, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::string className, const SliceCtx& ctx);
    // Update Slice Profiles based off Collected Variable Data post ParseExpr
    void UpdateSlices(std::vector<std::shared_ptr<VariableData>>& varDataGroup, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData,
                        std::string className, const SliceCtx& ctx);
    // Parse a given Expression and return a Collection of Variable Data used to Update SliceProfiles
    std::vector<std::shared_ptr<VariableData>>& ParseExpr(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ExpressionData>>& expr, const unsigned int& lineNumber);
    // Get Type Details (isPtr, isRef, isArr, etc) based of a given DeclData
    std::string GetTypeDetails(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>& localVar, bool& isPointer, bool& isReference, bool& isArray);
    // Try-Blocks contain both exprs and decls, need to extract those decls and create slice profiles
    // for them, along with capturing expressions to update collected slices
    void CollectTryBlockData(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::vector<std::shared_ptr<srcDispatch::TryData>>& tryBlocks,
                                std::string className, const SliceCtx& ctx);
    void CollectConditionalData(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ExpressionData>>>* exprStmts, std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>>* declStmts,
                                std::vector<std::any>& conditionals, const SliceCtx& ctx);
    // Given a list of Function Parameters create Initial SliceProfiles for each Parameter
    void ProcessFunctionParameters(const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>>& parameters,
                                    std::string currentFunctionName, std::string className, const SliceCtx& ctx);
    // Create a Function Signature based off given Function Data
    void ProcessFunctionSignature(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData, std::string className, const SliceCtx& ctx);


    // Use collected function call data to push a new cfunctions entry into a referenced slice profile
    void CreateSliceCallData(std::string functionName, int argIndex, int functionDefLine, SliceProfile& sliceProfile, unsigned int functionCallLine);

    void ProcessFunctionCall(std::shared_ptr<srcDispatch::CallData>& funcCallData);

    // Take large name strings and extract the root variable name
    std::string ExtractName(std::string elementName);

    bool IsPointerDereferenced(std::shared_ptr<srcDispatch::NameData>& varNameElem);

    // Attempt to Recursively dig into potential nested indices in a RHS to form dependency relations with a LHS variable
    void AppendIndices(std::shared_ptr<VariableData>& lhs, std::shared_ptr<VariableData>& varData);

    // Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
    // Logic constructed for use BEFORE InterProcedural
    SliceProfile* FetchSliceProfile(std::string profileName, std::shared_ptr<VariableData>& vd, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData,
                                    std::string className = "", std::vector<std::string> containingNameSpaces = {});

    // Extract the function name within either a call or a complex function name
    std::string GetSimpleFunctionName(std::string funcName);

    // Used for inserting Uses and Defs for Slices in the LHS of an Expression Statement
    // perform a map find and update the slice
    void UpdateLHSSlices(std::shared_ptr<VariableData>& varData);

    bool StringContainsCharacters(std::string &str);

    bool isAssignment(std::string& expr_op);

    bool isCompoundAssignment(std::string& expr_op);

    bool isLogical(std::string& expr_op);

    bool isWhiteSpace(std::string& str);

    std::unordered_map<std::string, std::vector<SliceProfile>>& GetProfileMap();

    // Component of function FindOtherPaths
    void ComputeOuterPaths(std::set<std::pair<int,int>>& otherPaths, std::vector<int>& sLines);

    // Component of function FindOtherPaths
    void ComputeExitPaths(std::set<std::pair<int,int>>& otherPaths, std::vector<int>& sLines, std::set<int>& ignoreLines);

    // Attempt to find other Forward Control-Flow paths | ComputeControlPaths Helper Function
    std::set<std::pair<int,int>> FindOtherPaths(std::vector<int>& sLines, std::set<int>& ignoreLines);

    // srcSlice focuses on Forward-Slicing, therefor our Control-Flows are going to be forward-flowing
    // we are not focusing on backwards-flows.
    void ComputeControlPaths();

    auto ArgumentProfile(std::pair<std::string, FunctionSignatureData> func, int paramIndex, std::unordered_set<std::string>& visit_func);

    // Need to track Aliases we have already read through
    // InterProcedural from the normal call should also be reflected
    // if a profile with alias(s) is used in function calls
    void ComputeAliasInterprocedural();
    void ComputeInterprocedural();

private:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> forloopdata;
    std::vector<std::pair<int, int>> whileloopdata;
    std::vector<std::pair<int, int>> dowhileloopdata;

    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elseifdata;
    std::vector<std::pair<int, int>> elsedata;

    std::unordered_map<std::string, std::vector<FunctionSignatureData>> functionSigMap;
    bool verboseMode, calculateControlEdges, progressMode;
};

#endif