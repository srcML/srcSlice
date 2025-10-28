#include "srcsliceprofile.hpp"

SliceProfile::SliceProfile() : containsDeclaration(false), potentialAlias(false), dereferenced(false),
                    isGlobal(false) { isPointer = false; isReference = false; }

SliceProfile::SliceProfile(
        std::string name, SlicePosition initial, bool alias, bool global,
        std::set<SlicePosition> aDef, std::set<SlicePosition> aUse,
        std::set<FunctionCallData> cFunc,
        std::set<std::pair<std::string, SlicePosition>> dv,
        std::set<std::pair<SlicePosition, SlicePosition>> edges,
        bool containsDecl, bool visit) :
        variableName(name), initialPosition(initial), potentialAlias(alias),
        isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc),
        dvars(dv), containsDeclaration(containsDecl), controlEdges(edges),
        visited(visit) {
    dereferenced = false;
    isPointer = false;
    isReference = false;
}

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
    for (std::string& nameSpace : profile.containingNameSpaces) {
        if (nameSpace != profile.containingNameSpaces.back())
            out << "\"" << nameSpace.substr(0, nameSpace.find(' ')) << "\",";
        else
            out << "\"" << nameSpace.substr(0, nameSpace.find(' ')) << "\"";
    }
    out << "]," << std::endl;

    out << "    \"class\":\"" << profile.nameOfContainingClass << "\"," << std::endl;
    out << "    \"function\":\"" << profile.function << "\"," << std::endl;
    out << "    \"type\":\"" << profile.variableType << "\"," << std::endl;
    out << "    \"name\":\"" << profile.variableName << "\"," << std::endl;
    
    out << "    \"initial\":" << profile.initialPosition.ToString() << "," << std::endl;

    out << "    \"dependence\":[";
    for (auto dvar : profile.dvars) {
        if (dvar != *(--profile.dvars.end()))
            out << "{\"" << dvar.first << "\":" << dvar.second.ToString() << "},";
        else
            out << "{\"" << dvar.first << "\":" << dvar.second.ToString() << "}";
    }
    out << "]," << std::endl;

    out << "    \"aliases\":[";
    for (auto alias : profile.aliases) {
        if (alias != *(--profile.aliases.end()))
            out << "{\"" << alias.first << "\":" << alias.second.ToString() << "},";
        else
            out << "{\"" << alias.first << "\":" << alias.second.ToString() << "}";
    }
    out << "]," << std::endl;

    out << "    \"calls\":[";
    for (auto cfunc : profile.cfunctions) {
        if (cfunc != *(--profile.cfunctions.end()))
            out << "{\"functionName\":\"" << cfunc.functionName <<
                    "\",\"parameter\":\"" << cfunc.parameterIndex <<
                    "\",\"definitionLine\":\"" << cfunc.definitionPosition.ToString() <<
                    "\",\"invoke\":\"" << cfunc.invokePosition.ToString() <<
                    "\"},";
        else
            out << "{\"functionName\":\"" << cfunc.functionName <<
                    "\",\"parameter\":\"" << cfunc.parameterIndex <<
                    "\",\"definitionLine\":\"" << cfunc.definitionPosition.ToString() <<
                    "\",\"invoke\":\"" << cfunc.invokePosition.ToString() <<
                    "\"}";
    }
    out << "]," << std::endl;

    // "use": [ { "start": "2:10", "end": "2:17" }, ... ]
    out << "    \"use\":[";
    for (auto use : profile.uses) {
        if (use != *(--profile.uses.end()))
            out << use.ToString() << ",";
        else
            out << use.ToString();
    }
    out << "]," << std::endl;

    // "definition": [ { "start": "2:10", "end": "2:17" }, ... ]
    out << "    \"definition\":[";
    for (auto def : profile.definitions) {
        if (def != *(--profile.definitions.end()))
            out << def.ToString() << ",";
        else
            out << def.ToString();
    }

    if (profile.showControlEdges) {
        out << "]," << std::endl;

        out << "    \"controlEdges\":[";
        for (auto edge : profile.controlEdges) {
            if (edge != *(--profile.controlEdges.end()))
                out << "[" << edge.first.ToString() <<
                "," << edge.second.ToString() << "],";
            else
                out << "[" << edge.first.ToString() <<
                "," << edge.second.ToString() << "]";
        }
        out << "]" << std::endl;
    } else {
        out << "]" << std::endl;
    }

    return out;
}