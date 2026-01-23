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
#include <iterator>
#include <srcslicecollection.hpp> // contains FunctionCallData and other items

class SliceProfile {
public:
    SliceProfile();

    SliceProfile(
            std::string name, SlicePosition line, bool alias = 0, bool global = 0,
            std::set<SlicePosition> aDef = {}, std::set<SlicePosition> aUse = {},
            std::vector<FunctionCallData> cFunc = {},
            std::vector<std::pair<std::string, SlicePosition>> dv = {},
            std::set<std::pair<SlicePosition, SlicePosition>> edges = {},
            bool containsDecl = false, bool visit = false);

    SliceProfile(const SliceProfile& rhs);

    bool operator==(const SliceProfile& rhs) const;
    bool operator!=(const SliceProfile& rhs) const;

    SlicePosition initialPosition;
    std::string file;
    std::string function;
    std::string nameOfContainingClass;
    std::vector<std::string> containingNameSpaces;
    std::string language;
    std::string checksum;
    bool potentialAlias = false;
    bool dereferenced = false;

    bool isGlobal = false;
    bool classMemberVar = false;
    bool containsDeclaration = false;

    bool isPointer = false;
    bool isReference = false;

    std::string variableName;
    std::string variableType;
    std::unordered_set<std::string> memberVariables;

    std::set<SlicePosition> definitions;
    std::set<SlicePosition> uses;

    std::vector<std::pair<std::string, SlicePosition>> dvars;
    // Insert a Dependent Variable entry with a given LHS var name and its position
    void insertDvar(std::string name, SlicePosition& sp);

    std::vector<std::pair<std::string, SlicePosition>> aliases;
    // Insert an Alias entry with a given pointer-alias name and its position
    void insertAlias(std::string name, SlicePosition& sp);
    
    std::vector<FunctionCallData> cfunctions;
    // Insert a Function Call entry with a given Function Call-Data
    void insertCfunction(FunctionCallData& fcd);

    std::set<std::pair<SlicePosition, SlicePosition>> controlEdges;

    bool visited = false;
    bool updated = false;
    bool showControlEdges = false;

    std::string currentPointerReference;
    bool ignorePtrRef = false;
    bool isPotentialArray = false;

    friend std::ostream& operator<<(std::ostream& out, SliceProfile& profile);
};

#endif