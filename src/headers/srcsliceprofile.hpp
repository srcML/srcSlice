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
            std::string name, SlicePosition line, bool alias = 0, bool global = 0,
            std::set<SlicePosition> aDef = {}, std::set<SlicePosition> aUse = {},
            std::set<FunctionCallData> cFunc = {},
            std::set<std::pair<std::string, SlicePosition>> dv = {},
            std::set<std::pair<SlicePosition, SlicePosition>> edges = {},
            bool containsDecl = false, bool visit = false);

    SliceProfile(const SliceProfile& rhs);

    SlicePosition initialPosition;
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

    std::set<SlicePosition> definitions;
    std::set<SlicePosition> uses;

    std::set<std::pair<std::string, SlicePosition>> dvars;
    std::set<std::pair<std::string, SlicePosition>> aliases;
    std::set<std::pair<SlicePosition, SlicePosition>> controlEdges;

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