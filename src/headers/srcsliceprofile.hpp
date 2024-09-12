#ifndef SRCSLICEPROFILE
#define SRCSLICEPROFILE

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <string>

class SliceProfile {
public:
    SliceProfile() : index(0), containsDeclaration(false), potentialAlias(false), dereferenced(false),
                     isGlobal(false) { isPointer = false; isReference = false; }

    SliceProfile(
            std::string name, int line, bool alias = 0, bool global = 0,
            std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {},
            std::vector<std::pair<std::string, std::pair<std::string, std::string>>> cFunc = {},
            std::set<std::pair<std::string, unsigned int>> dv = {}, bool containsDecl = false,
            std::set<std::pair<int, int>> edges = {}, bool visit = false) :
            variableName(name), lineNumber(line), potentialAlias(alias),
            isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc),
            dvars(dv), containsDeclaration(containsDecl), controlEdges(edges),
            visited(visit) {
        dereferenced = false;
        isPointer = false;
        isReference = false;
    }

    unsigned int index;
    int lineNumber;
    std::string file;
    std::string function;
    std::string nameOfContainingClass;
    bool potentialAlias;
    bool dereferenced;

    bool isGlobal;
    bool containsDeclaration;

    bool isPointer = false, isReference = false;

    std::string variableName;
    std::string variableType;
    std::unordered_set<std::string> memberVariables;

    std::set<unsigned int> definitions;
    std::set<unsigned int> uses;

    std::set<std::pair<std::string, unsigned int>> dvars;
    std::set<std::pair<std::string, unsigned int>> aliases;

    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> cfunctions;

    std::set<std::pair<int, int>> controlEdges;

    bool visited;

    bool returnUsesInserted = false;
    bool conditionalUsesInserted = false;
    bool conditionalDefsInserted = false;

    friend std::ostream& operator<<(std::ostream& out, SliceProfile& profile) {
        out << "    \"file\":\"" << profile.file << "\"," << std::endl;
        out << "    \"class\":\"" << profile.nameOfContainingClass << "\"," << std::endl;
        out << "    \"function\":\"" << profile.function << "\"," << std::endl;
        out << "    \"type\":\"" << profile.variableType << "\"," << std::endl;
        out << "    \"name\":\"" << profile.variableName << "\"," << std::endl;

        out << "    \"dependentVariables\": [ ";
        for (auto dvar : profile.dvars) {
            if (dvar != *(--profile.dvars.end()))
                out << "{ \"" << dvar.first << "\": " << dvar.second << " },";
            else
                out << "{ \"" << dvar.first << "\": " << dvar.second << " }";
        }
        out << " ]," << std::endl;

        out << "    \"aliases\": [ ";
        for (auto alias : profile.aliases) {
            if (alias != *(--profile.aliases.end()))
                out << "{ \"" << alias.first << "\": " << alias.second << " },";
            else
                out << "{ \"" << alias.first << "\": " << alias.second << " }";
        }
        out << " ]," << std::endl;

        out << "    \"calledFunctions\": [ ";
        for (auto cfunc : profile.cfunctions) {
            if (cfunc != *(--profile.cfunctions.end()))
                out << "{\"functionName\": \"" << cfunc.first.substr(0, cfunc.first.find('-')) << "\", \"parameterNumber\": \"" << cfunc.second.first << "\", \"definitionLine\": \"" << cfunc.second.second << "\"}, ";
            else
                out << "{\"functionName\": \"" << cfunc.first.substr(0, cfunc.first.find('-')) << "\", \"parameterNumber\": \"" << cfunc.second.first << "\", \"definitionLine\": \"" << cfunc.second.second << "\"}";
        }
        out << " ]," << std::endl;

        out << "    \"use\": [ ";
        for (auto use : profile.uses) {
            if (use != *(--profile.uses.end()))
                out << use << ", ";
            else
                out << use;
        }
        out << " ]," << std::endl;

        out << "    \"definition\": [ ";
        for (auto def : profile.definitions) {
            if (def != *(--profile.definitions.end()))
                out << def << ", ";
            else
                out << def;
        }
        out << " ]" << std::endl;

        // out << "    \"controlEdges\": [ ";
        // for (auto edge : profile.controlEdges) {
        //     if (edge != *(--profile.controlEdges.end()))
        //         out << "[" << edge.first << ", " << edge.second << "], ";
        //     else
        //         out << "[" << edge.first << ", " << edge.second << "]";
        // }
        // out << " ]" << std::endl;

        return out;
    }
};

#endif