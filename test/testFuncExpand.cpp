// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file testFuncExpand.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

TestArg data = {
    false, // control edges
    true // cfunc expand
};

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    ResetCount();
    std::cout << INFO << " Testing Cfunction Expansion" << std::endl;

    // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int x) {
    return x + 2;
}
int main() {
    int a = 0;
    int b = foo(a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode, data));

    json expected = R"({
    "x-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"x",
        "decl":"file.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:13"]
    },
    "a-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "decl":"file.cpp:6:9",
        "dependence":[{"b":"file.cpp:7:17"}],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","endOfFunction":"file.cpp:4:1","invoke":"file.cpp:7:13"}],
        "use":["file.cpp:2:13","file.cpp:3:12","file.cpp:7:17"],
        "definition":["file.cpp:6:9"]
    },
    "b-7-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "decl":"file.cpp:7:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:7:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
class Remer {
public:
    Remer(){}
    int foo(int x);
};
int Remer::foo(int x) {
    return x + 2;
}
int main() {
    Remer r;
    int a = 10;
    int b = r.foo(a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode, data));

    json expected = R"({
    "x-7-20":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"Remer::foo",
        "type":"int",
        "name":"x",
        "decl":"file.cpp:7:20",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:12"],
        "definition":["file.cpp:7:20"]
    },
    "x-5-17":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Remer",
        "function":"foo",
        "type":"int",
        "name":"x",
        "decl":"file.cpp:5:17",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:17"]
    },
    "r-11-11":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"Remer",
        "name":"r",
        "decl":"file.cpp:11:11",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:13:13"],
        "definition":["file.cpp:11:11"]
    },
    "a-12-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "decl":"file.cpp:12:9",
        "dependence":[{"b":"file.cpp:13:19"}],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:7:1","endOfFunction":"file.cpp:9:1","invoke":"file.cpp:13:13"}],
        "use":["file.cpp:7:20","file.cpp:8:12","file.cpp:13:19"],
        "definition":["file.cpp:12:9"]
    },
    "b-13-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "decl":"file.cpp:13:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:13:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
    
    std::cout << OK << " Passed!" << std::endl;
}