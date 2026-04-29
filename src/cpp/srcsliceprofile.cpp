// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcsliceprofile.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

#include "srcsliceprofile.hpp"

SliceProfile::SliceProfile() : containsDeclaration(false), potentialAlias(false), dereferenced(false),
                    isGlobal(false) { isPointer = false; isReference = false; }

SliceProfile::SliceProfile(
        std::string name, SlicePosition decl, bool alias, bool global,
        std::set<SlicePosition> aDef, std::set<SlicePosition> aUse,
        std::set<FunctionCallData> cFunc,
        std::set<std::pair<std::string, SlicePosition>> dv,
        std::set<std::pair<SlicePosition, SlicePosition>> edges,
        bool containsDecl, bool visit) :
        variableName(name), declPosition(decl), potentialAlias(alias),
        isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc),
        dvars(dv), containsDeclaration(containsDecl), controlEdges(edges),
        visited(visit) {}

SliceProfile::SliceProfile(const SliceProfile& rhs) {
    declPosition = rhs.declPosition;
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
    definitions = rhs.definitions;
    uses = rhs.uses;
    dvars = rhs.dvars;
    aliases = rhs.aliases;
    controlEdges = rhs.controlEdges;
    cfunctions = rhs.cfunctions;
    endOfScope = rhs.endOfScope;
    visited = rhs.visited;
    updated = rhs.updated;
    showControlEdges = rhs.showControlEdges;
    currentPointerReference = rhs.currentPointerReference;
    isPotentialArray = rhs.isPotentialArray;
    ignorePtrRef = rhs.ignorePtrRef;
    isFragment = rhs.isFragment;
    expandCalls = rhs.expandCalls;
}

bool SliceProfile::operator==(const SliceProfile& rhs) const {
    if (this == &rhs) return true; // self-check protection

    if (declPosition != rhs.declPosition) return false;
    if (file != rhs.file) return false;
    if (function != rhs.function) return false;
    if (nameOfContainingClass != rhs.nameOfContainingClass) return false;
    if (containingNameSpaces != rhs.containingNameSpaces) return false;
    if (language != rhs.language) return false;
    if (checksum != rhs.checksum) return false;
    if (potentialAlias != rhs.potentialAlias) return false;
    if (dereferenced != rhs.dereferenced) return false;
    if (isGlobal != rhs.isGlobal) return false;
    if (classMemberVar != rhs.classMemberVar) return false;
    if (containsDeclaration != rhs.containsDeclaration) return false;
    if (isPointer != rhs.isPointer) return false;
    if (isReference != rhs.isReference) return false;
    if (variableName != rhs.variableName) return false;
    if (variableType != rhs.variableType) return false;
    if (definitions != rhs.definitions) return false;
    if (uses != rhs.uses) return false;
    if (dvars != rhs.dvars) return false;
    if (aliases != rhs.aliases) return false;
    if (controlEdges != rhs.controlEdges) return false;
    if (cfunctions != rhs.cfunctions) return false;
    if (endOfScope != rhs.endOfScope) return false;
    if (visited != rhs.visited) return false;
    if (updated != rhs.updated) return false;
    if (showControlEdges != rhs.showControlEdges) return false;
    if (currentPointerReference != rhs.currentPointerReference) return false;
    if (isPotentialArray != rhs.isPotentialArray) return false;
    if (ignorePtrRef != rhs.ignorePtrRef) return false;
    if (isFragment != rhs.isFragment) return false;
    if (expandCalls != rhs.expandCalls) return false;

    return true;
}
bool SliceProfile::operator!=(const SliceProfile& rhs) const {
    return !(*this == rhs);
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
    
    out << "    \"decl\":" << profile.declPosition.StartToString() << "," << std::endl;

    out << "    \"dependence\":[";
    first = true;
    for (auto dvar : profile.dvars) {
        if (!first) {
            out << ",";
        }
        out << "{\"" << dvar.first << "\":" << dvar.second.StartToString() << "}";
        first = false;
    }
    out << "]," << std::endl;

    out << "    \"aliases\":[";
    first = true;
    for (auto alias : profile.aliases) {
        if (!first) {
            out << ",";
        }
        out << "{\"" << alias.first << "\":" << alias.second.StartToString() << "}";
        first = false;
    }
    out << "]," << std::endl;

    out << "    \"calls\":[";
    first = true;
    for (auto cfunc : profile.cfunctions) {
        if (!first) {
            out << ",";
        }
        out << "{" <<
                    "\"functionName\":\"" << cfunc.functionName;
                    if (profile.expandCalls) {}
                    out << "\",\"parameter\":\"" << cfunc.parameterIndex;
                    out << "\",\"definitionPosition\":" << cfunc.funcPos.StartToString();
                    if (profile.expandCalls) {
                        out << ",\"endOfFunction\":" << cfunc.funcPos.EndToString();
                    }
                    out << ",\"invoke\":" << cfunc.invokePosition.StartToString() <<
                "}";
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
        out << use.StartToString();
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
        out << def.StartToString();
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
            out << "[" << edge.first.StartToString() <<
            "," << edge.second.StartToString() << "]";
            first = false;
        }
        out << "]" << std::endl;
    } else {
        out << "]" << std::endl;
    }

    return out;
}

void SliceProfile::merge(SliceProfile& other) {
    if (!other.isFragment) {
        std::cerr << "[!] SliceProfile::Merge argument should be a fragment, merge might not be correct.\n";
    }
    
    uses.insert(other.uses.begin(), other.uses.end());
    other.uses.clear();

    definitions.insert(other.definitions.begin(), other.definitions.end());
    other.definitions.clear();

    controlEdges.insert(other.controlEdges.begin(), other.controlEdges.end());
    other.controlEdges.clear();

    dvars.insert(other.dvars.begin(), other.dvars.end());
    other.dvars.clear();

    aliases.insert(other.aliases.begin(), other.aliases.end());
    other.aliases.clear();

    cfunctions.insert(other.cfunctions.begin(), other.cfunctions.end());
    other.cfunctions.clear();
}

bool SliceProfile::inScope(const SlicePosition& pos) {
    if (isGlobal || isFragment) {
        return file == pos.GetFileName();
    }

    // class member variables are tricky to scope effectively
    if (classMemberVar) {
        return true;
    }

    if (pos < declPosition) return false;
    if (endOfScope < pos) return false;
    return true;
}