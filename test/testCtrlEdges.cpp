#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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
        "initial":"file.cpp:3:16",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "i-3-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:3:14",
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
        "initial":"file.cpp:4:18",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "i-3-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:3:14",
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
        "initial":"file.cpp:4:18",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
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

    json produced = json::parse(FetchSlices(sourceCode, true));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
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

    REQUIRE( CompareJson(sourceCode, GetTestName(), produced, expected, true) );
}

/**
 * @todo testing against switches
 */