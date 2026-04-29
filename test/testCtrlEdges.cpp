// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file testCtrlEdges.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

TestArg data = {
    true, // control edges
    false // cfunc expand
};

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    ResetCount();
    std::cout << INFO << " Testing Simple Flows" << std::endl;

    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a % 2 == 0) {
        cout << a << endl;
    } else {
        a = 13;
    }
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
        "use":["file.cpp:4:9","file.cpp:5:17"],
        "definition":["file.cpp:3:9","file.cpp:7:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:17"],
            ["file.cpp:4:9","file.cpp:7:9"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a % 2 == 0) {
        cout << a << endl;
    } else if (a > 25) {
        a = 13;
    }
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
        "use":["file.cpp:4:9","file.cpp:5:17","file.cpp:6:16"],
        "definition":["file.cpp:3:9","file.cpp:7:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:17"],
            ["file.cpp:4:9","file.cpp:6:16"],
            ["file.cpp:6:16","file.cpp:7:9"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a % 2 == 0) {
        cout << a << endl;
    }
    a = 13;
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
        "use":["file.cpp:4:9","file.cpp:5:17"],
        "definition":["file.cpp:3:9","file.cpp:7:5"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:17"],
            ["file.cpp:4:9","file.cpp:7:5"],
            ["file.cpp:5:17","file.cpp:7:5"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a % 2 == 0) {
        cout << a << endl;
    } else if (a > 25) {
        a = 13;
    }
    cout << a << endl;
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
        "use":["file.cpp:4:9","file.cpp:5:17","file.cpp:6:16","file.cpp:9:13"],
        "definition":["file.cpp:3:9","file.cpp:7:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:17"],
            ["file.cpp:4:9","file.cpp:6:16"],
            ["file.cpp:5:17","file.cpp:9:13"],
            ["file.cpp:6:16","file.cpp:7:9"],
            ["file.cpp:6:16","file.cpp:9:13"],
            ["file.cpp:7:9","file.cpp:9:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a % 2 == 0) {
        cout << a << endl;
    } else {
        a = 13;
    }
    cout << a << endl;
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
        "use":["file.cpp:4:9","file.cpp:5:17","file.cpp:9:13"],
        "definition":["file.cpp:3:9","file.cpp:7:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:17"],
            ["file.cpp:4:9","file.cpp:7:9"],
            ["file.cpp:5:17","file.cpp:9:13"],
            ["file.cpp:7:9","file.cpp:9:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a < 1337) {
        if (a % 19 < 8) {
            cout << a << endl;
        }
        a = 3425;
    }
    cout << a << endl;
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
        "use":["file.cpp:4:9","file.cpp:5:13","file.cpp:6:21","file.cpp:10:13"],
        "definition":["file.cpp:3:9","file.cpp:8:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:13"],
            ["file.cpp:5:13","file.cpp:6:21"],
            ["file.cpp:6:21","file.cpp:8:9"],
            ["file.cpp:5:13","file.cpp:8:9"],
            ["file.cpp:8:9","file.cpp:10:13"],
            ["file.cpp:4:9","file.cpp:10:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a < 1337) {
        if (a % 19 < 8) {
            cout << a << endl;
        }
        // a = 3425;
    }
    cout << a << endl;
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
        "use":["file.cpp:4:9","file.cpp:5:13","file.cpp:6:21","file.cpp:10:13"],
        "definition":["file.cpp:3:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:13"],
            ["file.cpp:5:13","file.cpp:6:21"],
            ["file.cpp:6:21","file.cpp:10:13"],
            ["file.cpp:5:13","file.cpp:10:13"],
            ["file.cpp:4:9","file.cpp:10:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a < 1337) {
        if (a % 19 < 8) {
            cout << a << endl;
        } else { a = 24; }
        a = 3425;
    }
    cout << a << endl;
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
        "use":["file.cpp:4:9","file.cpp:5:13","file.cpp:6:21","file.cpp:10:13"],
        "definition":["file.cpp:3:9","file.cpp:7:18","file.cpp:8:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:13"],
            ["file.cpp:5:13","file.cpp:6:21"],
            ["file.cpp:5:13","file.cpp:7:18"],
            ["file.cpp:6:21","file.cpp:8:9"],
            ["file.cpp:7:18","file.cpp:8:9"],
            ["file.cpp:8:9","file.cpp:10:13"],
            ["file.cpp:4:9","file.cpp:10:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a < 1337) {
        if (a % 19 < 8) {
            cout << a << endl;
        } else if (a > 13) { a = 24; }
        a = 3425;
    }
    cout << a << endl;
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
        "use":["file.cpp:4:9","file.cpp:5:13","file.cpp:6:21","file.cpp:7:20","file.cpp:10:13"],
        "definition":["file.cpp:3:9","file.cpp:7:30","file.cpp:8:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:13"],
            ["file.cpp:5:13","file.cpp:6:21"],
            ["file.cpp:5:13","file.cpp:7:20"],
            ["file.cpp:6:21","file.cpp:8:9"],
            ["file.cpp:7:20","file.cpp:7:30"],
            ["file.cpp:7:20","file.cpp:8:9"],
            ["file.cpp:7:30","file.cpp:8:9"],
            ["file.cpp:8:9","file.cpp:10:13"],
            ["file.cpp:4:9","file.cpp:10:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0, b = 0;
    if (a % 2 == 0) {
        cout << a << endl;
    } else if (b > 25) {
        a = 13;
    }
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
        "use":["file.cpp:4:9","file.cpp:5:17"],
        "definition":["file.cpp:3:9","file.cpp:7:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:9"],
            ["file.cpp:4:9","file.cpp:5:17"],
            ["file.cpp:4:9","file.cpp:7:9"]
        ]
    },
    "b-3-16":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "decl":"file.cpp:3:16",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:16"],
        "definition":["file.cpp:3:16"],
        "controlEdges":[
            ["file.cpp:3:16","file.cpp:6:16"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    while (a < 10) {
        if (a % 2 == 0) {
            cout << a << endl;
        }
    }
    cout << a << endl;
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
        "use":["file.cpp:4:12","file.cpp:5:13","file.cpp:6:21","file.cpp:9:13"],
        "definition":["file.cpp:3:9"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:12"],
            ["file.cpp:4:12","file.cpp:5:13"],
            ["file.cpp:4:12","file.cpp:9:13"],
            ["file.cpp:5:13","file.cpp:6:21"],
            ["file.cpp:5:13","file.cpp:9:13"],
            ["file.cpp:6:21","file.cpp:9:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    for (int i = 0; i < 5; ++i) {
        for (int k = 0; k < 5; ++k) {
            cout << i+k << endl;
        }
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode, data));

    json expected = R"({
    "i-3-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "decl":"file.cpp:3:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:21","file.cpp:3:30","file.cpp:5:21"],
        "definition":["file.cpp:3:14","file.cpp:3:30"],
        "controlEdges":[
            ["file.cpp:3:14","file.cpp:3:21"],
            ["file.cpp:3:21","file.cpp:3:30"],
            ["file.cpp:3:30","file.cpp:5:21"]
        ]
    },
    "k-4-18":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"k",
        "decl":"file.cpp:4:18",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:25","file.cpp:4:34","file.cpp:5:23"],
        "definition":["file.cpp:4:18","file.cpp:4:34"],
        "controlEdges":[
            ["file.cpp:4:18","file.cpp:4:25"],
            ["file.cpp:4:25","file.cpp:4:34"],
            ["file.cpp:4:34","file.cpp:5:23"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    for (int i = 0; i < 5; ++i) {
        for (int k = 0; k < 5; ++k) {
            cout << i+k << endl;
        }
        cout << i << endl;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode, data));

    json expected = R"({
    "i-3-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "decl":"file.cpp:3:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:21","file.cpp:3:30","file.cpp:5:21","file.cpp:7:17"],
        "definition":["file.cpp:3:14","file.cpp:3:30"],
        "controlEdges":[
            ["file.cpp:3:14","file.cpp:3:21"],
            ["file.cpp:3:21","file.cpp:3:30"],
            ["file.cpp:3:30","file.cpp:5:21"],
            ["file.cpp:5:21","file.cpp:7:17"]
        ]
    },
    "k-4-18":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"k",
        "decl":"file.cpp:4:18",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:25","file.cpp:4:34","file.cpp:5:23"],
        "definition":["file.cpp:4:18","file.cpp:4:34"],
        "controlEdges":[
            ["file.cpp:4:18","file.cpp:4:25"],
            ["file.cpp:4:25","file.cpp:4:34"],
            ["file.cpp:4:34","file.cpp:5:23"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("Control-Edge Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    do { cout << a; ++a } while (a<5);
    cout << a;
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
        "use":["file.cpp:4:18","file.cpp:4:23","file.cpp:4:34","file.cpp:5:13"],
        "definition":["file.cpp:3:9","file.cpp:4:23"],
        "controlEdges":[
            ["file.cpp:3:9","file.cpp:4:18"],
            ["file.cpp:4:18","file.cpp:4:23"],
            ["file.cpp:4:23","file.cpp:4:34"],
            ["file.cpp:4:34","file.cpp:5:13"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    std::cout << INFO << " Testing Multi-File Flows" << std::endl;
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
void foo(int a) {
    a = 5;
}
)",
R"(
int main() {
    int b = 0;
    foo(b);
}
)"
    }, {"file.cpp","/utils/file2.cpp"}, data));

    json expected = R"({
    "a-2-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "decl":"file.cpp:2:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:2:14","file.cpp:3:5"],
        "controlEdges":[
            ["file.cpp:2:14","file.cpp:3:5"]
        ]
    },
    "b-3-9":{
        "file":"/utils/file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "decl":"/utils/file2.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"/utils/file2.cpp:4:5"}],
        "use":["/utils/file2.cpp:4:9","file.cpp:2:14","file.cpp:3:5"],
        "definition":["/utils/file2.cpp:3:9"],
        "controlEdges":[
            ["/utils/file2.cpp:3:9","/utils/file2.cpp:4:9"],
            ["file.cpp:2:14","file.cpp:3:5"]
        ]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}

/**
 * @todo testing against switches
 */