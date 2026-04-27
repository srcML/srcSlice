// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcsliceprofile.hpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

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
            std::set<FunctionCallData> cFunc = {},
            std::set<std::pair<std::string, SlicePosition>> dv = {},
            std::set<std::pair<SlicePosition, SlicePosition>> edges = {},
            bool containsDecl = false, bool visit = false);

    SliceProfile(const SliceProfile& rhs);

    bool operator==(const SliceProfile& rhs) const;
    bool operator!=(const SliceProfile& rhs) const;

    SlicePosition declPosition;
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

    std::set<SlicePosition> definitions;
    std::set<SlicePosition> uses;

    std::set<std::pair<std::string, SlicePosition>> dvars;
    std::set<std::pair<std::string, SlicePosition>> aliases;
    std::set<FunctionCallData> cfunctions;

    SlicePosition endOfScope;

    void merge(SliceProfile& other);
    bool inScope(const SlicePosition& pos);

    std::set<std::pair<SlicePosition, SlicePosition>> controlEdges;

    bool visited = false;
    bool updated = false;
    bool partial = false;
    bool showControlEdges = false;

    std::string currentPointerReference;
    bool ignorePtrRef = false;
    bool isPotentialArray = false;

    bool isFragment = false;

    friend std::ostream& operator<<(std::ostream& out, SliceProfile& profile);
};

#endif