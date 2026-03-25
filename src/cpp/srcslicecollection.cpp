#include "srcslicecollection.hpp"

SlicePosition::SlicePosition(): filename("") {}
SlicePosition::SlicePosition(
                    srcDispatch::DeltaElement<srcDispatch::Position> start,
                    srcDispatch::DeltaElement<srcDispatch::Position> end,
                    std::string filename
                ): start(start), end(end), filename(filename) {}

SlicePosition::SlicePosition(const SlicePosition& position) {
    start = position.start;
    end = position.end;
    filename = position.filename;
    data = position.data;
}

// Creates a String of a JSON object, ie: "file.cpp:2:12"
// based on the start position
std::string SlicePosition::ToString() const {
    std::string s;

    s += "\"";

    if (!filename.empty()) {
        s += filename;
    }

    if (start) {
        s += ":";
        s += start->ToString();
    }

    s += "\"";

    return s;
}
std::string SlicePosition::RangeToString() const {
    std::string s;

    s += "\"";
    if (start && !filename.empty()) {
        s += filename; s += ":";
        s += start->ToString();
    }
    s += "-";
    if (end && !filename.empty()) {
        s += filename; s += ":";
        s += end->ToString();
    }
    s += "\"";

    return s;
}
// produces a string used in JSON key creation, ie: argc-3-14-e12a342c...2a41
// introducing line-col into key string helps keep keys unique when there are
// multiple variables of the same name throughout a file
std::string SlicePosition::ToNameString() const {
    std::string s;

    std::string lineStr = start ? std::to_string(start->GetLine()) : "0";
    std::string colStr = start ? std::to_string(start->GetColumn()) : "0";
    s += lineStr + "-" + colStr;

    return s;
}

srcDispatch::DeltaElement<srcDispatch::Position> SlicePosition::GetStart() const {
    return start;
}
srcDispatch::DeltaElement<srcDispatch::Position> SlicePosition::GetEnd() const {
    return end;
}
std::string SlicePosition::GetFileName() const { return filename; }
PositionMeta& SlicePosition::GetData() { return data; }

SlicePosition& SlicePosition::operator=(SlicePosition rhs) {
    if (this == &rhs) return *this;

    start = rhs.start;
    end = rhs.end;
    filename = rhs.filename;
    data = rhs.data;

    return *this;
}
bool SlicePosition::operator==(const SlicePosition& rhs) const {
    bool validPositions = (start && end && rhs.start && rhs.end);
    if (validPositions) {
        bool matchingStarts = start->GetLine() == rhs.start->GetLine() && start->GetColumn() == rhs.start->GetColumn();
        bool matchingEnds = end->GetLine() == rhs.end->GetLine() && end->GetColumn() == rhs.end->GetColumn();
        bool matchingFiles = filename == rhs.filename;
        
        return matchingStarts && matchingEnds && matchingFiles;
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
        if (start->GetColumn() == rhs.start->GetColumn()) {
            // if line:col matches check if filenames are different
            return filename != rhs.filename;
        }
        return start->GetColumn() < rhs.start->GetColumn();
    }
    return false;
}
bool SlicePosition::operator<=(const SlicePosition& rhs) const {
    return (rhs < *this) || rhs == *this;
}
bool SlicePosition::operator>(const SlicePosition& rhs) const {
    return rhs < *this;
}
bool SlicePosition::operator>=(const SlicePosition& rhs) const {
    return (rhs > *this) || rhs == *this;
}

bool IsContained(SlicePosition& a, SlicePosition b) {
    if (a.GetFileName() != b.GetFileName()) return false;
    
    bool lineContained = a.GetStart()->GetLine() >= b.GetStart()->GetLine() && a.GetEnd()->GetLine() <= b.GetEnd()->GetLine();

    bool isOneLiner = a.GetStart()->GetLine() == b.GetStart()->GetLine() && a.GetEnd()->GetLine() == b.GetEnd()->GetLine();
    bool colContained = true;
    
    if (isOneLiner) {
        // start column of a should be greater than the start column of b
        // end column of a should be less than the end column of b
        colContained = a.GetStart()->GetColumn() >= b.GetStart()->GetColumn() && a.GetEnd()->GetColumn() <= b.GetEnd()->GetColumn();
    } else {
        // if the end row of b and the start row of a are the same
        // check if the start column of a is less than the end column of b
        if (a.GetStart()->GetLine() == b.GetEnd()->GetLine()) {
            colContained = a.GetStart()->GetColumn() < b.GetEnd()->GetColumn();
        }
    }

    return lineContained && colContained;
};

