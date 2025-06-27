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
#include <FunctionPolicy.hpp>
#include <ClassPolicy.hpp>
#include <UnitPolicy.hpp>

class VariableData {
public:
    VariableData(std::string name=""): lhsVarName(name){}
    VariableData(const VariableData& rhs) {
        lhsVarName = rhs.lhsVarName;
        rhsElems = rhs.rhsElems;
        indices = rhs.indices;
        lhs = rhs.lhs;
        isAddrOf = rhs.isAddrOf;
        dereferenced = rhs.dereferenced;
        uses = rhs.uses;
        definitions = rhs.definitions;
        originLine = rhs.originLine;
    }
    ~VariableData() {}

    VariableData& operator=(VariableData rhs) {
        std::swap(*this, rhs);
        return *this;
    }

    // Basic clean up of this struct to allow simple re-purposing
    void clear(){
        lhsVarName.clear();
        rhsElems.clear();
        indices.clear();
        lhs = false;
        isAddrOf = false;
        dereferenced = false;
        uses.clear();
        definitions.clear();
    }

    // Name of an expression variable that may be a LHS/RHS var
    // potentially have none to many RHS variables the LHS uses
    std::string GetNameOfIdentifier() const {
        if (lhsVarName.empty()) return "";
        return lhsVarName;
    }

    // when the lhsVarName is not an empty string its considered initialized
    bool isInitialized() { return !lhsVarName.empty(); }

    void InitializeLHS(std::string name, unsigned int line) { lhsVarName = name; originLine = line; }

    void SetOriginLine(unsigned int line) { originLine = line; }

    void AddRHS(std::shared_ptr<VariableData> var) {
        if (rhsElems.size() == 0 || rhsElems.back() != var)
            rhsElems.push_back(var);
    }
    std::shared_ptr<VariableData> GetRecentRHS() { return rhsElems.size() > 0 ? rhsElems.back() : nullptr; }

    std::string lhsVarName;
    std::vector<std::shared_ptr<VariableData>> rhsElems;
    std::vector<std::shared_ptr<VariableData>> indices;

    bool lhs = false;
    bool isAddrOf = false;
    bool dereferenced = false;

    // used concerning the user redefining data a pointer points-to
    // but can be modified to be more expansive
    bool userModified = false;
    
    unsigned int originLine;
    std::set<unsigned int> uses;
    std::set<unsigned int> definitions;
};

// Store Data about Function Signatures by grouping functions of the same name
// like Slices in profileMap
struct FunctionSignatureData {
    std::unordered_map<std::string, std::vector<srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>>> functionSigMap;
};

// Store meta-data about a function-call by tracking:
// name of the function being called, line where the call is made, index of the parameter,
// and the line where the function is potentially defined
class FunctionCallData {
public:
    FunctionCallData(std::string funcName, unsigned int paramIndex, unsigned int funcDefLine, unsigned int invokeLine):
        functionName(funcName), parameterIndex(paramIndex), functionDefinition(funcDefLine), lineOfInvoke(invokeLine) {};

    FunctionCallData(const FunctionCallData& rhs) {
        functionName = rhs.functionName;
        lineOfInvoke = rhs.lineOfInvoke;
        parameterIndex = rhs.parameterIndex;
        functionDefinition = rhs.functionDefinition;
    };

    bool operator==(const FunctionCallData& rhs) const {
        if (functionName != rhs.functionName) return false;
        if (lineOfInvoke != rhs.lineOfInvoke) return false;
        if (parameterIndex != rhs.parameterIndex) return false;
        if (functionDefinition != rhs.functionDefinition) return false;
        return true;
    }
    bool operator!=(const FunctionCallData& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const FunctionCallData& rhs) const {
        if (functionName < rhs.functionName) return true;
        if (lineOfInvoke < rhs.lineOfInvoke) return true;
        if (parameterIndex < rhs.parameterIndex) return true;
        if (functionDefinition < rhs.functionDefinition) return true;
        return false;
    }
    bool operator>(const FunctionCallData& rhs) const {
        return !(*this < rhs);
    }

    friend std::ostream& operator<<(std::ostream& outStream, const FunctionCallData& data) {
        outStream << "[" << data.functionName << " | " << data.parameterIndex << " | " << data.functionDefinition << " | " << data.lineOfInvoke << "]";
        return outStream;
    }

    std::string functionName;
    unsigned int lineOfInvoke;
    unsigned int parameterIndex;
    unsigned int functionDefinition;
};

// Hold context data srcslice uses based off srcSAXEventContext without copying
// the entirety of srcSAXEventContext
class SliceCtx {
public:
    SliceCtx(const srcDispatch::srcSAXEventContext& ctx) {
        currentFilePath = ctx.currentFilePath;
        currentFileChecksum = ctx.currentFileChecksum;
        currentFileLanguage = ctx.currentFileLanguage;
        containingNamespaces = ctx.currentNamespaces;
    }

    SliceCtx(const SliceCtx& rhs) {
        currentFilePath = rhs.currentFilePath;
        currentFileChecksum = rhs.currentFileChecksum;
        currentFileLanguage = rhs.currentFileLanguage;
        containingNamespaces = rhs.containingNamespaces;
    }

    std::string currentFilePath;
    std::string currentFileChecksum;
    std::string currentFileLanguage;
    std::vector<std::string> containingNamespaces;
};

#endif