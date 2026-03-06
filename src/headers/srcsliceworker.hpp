#ifndef SRCSLICE_WORKER
#define SRCSLICE_WORKER

#include <srcsliceprofile.hpp>
#include <srcsliceprogress.hpp>
#include <srcsliceparse.hpp>

#include <chrono>
#include <atomic>
#include <thread>

#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

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

// sub-structure data that is later passed and unioned into the larger main structure
struct Blob {
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;

    std::vector<SlicePosition> loopdata;
    std::vector<SlicePosition> forloopdata;
    std::vector<SlicePosition> whileloopdata;
    std::vector<SlicePosition> dowhileloopdata;

    std::vector<SlicePosition> ifdata;
    std::vector<SlicePosition> ifStmts;
    std::vector<SlicePosition> elseifdata;
    std::vector<SlicePosition> elsedata;

    std::unordered_map<std::string, std::vector<FunctionSignatureData>> functionSigMap;
    
    // logic flags
    bool verboseMode = false;
    bool calculateControlEdges = false;
};

enum class EXPRESSION_TYPE {
    NORMAL,
    IF_CONDITION,
    ELIF_CONDITION,
    SWITCH_CONDITION
};

// Functions used when processing data
namespace SrcSliceOperations {
    // Creates Initial SliceProfiles based off a list of decl statements
    void ProcessDecls(Blob& data, const SliceCtx& sctx, DeclStmts& declStmts, std::string className = "", bool globalDecls = false);
    // Creates Initial SliceProfiles for Function Parameters and Variables Declared within the function definition
    void ProcessFunctions(Blob& data, const SliceCtx& sctx, Functions& funcs);
    // Process Class Data and create slices of Class Member Variables and process Member Functions
    void ProcessClasses(Blob& data, const SliceCtx& sctx, Classes& classes);
    // Process Signatures from Free-Functions and Class Methods
    void ProcessSignatures(Blob& data, const SliceCtx& sctx, Functions& funcs, Classes& classes);

    // Moves over the sequence of statements and processes them in order
    // |__ Creates Initial SliceProfiles for Variables Declared within a specified Block within a Function Definition
    // |__ Extract Expressions within a specified Block within a Function Definition
    void ProcessStmts(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, const BlockInfo& block, std::string className);
    
    // Creates Initial SliceProfile based off DeclData
    void CreateSliceProfile(Blob& data, const SliceCtx& sctx, const DeclInfo& deltaDeclData, const FunctionInfo& funcData, std::string className);
    // Process Constructor Initializer Lists establishing connection between Class Members and Ctor Parameters
    void ProcessInitLists(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::string className);
    
    // Extract Expressions within a specified Block within a Function Definition
    void ProcessExprStmts(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, const BlockInfo& block, std::string className);
    
    // Capture SliceProfile Data from a given Expression within a specified Block within a Function Definition
    void ProcessExprStmt(
        Blob& data,
        const SliceCtx& sctx,
        const ExprInfo& expr,
        const FunctionInfo& funcData,
        std::string className,
        EXPRESSION_TYPE expr_type = EXPRESSION_TYPE::NORMAL
    );
    // Parse a given Expression and return a Collection of Variable Data used to Update SliceProfiles
    void ParseExpr(
        Blob& data,
        const SliceCtx& sctx,
        const ExprInfo& expr, 
        EXPRESSION_TYPE expr_type,
        std::vector<std::string> lhsStack = {},
        bool isArg = false,
        srcDispatch::CallData* funcCallData = nullptr,
        int argIndex = 0
    );
    // Get Type Details (isPtr, isRef, isArr, etc) based of a given DeclData
    std::string GetTypeDetails(const DeclInfo& localVar, bool& isPointer, bool& isReference, bool& isArray);
    // Try-Blocks contain both exprs and decls, need to extract those decls and create slice profiles
    // for them, along with capturing expressions to update collected slices
    void CollectTryBlockData(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::shared_ptr<srcDispatch::TryData>& tryBlock,
                                std::string className);
    void CollectConditionalData(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::any& cntl, const std::string& className);
    // Given a list of Function Parameters create Initial SliceProfiles for each Parameter
    void ProcessFunctionParameters(Blob& data, const SliceCtx& sctx, const FunctionInfo& funcData, std::vector<DeclInfo>& parameters,
                                    std::string currentFunctionName, std::string className);
    // Create a Function Signature based off given Function Data
    void ProcessFunctionSignature(Blob& data, const SliceCtx& sctx, FunctionInfo& funcData, std::string className);

    // Use collected function call data to push a new cfunctions entry into a referenced slice profile
    void CreateSliceCallData(
        Blob& data, const SliceCtx& sctx, std::string functionName,
        int argIndex, int argc, SlicePosition functionPosition, SliceProfile& sliceProfile,
        SlicePosition invokePosition
    );

    // Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
    // Logic constructed for use BEFORE InterProcedural
    SliceProfile* FetchSliceProfile(Blob& data, const SliceCtx& sctx, std::string profileName, const FunctionInfo& funcData,
        std::string className = "", std::vector<std::string> containingNameSpaces = {});

        
    bool IsPointerDereferenced(std::shared_ptr<srcDispatch::NameData>& varNameElem);

    // Take large name strings and extract the root variable name
    std::string ExtractName(std::string elementName);

    // Extract the function name within either a call or a complex function name
    std::string GetSimpleFunctionName(std::string funcName);

    std::string GenerateArrayType(std::string typeString, int dim);

    bool StringContainsCharacters(std::string &str);

    bool isAssignment(std::string& expr_op);

    bool isCompoundAssignment(std::string& expr_op);

    bool isLogical(std::string& expr_op);

    bool isWhiteSpace(std::string& str);
};

class SrcSliceWorker {
public:
    SrcSliceWorker(std::shared_ptr<srcDispatch::UnitData> unitData, const srcDispatch::srcSAXEventContext &ctx,
                    bool verboseMode, bool calculateControlEdges);
    ~SrcSliceWorker();

    void Start();
    void Perform();
    void WaitForJob();
    bool Finished();

    std::shared_ptr<srcDispatch::UnitData> unit;
    std::atomic_bool jobFinished{false};

    std::thread work;
    Blob data;
    SliceCtx sctx;
    bool dispatched;
};

#endif