size_t GetDistance(SlicePosition& a, SlicePosition& b) {
    size_t dist = 0;
    dist = b.GetStart()->GetLine() - a.GetStart()->GetLine();

    // if row a > row b => a isnt connected to b
    if (dist >= 0) {
        size_t bc = b.GetEnd()->GetColumn();
        size_t ac = a.GetStart()->GetColumn();
        
        dist += ac - bc;
    }

    return dist;
}

size_t FindContextBlock(SlicePosition& sline, std::vector<SlicePosition>& group) {
    size_t ctxIndex = -1;
    for (size_t i = 0; i < group.size(); ++i) {
        // skip ifStmt blocks that are below the sline
        if (sline < group[i]) continue;

        // may trigger multiple times in nesting situations
        if (IsContained(sline, group[i])) {
            ctxIndex = i;
        }
    }
    return ctxIndex;
};

FunctionSignatureData::FunctionSignatureData(
    srcDispatch::DeltaElement<std::shared_ptr<srcDispatch::FunctionData>>& func,
    std::string className,
    const SliceCtx& ctx) {
    position = SlicePosition(func->startPosition, func->endPosition, ctx.currentFilePath);
    name = func->name.ToString();
    returnType = func->returnType.ToString();
    parameters = func->parameters;
    containingClass = className;
    currentFilePath = ctx.currentFilePath;
    currentFileChecksum = ctx.currentFileChecksum;
    currentFileLanguage = ctx.currentFileLanguage;
    containingNamespaces = ctx.containingNamespaces;
}

FunctionCallData::FunctionCallData(
    std::string funcName,
    unsigned int paramIndex,
    unsigned int argc,
    SlicePosition defPos,
    SlicePosition invokePos,
    bool ignore_
): functionName(funcName), parameterIndex(paramIndex),
definitionPosition(defPos), invokePosition(invokePos),
argumentCount(argc), ignore(ignore_) {};

FunctionCallData::FunctionCallData(const FunctionCallData& rhs) {
    functionName = rhs.functionName;
    invokePosition = rhs.invokePosition;
    parameterIndex = rhs.parameterIndex;
    argumentCount = rhs.argumentCount;
    definitionPosition = rhs.definitionPosition;
    ignore = rhs.ignore;
};

FunctionCallData& FunctionCallData::operator=(const FunctionCallData& rhs) {
    if (this == &rhs) return *this;

    functionName = rhs.functionName;
    invokePosition = rhs.invokePosition;
    parameterIndex = rhs.parameterIndex;
    argumentCount = rhs.argumentCount;
    definitionPosition = rhs.definitionPosition;
    ignore = rhs.ignore;

    return *this;
}

bool FunctionCallData::operator==(const FunctionCallData& rhs) const {
    if (functionName != rhs.functionName) return false;
    if (invokePosition != rhs.invokePosition) return false;
    if (parameterIndex != rhs.parameterIndex) return false;
    if (argumentCount != rhs.argumentCount) return false;
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
    if (argumentCount < rhs.argumentCount) return true;
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
    defined = true;
    currentFilePath = ctx.currentFilePath;
    currentFileChecksum = ctx.currentFileChecksum;
    currentFileLanguage = ctx.currentFileLanguage;
    containingNamespaces = ctx.currentNamespaces;
}

SliceCtx::SliceCtx(const SliceCtx& rhs) {
    defined = rhs.defined;
    currentFilePath = rhs.currentFilePath;
    currentFileChecksum = rhs.currentFileChecksum;
    currentFileLanguage = rhs.currentFileLanguage;
    containingNamespaces = rhs.containingNamespaces;
}