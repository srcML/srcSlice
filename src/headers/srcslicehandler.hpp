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

// type aliasing
typedef std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclStmtData>>> DeclStmts;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>> DeclInfo;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ExpressionData>> ExprInfo;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> FunctionInfo;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::BlockData>> BlockInfo;
typedef std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>> Functions;
typedef std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ClassData>>> Classes;

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
    void ProcessDecls(DeclStmts& declStmts, std::string className = "");
    // Creates Initial SliceProfiles for Function Parameters and Variables Declared within the function definition
    void ProcessFunctions(Functions& funcs);
    // Process Class Data and create slices of Class Member Variables and process Member Functions
    void ProcessClasses(Classes& classes);
    // Process Signatures from Free-Functions and Class Methods
    void ProcessSignatures(Functions& funcs, Classes& classes);

    // Moves over the sequence of statements and processes them in order
    // |__ Creates Initial SliceProfiles for Variables Declared within a specified Block within a Function Definition
    // |__ Extract Expressions within a specified Block within a Function Definition
    void ProcessStmts(const FunctionInfo& funcData, const BlockInfo& block, std::string className);
    
    // Creates Initial SliceProfile based off DeclData
    void CreateSliceProfile(const DeclInfo& deltaDeclData, const FunctionInfo& funcData, std::string className);
    // Process Constructor Initializer Lists establishing connection between Class Members and Ctor Parameters
    void ProcessInitLists(const FunctionInfo& funcData, std::string className);
    
    // Extract Expressions within a specified Block within a Function Definition
    void ProcessExprStmts(const FunctionInfo& funcData, const BlockInfo& block, std::string className);
    
    // Capture SliceProfile Data from a given Expression within a specified Block within a Function Definition
    void ProcessExprStmt(const ExprInfo& expr, const FunctionInfo& funcData, std::string className);
    // Update Slice Profiles based off Collected Variable Data post ParseExpr
    void UpdateSlices(std::vector<std::shared_ptr<VariableData>>& varDataGroup, const FunctionInfo& funcData,
                        std::string className);
    // Parse a given Expression and return a Collection of Variable Data used to Update SliceProfiles
    std::vector<std::shared_ptr<VariableData>>& ParseExpr(const ExprInfo& expr, const SlicePosition& exprPos);
    // Get Type Details (isPtr, isRef, isArr, etc) based of a given DeclData
    std::string GetTypeDetails(const DeclInfo& localVar, bool& isPointer, bool& isReference, bool& isArray);
    // Try-Blocks contain both exprs and decls, need to extract those decls and create slice profiles
    // for them, along with capturing expressions to update collected slices
    void CollectTryBlockData(const FunctionInfo& funcData, std::shared_ptr<srcDispatch::TryData>& tryBlock,
                                std::string className);
    void CollectConditionalData(const FunctionInfo& funcData, std::any& cntl, const std::string& className);
    // Given a list of Function Parameters create Initial SliceProfiles for each Parameter
    void ProcessFunctionParameters(const FunctionInfo& funcData, std::vector<DeclInfo>& parameters,
                                    std::string currentFunctionName, std::string className);
    // Create a Function Signature based off given Function Data
    void ProcessFunctionSignature(FunctionInfo& funcData, std::string className);


    // Use collected function call data to push a new cfunctions entry into a referenced slice profile
    void CreateSliceCallData(std::string functionName, int argIndex, SlicePosition functionPosition, SliceProfile& sliceProfile, SlicePosition invokePosition);

    void ProcessFunctionCall(std::shared_ptr<srcDispatch::CallData>& funcCallData);

    // Take large name strings and extract the root variable name
    std::string ExtractName(std::string elementName);

    bool IsPointerDereferenced(std::shared_ptr<srcDispatch::NameData>& varNameElem);

    // Attempt to Recursively dig into potential nested indices in a RHS to form dependency relations with a LHS variable
    void AppendIndices(std::shared_ptr<VariableData>& lhs, std::shared_ptr<VariableData>& varData);

    // Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
    // Logic constructed for use BEFORE InterProcedural
    SliceProfile* FetchSliceProfile(std::string profileName, std::shared_ptr<VariableData>& vd, const FunctionInfo& funcData,
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
    void ComputeOuterPaths(std::set<std::pair<SlicePosition,SlicePosition>>& otherPaths, std::vector<SlicePosition>& sLines);

    // Component of function FindOtherPaths
    void ComputeExitPaths(std::set<std::pair<SlicePosition,SlicePosition>>& otherPaths, std::vector<SlicePosition>& sLines, std::set<SlicePosition>& ignoreLines);

    // Attempt to find other Forward Control-Flow paths | ComputeControlPaths Helper Function
    std::set<std::pair<SlicePosition,SlicePosition>> FindOtherPaths(std::vector<SlicePosition>& sLines, std::set<SlicePosition>& ignoreLines);

    // srcSlice focuses on Forward-Slicing, therefor our Control-Flows are going to be forward-flowing
    // we are not focusing on backwards-flows.
    void ComputeControlPaths();

    auto ArgumentProfile(std::pair<std::string, FunctionSignatureData> func, int paramIndex);

    // Need to track Aliases we have already read through
    // InterProcedural from the normal call should also be reflected
    // if a profile with alias(s) is used in function calls
    void ComputeAliasInterprocedural();
    void ComputeInterprocedural();
    // Perform a second pass over the system dictionary performing another round of ComputeInterprocedural
    // against slices who have cfunction elements that could not resolve on the initial pass
    void Finalize();
    void ModifySlice(SliceProfile& sp);
    void ResolveCall(SliceProfile& sp);
    void UpdateCalls(SliceProfile& sp);

private:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    std::vector<SliceProfile*> partialSliceProfiles;
    std::unordered_set <std::string> visited_func;

    std::vector<SlicePosition> loopdata;
    std::vector<SlicePosition> forloopdata;
    std::vector<SlicePosition> whileloopdata;
    std::vector<SlicePosition> dowhileloopdata;

    std::vector<SlicePosition> ifdata;
    std::vector<SlicePosition> elseifdata;
    std::vector<SlicePosition> elsedata;

    std::unordered_map<std::string, std::vector<FunctionSignatureData>> functionSigMap;
    bool verboseMode, calculateControlEdges, progressMode;

    SliceCtx sctx;
};

#endif