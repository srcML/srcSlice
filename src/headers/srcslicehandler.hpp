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
    SrcSliceHandler(const std::string& sourceCodeStr, bool ce);

    std::vector<std::shared_ptr<srcDispatch::ClassData>> GetClassInfo(std::shared_ptr<srcDispatch::UnitData>& unit);

    std::vector<std::shared_ptr<srcDispatch::FunctionData>> GetFunctionInfo(std::shared_ptr<srcDispatch::UnitData>& unit);

    std::vector<std::shared_ptr<srcDispatch::DeclData>> GetDeclInfo(std::shared_ptr<srcDispatch::UnitData>& unit);

    void ProcessUnits();

    void Notify(const srcDispatch::PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) override;

    void NotifyWrite(const srcDispatch::PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {};

    void ProcessFunctionData(std::shared_ptr<srcDispatch::FunctionData> function_data, std::string className,
                            std::vector<std::string>& containingNamespaces, const SliceCtx &ctx);

    void ProcessClassData(std::shared_ptr<srcDispatch::ClassData> class_data, const SliceCtx &ctx);

    void ProcessDeclStmts(std::shared_ptr<srcDispatch::FunctionData> funcData, std::shared_ptr<srcDispatch::BlockData> block, const std::shared_ptr<srcDispatch::ClassData> classData,
                            std::string className, std::shared_ptr<std::vector<std::shared_ptr<srcDispatch::DeclData>>> potentialGlobals,
                            const SliceCtx &ctx);

    void ProcessInitLists(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::string className, const SliceCtx &ctx);
    void ProcessExprStmts(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::shared_ptr<srcDispatch::BlockData> block,
                            std::string className, const SliceCtx &ctx);

    void UpdateSlices(std::vector<std::shared_ptr<VariableData>> varDataGroup, srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData,
                        std::string className, const SliceCtx &ctx);

    // Use collected function call data to push a new cfunctions entry into a referenced slice profile
    void CreateSliceCallData(std::string functionName, int argIndex, int functionDefLine, SliceProfile& sliceProfile, unsigned int functionCallLine);

    void ProcessFunctionCall(std::shared_ptr<srcDispatch::CallData> funcCallData);

    // try blocks contain both exprs and decls, need to extract those decls and create slice profiles
    // for them, along with capturing expressions to update collected slices
    void CollectTryBlockData(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::vector<std::shared_ptr<srcDispatch::TryData>>& tryBlocks,
                                std::string className, const SliceCtx &ctx);

    void CollectConditionalData(std::vector<std::shared_ptr<srcDispatch::ExpressionData>>* exprStmts, std::vector<std::shared_ptr<srcDispatch::DeclData>>* declStmts,
                                std::vector<std::any>& conditionals);

    // Take large name strings and extract the root variable name
    std::string ExtractName(std::string elementName);

    bool IsPointerDereferenced(std::shared_ptr<srcDispatch::NameData>& varNameElem);

    // Attempt to Recursively dig into potential nested indices in a RHS to form dependency relations with a LHS variable
    void AppendIndices(std::shared_ptr<VariableData>& lhs, std::shared_ptr<VariableData>& varData);

    std::vector<std::shared_ptr<VariableData>> ParseExpr(const srcDispatch::ExpressionData& expr, const unsigned int& lineNumber);

    void ProcessFunctionParameters(std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>>& parameters, const std::string& currentFunctionName,
                                    std::string className, std::vector<std::string>& containingNamespaces,
                                    const SliceCtx &ctx);

    void ProcessFunctionSignature(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> funcData, std::string className, std::vector<std::string>& containingNamespaces,
                                    const SliceCtx &ctx);

    // Attempt to get the SliceProfile by finger-printing based on VariableData and containing elements (function, class, namespace)
    // Logic constructed for use BEFORE InterProcedural
    SliceProfile* FetchSliceProfile(std::string profileName, const std::shared_ptr<VariableData>& vd, const srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& funcData,
                                    const std::string& className = "", const std::vector<std::string>& containingNameSpaces = {});

    // Extract the function name within either a call or a complex function name
    std::string GetSimpleFunctionName(std::string funcName);

    // Used for inserting Uses and Defs for Slices in the LHS of an Expression Statement
    // perform a map find and update the slice
    void UpdateLHSSlices(std::shared_ptr<VariableData> varData);

    bool StringContainsCharacters(const std::string &str);

    bool isAssignment(const std::string& expr_op);

    bool isCompoundAssignment(const std::string& expr_op);

    bool isLogical(const std::string& expr_op);

    bool isWhiteSpace(const std::string& str);

    std::unordered_map<std::string, std::vector<SliceProfile>>& GetProfileMap();

    // Component of function FindOtherPaths
    void ComputeOuterPaths(std::set<std::pair<int,int>>& otherPaths, const std::vector<int>& sLines);

    // Component of function FindOtherPaths
    void ComputeExitPaths(std::set<std::pair<int,int>>& otherPaths, const std::vector<int>& sLines, const std::set<int>& ignoreLines);

    // Attempt to find other Forward Control-Flow paths | ComputeControlPaths Helper Function
    std::set<std::pair<int,int>> FindOtherPaths(const std::vector<int>& sLines, const std::set<int>& ignoreLines);

    // srcSlice focuses on Forward-Slicing, therefor our Control-Flows are going to be forward-flowing
    // we are not focusing on backwards-flows.
    void ComputeControlPaths();

    auto ArgumentProfile(std::pair<std::string, srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>> func, int paramIndex, std::unordered_set<std::string>& visit_func);

    // Need to track Aliases we have already read through
    // InterProcedural from the normal call should also be reflected
    // if a profile with alias(s) is used in function calls
    void ComputeAliasInterprocedural();
    void ComputeInterprocedural();

private:
    void GenerateSlices();

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