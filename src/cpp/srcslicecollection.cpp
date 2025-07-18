#include "srcslicecollection.hpp"

VariableData::VariableData(std::string name) {
    dereferenceCount = 0;
    lhsVarName = name;
}

VariableData::VariableData(const VariableData& rhs) {
    lhsVarName = rhs.lhsVarName;
    rhsElems = rhs.rhsElems;
    indices = rhs.indices;
    lhs = rhs.lhs;
    isAddrOf = rhs.isAddrOf;
    dereferenced = rhs.dereferenced;
    uses = rhs.uses;
    definitions = rhs.definitions;
    originLine = rhs.originLine;
    dereferenceCount = 0;
}

VariableData::~VariableData() {}

VariableData& VariableData::operator=(VariableData rhs) {
    std::swap(*this, rhs);
    return *this;
}

// Basic clean up of this struct to allow simple re-purposing
void VariableData::clear(){
    lhsVarName.clear();
    rhsElems.clear();
    indices.clear();
    lhs = false;
    isAddrOf = false;
    dereferenced = false;
    uses.clear();
    definitions.clear();
    dereferenceCount = 0;
}

// Name of an expression variable that may be a LHS/RHS var
// potentially have none to many RHS variables the LHS uses
std::string VariableData::GetNameOfIdentifier() const {
    if (lhsVarName.empty()) return "";
    return lhsVarName;
}

// when the lhsVarName is not an empty string its considered initialized
bool VariableData::isInitialized() { return !lhsVarName.empty(); }

void VariableData::InitializeLHS(std::string name, unsigned int line) { lhsVarName = name; originLine = line; }

void VariableData::SetOriginLine(unsigned int line) { originLine = line; }

void VariableData::AddRHS(std::shared_ptr<VariableData> var) {
    if (rhsElems.size() == 0 || rhsElems.back() != var)
        rhsElems.push_back(var);
}
std::shared_ptr<VariableData> VariableData::GetRecentRHS() { return rhsElems.size() > 0 ? rhsElems.back() : nullptr; }

FunctionSignatureData::FunctionSignatureData(srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& func, const SliceCtx& ctx) {
    lineNumber = func->startLineNumber.GetElement();
    name = func->name.ToString();
    returnType = func->returnType.ToString();
    parameters = func->parameters;
    currentFilePath = ctx.currentFilePath;
    currentFileChecksum = ctx.currentFileChecksum;
    currentFileLanguage = ctx.currentFileLanguage;
    containingNamespaces = ctx.containingNamespaces;
}

FunctionCallData::FunctionCallData(std::string funcName, unsigned int paramIndex, unsigned int funcDefLine, unsigned int invokeLine):
        functionName(funcName), parameterIndex(paramIndex), functionDefinition(funcDefLine), lineOfInvoke(invokeLine) {};

FunctionCallData::FunctionCallData(const FunctionCallData& rhs) {
    functionName = rhs.functionName;
    lineOfInvoke = rhs.lineOfInvoke;
    parameterIndex = rhs.parameterIndex;
    functionDefinition = rhs.functionDefinition;
};

bool FunctionCallData::operator==(const FunctionCallData& rhs) const {
    if (functionName != rhs.functionName) return false;
    if (lineOfInvoke != rhs.lineOfInvoke) return false;
    if (parameterIndex != rhs.parameterIndex) return false;
    if (functionDefinition != rhs.functionDefinition) return false;
    return true;
}
bool FunctionCallData::operator!=(const FunctionCallData& rhs) const {
    return !(*this == rhs);
}

bool FunctionCallData::operator<(const FunctionCallData& rhs) const {
    if (functionName < rhs.functionName) return true;
    if (lineOfInvoke < rhs.lineOfInvoke) return true;
    if (parameterIndex < rhs.parameterIndex) return true;
    if (functionDefinition < rhs.functionDefinition) return true;
    return false;
}
bool FunctionCallData::operator>(const FunctionCallData& rhs) const {
    return !(*this < rhs);
}

std::ostream& operator<<(std::ostream& outStream, const FunctionCallData& data) {
    outStream << "[" << data.functionName << " | " << data.parameterIndex << " | " << data.functionDefinition << " | " << data.lineOfInvoke << "]";
    return outStream;
}

SliceCtx::SliceCtx(const srcDispatch::srcSAXEventContext& ctx) {
    currentFilePath = ctx.currentFilePath;
    currentFileChecksum = ctx.currentFileChecksum;
    currentFileLanguage = ctx.currentFileLanguage;
    containingNamespaces = ctx.currentNamespaces;
}

SliceCtx::SliceCtx(const SliceCtx& rhs) {
    currentFilePath = rhs.currentFilePath;
    currentFileChecksum = rhs.currentFileChecksum;
    currentFileLanguage = rhs.currentFileLanguage;
    containingNamespaces = rhs.containingNamespaces;
}