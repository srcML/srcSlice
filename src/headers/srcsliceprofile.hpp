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
#include <srcslicecollection.hpp> // contains FunctionCallData and other items

class SliceProfile {
public:
    SliceProfile();

    SliceProfile(
            std::string name, int line, bool alias = 0, bool global = 0,
            std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {},
            std::set<FunctionCallData> cFunc = {},
            std::set<std::pair<std::string, unsigned int>> dv = {}, std::set<std::pair<int, int>> edges = {},
            bool containsDecl = false, bool visit = false);

    SliceProfile(const SliceProfile& rhs);

    int lineNumber;
    std::string file;
    std::string function;
    std::string nameOfContainingClass;
    std::vector<std::string> containingNameSpaces;
    std::string language;
    std::string checksum;
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
    std::set<std::pair<int, int>> controlEdges;

    std::set<FunctionCallData> cfunctions;

    bool visited;

    bool returnUsesInserted = false;
    bool conditionalUsesInserted = false;
    bool conditionalDefsInserted = false;
    bool showControlEdges = false;

    std::string currentPointerReference;
    bool ignorePtrRef = false;
    bool isPotentialArray = false;

    friend std::ostream& operator<<(std::ostream& out, SliceProfile& profile);
};

#endif