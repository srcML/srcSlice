#ifndef SRCSLICEHANDLER
#define SRCSLICEHANDLER

#include <srcsliceprofile.hpp>
#include <srcsliceprogress.hpp>
#include <srcsliceparse.hpp>
#include <srcsliceworker.hpp>

#include <mutex>
#include <queue>

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

#if _WIN64
    #include <windows.h>
#elif __APPLE__
    #include <sys/types.h>
    #include <sys/sysctl.h>
#else
    #include <sched.h>
#endif

int CPUCount();

// type aliasing
typedef std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclStmtData>>> DeclStmts;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>> DeclInfo;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ExpressionData>> ExprInfo;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>> FunctionInfo;
typedef srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::BlockData>> BlockInfo;
typedef std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>> Functions;
typedef std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::ClassData>>> Classes;

inline std::atomic_bool unitsScaned{false};
inline std::mutex dataMutex;

class SrcSliceHandler : public srcDispatch::PolicyListener {
public:
    ~SrcSliceHandler();

    // Use literal string filename ctor of srcSAXController (srcslice cpp main)
    SrcSliceHandler(const char* filename, bool v, bool p, bool ce, int threads);

    // Use string srcml buffer ctor of srcSAXController
    SrcSliceHandler(std::string& sourceCodeStr, bool ce);

    void Notify(const srcDispatch::PolicyDispatcher *policy, const srcDispatch::srcSAXEventContext &ctx) override;

    void NotifyWrite(const srcDispatch::PolicyDispatcher *policy [[maybe_unused]], srcDispatch::srcSAXEventContext &ctx [[maybe_unused]]) {};

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

    SliceProfileIterator ArgumentProfile(const std::string& funcName, FunctionSignatureData& funcSig, int paramIndex);

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

    // thread target for managing SrcSliceWorker jobs
    void ManageThreads();

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

    bool verboseMode = false;
    bool calculateControlEdges = false;
    bool progressMode = false;

    int threadCount;
    std::queue<SrcSliceWorker*> backlog;

    SliceCtx sctx;
};

#endif