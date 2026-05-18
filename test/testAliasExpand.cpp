// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file testAliasExpand.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

TestArg data = {
    false,  // control edges
    false,  // cfunc expand
    true    // alias expand
};

TEST_CASE( TestName("Alias Expansion Test"), "[srcslice]" ) {
    ResetCount();
    std::cout << INFO << " Testing Alias Expansion" << std::endl;

    std::string sourceCode = R"(
int main() {
    int a = 2;
    int* p = &a;
    int** mptr = &p;
}
)";

    json produced = json::parse(FetchSlices(sourceCode, data));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "decl":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15","file.cpp:5:19"],
        "definition":["file.cpp:3:9"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "decl":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:19"],
        "definition":["file.cpp:4:10"]
    },
    "mptr-5-11":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int**",
        "name":"mptr",
        "decl":"file.cpp:5:11",
        "dependence":[],
        "aliases":[{"p":"file.cpp:5:19"},{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:11"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}