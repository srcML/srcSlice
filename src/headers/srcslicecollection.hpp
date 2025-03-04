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
#include <fstream>
#include <sstream>
#include <FunctionPolicySingleEvent.hpp>
#include <ClassPolicySingleEvent.hpp>
#include <UnitPolicySingleEvent.hpp>

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
    std::unordered_map<std::string, std::vector<std::shared_ptr<FunctionData>>> functionSigMap;
};

class ConditionalImpact {
    public:
        ConditionalImpact(std::pair<int,int> cr): conditionalRange(cr) {};

        // compare the left slice profile to a right slice profile
        // and determine of they match
        bool Contains(SliceProfile& lsp, SliceProfile& rsp) {
            // compare the name and initial decl line number
            return (lsp.variableName == rsp.variableName && lsp.lineNumber == rsp.lineNumber);
        }

        // Check if a slice profile is contained in the impact controls
        bool Contains(SliceProfile& sp) {
            for (const auto& controlVar : controls) {
                if ( (controlVar->variableName == sp.variableName) && (controlVar->lineNumber == sp.lineNumber) )
                    return true;
            }
            return false;
        }

        void AddImpact(SliceProfile& sp) {
            if (impacts.size() == 0 || !Contains(*(impacts.back()), sp)) {
                impacts.push_back(&sp);
            }
        }

        void AddControl(SliceProfile& sp) {
            if (controls.size() == 0 || !Contains(*(controls.back()), sp)) {
                controls.push_back(&sp);
            }
        }

        // check if the Impact Collection control contains impacts
        bool HasImpacts() { return !impacts.empty(); }

        // determine if a local variable line is contained in this conditionals line range
        bool IsOfInterest(int line) {
            return (line >= conditionalRange.first && line <= conditionalRange.second);
        }
        
        // check if this conditionals line range is nested within a given conditionalRange (cr)
        bool IsNested(std::pair<int,int> cr) {
            return cr.first < conditionalRange.first && cr.second > conditionalRange.second;
        }
        
        // start and end line range of conditional
        std::pair<int,int> conditionalRange;
        // list of references to slice profiles the conditional potentially impacts
        std::vector<SliceProfile*> impacts;
        // list of control slices
        std::vector<SliceProfile*> controls;
};

#endif