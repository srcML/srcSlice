#include "srcslicecollection.hpp"

SlicePosition::SlicePosition(){}
SlicePosition::SlicePosition(
                    srcDispatch::DeltaElement<srcDispatch::Position> start,
                    srcDispatch::DeltaElement<srcDispatch::Position> end
                ): start(start), end(end) {}

SlicePosition::SlicePosition(const SlicePosition& position) {
    start = position.start;
    end = position.end;
}

// Creates a String of a JSON object, ie: "2:12"
std::string SlicePosition::ToString() const {
    std::string s;

    s += "\"";
    if (start) {
        s += start->ToString();
    }
    s += "\"";

    return s;
}
// produces a string used in JSON key creation, ie: argc-3-14-e12a342c...2a41
// introducing line-col into key string helps keep keys unique when there are
// multiple variables of the same name throughout a file
std::string SlicePosition::ToNameString() const {
    std::string s;

    std::string lineStr = std::to_string(start->GetLine());
    std::string colStr = std::to_string(start->GetColumn());
    s += lineStr + "-" + colStr;

    return s;
}

srcDispatch::DeltaElement<srcDispatch::Position> SlicePosition::GetStart() const {
    return start;
}
srcDispatch::DeltaElement<srcDispatch::Position> SlicePosition::GetEnd() const {
    return end;
}

SlicePosition& SlicePosition::operator=(SlicePosition rhs) {
    if (this == &rhs) return *this;

    start = rhs.start;
    end = rhs.end;
    return *this;
}
bool SlicePosition::operator==(const SlicePosition& rhs) const {
    bool validPositions = (start && end && rhs.start && rhs.end);
    if (validPositions) {
        bool matchingStarts = start->GetLine() == rhs.start->GetLine() && start->GetColumn() == rhs.start->GetColumn();
        bool matchingEnds = end->GetLine() == rhs.end->GetLine() && end->GetColumn() == rhs.end->GetColumn();
        return matchingStarts && matchingEnds;
    }
    return false;
}
bool SlicePosition::operator!=(const SlicePosition& rhs) const {
    return !(*this == rhs);
}
bool SlicePosition::operator<(const SlicePosition& rhs) const {
    bool validPositions = (start && rhs.start);
    if (validPositions) {
        // line comparison
        if (start->GetLine() < rhs.start->GetLine()) return true;
        if (start->GetLine() > rhs.start->GetLine()) return false;

        // same line number compare the columns
        return start->GetColumn() < rhs.start->GetColumn();
    }
    return false;
}
bool SlicePosition::operator>(const SlicePosition& rhs) const {
    return rhs < *this;
}

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
    originPosition = rhs.originPosition;
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

void VariableData::InitializeLHS(std::string name, SlicePosition position) {
    lhsVarName = name;
    originPosition = position;
}

void VariableData::SetOriginLine(SlicePosition position) {
    originPosition = position;
}

void VariableData::AddRHS(std::shared_ptr<VariableData> var) {
    if (rhsElems.size() == 0 || rhsElems.back() != var)
        rhsElems.push_back(var);
}
std::shared_ptr<VariableData> VariableData::GetRecentRHS() {
    return rhsElems.size() > 0 ? rhsElems.back() : nullptr;
}

FunctionSignatureData::FunctionSignatureData(
    srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& func,
    std::string className,
    const SliceCtx& ctx) {
    position = SlicePosition(func->startPosition, func->endPosition);
    name = func->name.ToString();
    returnType = func->returnType.ToString();
    parameters = func->parameters;
    containingClass = className;
    currentFilePath = ctx.currentFilePath;
    currentFileChecksum = ctx.currentFileChecksum;
    currentFileLanguage = ctx.currentFileLanguage;
    containingNamespaces = ctx.containingNamespaces;
}

FunctionCallData::FunctionCallData(std::string funcName, unsigned int paramIndex,
                                    SlicePosition defPos,
                                    SlicePosition invokePos,
                                    bool ignore_
                                ): functionName(funcName), parameterIndex(paramIndex),
                                definitionPosition(defPos), invokePosition(invokePos),
                                ignore(ignore_) {};

FunctionCallData::FunctionCallData(const FunctionCallData& rhs) {
    functionName = rhs.functionName;
    invokePosition = rhs.invokePosition;
    parameterIndex = rhs.parameterIndex;
    definitionPosition = rhs.definitionPosition;
    ignore = rhs.ignore;
};

bool FunctionCallData::operator==(const FunctionCallData& rhs) const {
    if (functionName != rhs.functionName) return false;
    if (invokePosition != rhs.invokePosition) return false;
    if (parameterIndex != rhs.parameterIndex) return false;
    if (definitionPosition != rhs.definitionPosition) return false;
    if (ignore != rhs.ignore) return false;
    return true;
}
bool FunctionCallData::operator!=(const FunctionCallData& rhs) const {
    return !(*this == rhs);
}

bool FunctionCallData::operator<(const FunctionCallData& rhs) const {
    if (functionName < rhs.functionName) return true;
    if (invokePosition < rhs.invokePosition) return true;
    if (parameterIndex < rhs.parameterIndex) return true;
    if (definitionPosition < rhs.definitionPosition) return true;
    if (ignore == false && rhs.ignore == true) return true;
    return false;
}
bool FunctionCallData::operator>(const FunctionCallData& rhs) const {
    return !(*this < rhs);
}

std::ostream& operator<<(std::ostream& outStream, const FunctionCallData& data) {
    outStream << "[" << data.functionName << " | " << data.parameterIndex
    << " | " << data.definitionPosition.ToString() << " | " << data.invokePosition.ToString() << "]";
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