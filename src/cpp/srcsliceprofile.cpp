#include "srcsliceprofile.hpp"

SliceProfile::SliceProfile() : containsDeclaration(false), potentialAlias(false), dereferenced(false),
                    isGlobal(false) { isPointer = false; isReference = false; }

SliceProfile::SliceProfile(
        std::string name, SlicePosition initial, bool alias, bool global,
        std::set<SlicePosition> aDef, std::set<SlicePosition> aUse,
        std::vector<FunctionCallData> cFunc,
        std::vector<std::pair<std::string, SlicePosition>> dv,
        std::set<std::pair<SlicePosition, SlicePosition>> edges,
        bool containsDecl, bool visit) :
        variableName(name), initialPosition(initial), potentialAlias(alias),
        isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc),
        dvars(dv), containsDeclaration(containsDecl), controlEdges(edges),
        visited(visit) {}

SliceProfile::SliceProfile(const SliceProfile& rhs) {
    initialPosition = rhs.initialPosition;
    file = rhs.file;
    function = rhs.function;
    nameOfContainingClass = rhs.nameOfContainingClass;
    containingNameSpaces = rhs.containingNameSpaces;
    language = rhs.language;
    checksum = rhs.checksum;
    potentialAlias = rhs.potentialAlias;
    dereferenced = rhs.dereferenced;
    isGlobal = rhs.isGlobal;
    classMemberVar = rhs.classMemberVar;
    containsDeclaration = rhs.containsDeclaration;
    isPointer = rhs.isPointer;
    isReference = rhs.isReference;
    variableName = rhs.variableName;
    variableType = rhs.variableType;
    memberVariables = rhs.memberVariables;
    definitions = rhs.definitions;
    uses = rhs.uses;
    dvars = rhs.dvars;
    aliases = rhs.aliases;
    controlEdges = rhs.controlEdges;
    cfunctions = rhs.cfunctions;
    visited = rhs.visited;
    updated = rhs.updated;
    returnUsesInserted = rhs.returnUsesInserted;
    conditionalUsesInserted = rhs.conditionalUsesInserted;
    conditionalDefsInserted = rhs.conditionalDefsInserted;
    showControlEdges = rhs.showControlEdges;
    currentPointerReference = rhs.currentPointerReference;
    isPotentialArray = rhs.isPotentialArray;
    ignorePtrRef = rhs.ignorePtrRef;
}

std::ostream& operator<<(std::ostream& out, SliceProfile& profile) {
    out << "    \"file\":\"" << profile.file << "\"," << std::endl;
    out << "    \"language\":\"" << profile.language << "\"," << std::endl;
    
    out << "    \"namespace\":[";
    bool first = true;
    for (std::string& nameSpace : profile.containingNameSpaces) {
        if (!first) {
            out << ",";
        }
        out << "\"" << nameSpace.substr(0, nameSpace.find(' ')) << "\"";
        first = false;
    }
    out << "]," << std::endl;

    out << "    \"class\":\"" << profile.nameOfContainingClass << "\"," << std::endl;
    out << "    \"function\":\"" << profile.function << "\"," << std::endl;
    out << "    \"type\":\"" << profile.variableType << "\"," << std::endl;
    out << "    \"name\":\"" << profile.variableName << "\"," << std::endl;
    
    out << "    \"initial\":" << profile.initialPosition.ToString() << "," << std::endl;

    out << "    \"dependence\":[";
    first = true;
    for (auto dvar : profile.dvars) {
        if (!first) {
            out << ",";
        }
        out << "{\"" << dvar.first << "\":" << dvar.second.ToString() << "}";
        first = false;
    }
    out << "]," << std::endl;

    out << "    \"aliases\":[";
    first = true;
    for (auto alias : profile.aliases) {
        if (!first) {
            out << ",";
        }
        out << "{\"" << alias.first << "\":" << alias.second.ToString() << "}";
        first = false;
    }
    out << "]," << std::endl;

    out << "    \"calls\":[";
    first = true;
    for (auto cfunc : profile.cfunctions) {
        if (!first) {
            out << ",";
        }
        out << "{\"functionName\":\"" << cfunc.functionName <<
                "\",\"parameter\":\"" << cfunc.parameterIndex <<
                "\",\"definitionPosition\":" << cfunc.definitionPosition.ToString() <<
                ",\"invoke\":" << cfunc.invokePosition.ToString() << "}";
        first = false;
    }
    out << "]," << std::endl;

    // "use": [ { "start": "2:10", "end": "2:17" }, ... ]
    out << "    \"use\":[";
    first = true;
    for (auto use : profile.uses) {
        if (!first) {
            out << ",";
        }
        out << use.ToString();
        first = false;
    }
    out << "]," << std::endl;

    // "definition": [ { "start": "2:10", "end": "2:17" }, ... ]
    out << "    \"definition\":[";
    first = true;
    for (auto def : profile.definitions) {
        if (!first) {
            out << ",";
        }
        out << def.ToString();
        first = false;
    }

    if (profile.showControlEdges) {
        out << "]," << std::endl;

        out << "    \"controlEdges\":[";
        first = true;
        for (auto edge : profile.controlEdges) {
            if (!first) {
                out << ",";
            }
            out << "[" << edge.first.ToString() <<
            "," << edge.second.ToString() << "]";
            first = false;
        }
        out << "]" << std::endl;
    } else {
        out << "]" << std::endl;
    }

    return out;
}

void SliceProfile::insertDvar(std::string name, SlicePosition& sp) {
    std::pair<std::string, SlicePosition> p = std::make_pair(name, sp);
    bool contained = std::find(
        dvars.begin(),
        dvars.end(),
        p
    ) != dvars.end();

    if (!contained) {
        dvars.push_back(p);
    }
}
void SliceProfile::insertAlias(std::string name, SlicePosition& sp) {
    std::pair<std::string, SlicePosition> p = std::make_pair(name, sp);
    bool contained = std::find(
        aliases.begin(),
        aliases.end(),
        p
    ) != aliases.end();
    
    if (!contained) {
        aliases.push_back(p);
    }
}
void SliceProfile::insertCfunction(FunctionCallData& fcd) {
    bool contained = std::find(
        cfunctions.begin(),
        cfunctions.end(),
        fcd
    ) != cfunctions.end();
    
    if (!contained) {
        cfunctions.push_back(fcd);
    }
}