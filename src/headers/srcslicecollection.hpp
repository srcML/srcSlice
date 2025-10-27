#ifndef SRCSLICECOLLETION
#define SRCSLICECOLLETION

#include <exception>
#include <unordered_map>
#include <set>
#include <memory>
#include <any>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <srcSAXHandler.hpp>
#include <srcDispatcher.hpp>
#include <FunctionPolicy.hpp>
#include <ClassPolicy.hpp>
#include <UnitPolicy.hpp>

class VariableData {
public:
    VariableData(std::string name="");
    VariableData(const VariableData& rhs);
    ~VariableData();

    VariableData& operator=(VariableData rhs);

    // Basic clean up of this struct to allow simple re-purposing
    void clear();

    // Name of an expression variable that may be a LHS/RHS var
    // potentially have none to many RHS variables the LHS uses
    std::string GetNameOfIdentifier() const;

    // when the lhsVarName is not an empty string its considered initialized
    bool isInitialized();

    void InitializeLHS(std::string name, srcDispatch::DeltaElement<srcDispatch::Position> position);

    void SetOriginLine(srcDispatch::DeltaElement<srcDispatch::Position> position);

    void AddRHS(std::shared_ptr<VariableData> var);
    std::shared_ptr<VariableData> GetRecentRHS();

    std::string lhsVarName;
    std::vector<std::shared_ptr<VariableData>> rhsElems;
    std::vector<std::shared_ptr<VariableData>> indices;

    bool lhs = false;
    bool isAddrOf = false;
    bool dereferenced = false;

    // used concerning the user redefining data a pointer points-to
    // but can be modified to be more expansive
    bool userModified = false;
    
    srcDispatch::DeltaElement<srcDispatch::Position> originPosition;
    int dereferenceCount;
    std::set<srcDispatch::DeltaElement<srcDispatch::Position>> uses;
    std::set<srcDispatch::DeltaElement<srcDispatch::Position>> definitions;
};

// Hold context data srcslice uses based off srcSAXEventContext without copying
// the entirety of srcSAXEventContext
class SliceCtx {
public:
    SliceCtx(const srcDispatch::srcSAXEventContext& ctx);

    SliceCtx(const SliceCtx& rhs);

    std::string currentFilePath;
    std::string currentFileChecksum;
    std::string currentFileLanguage;
    std::vector<std::string> containingNamespaces;
};

// Store Data about Function Signatures by grouping functions of the same name
// like Slices in profileMap
class FunctionSignatureData {
public:
    FunctionSignatureData(){};
    FunctionSignatureData(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& func,
                            std::string className, const SliceCtx& ctx);
    srcDispatch::DeltaElement<srcDispatch::Position> position;
    std::string name;
    std::string returnType;
    std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::DeclData>>> parameters;
    std::string containingClass;
    std::string currentFilePath;
    std::string currentFileChecksum;
    std::string currentFileLanguage;
    std::vector<std::string> containingNamespaces;
};

// Store meta-data about a function-call by tracking:
// name of the function being called, line where the call is made, index of the parameter,
// and the line where the function is potentially defined
class FunctionCallData {
public:
    FunctionCallData(std::string funcName, unsigned int paramIndex,
                    srcDispatch::DeltaElement<srcDispatch::Position> defPos,
                    srcDispatch::DeltaElement<srcDispatch::Position> invokePos,
                    bool ignore_ = false);

    FunctionCallData(const FunctionCallData& rhs);

    bool operator==(const FunctionCallData& rhs) const;
    bool operator!=(const FunctionCallData& rhs) const;

    bool operator<(const FunctionCallData& rhs) const;
    bool operator>(const FunctionCallData& rhs) const;

    friend std::ostream& operator<<(std::ostream& outStream, const FunctionCallData& data);

    std::string functionName;
    srcDispatch::DeltaElement<srcDispatch::Position> invokePosition;
    unsigned int parameterIndex;
    srcDispatch::DeltaElement<srcDispatch::Position> definitionPosition;
    bool ignore;
};

#endif