#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

/**
 * @section Toy Code focusing on simple/complex expressions and declaration statements
 */

TEST_CASE( TestName(), "[srcslice]" ) {
    std::cout << INFO << " Testing General Expressions" << std::endl;

        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b + c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b - c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b * c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b / c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b % c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a += b + c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:5"],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b += c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9","file.cpp:8:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"},{"b":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b -= c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9","file.cpp:8:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"},{"b":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b - c *= d;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9","file.cpp:8:13"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[{"a":"file.cpp:8:18"},{"c":"file.cpp:8:18"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:18"],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a -= b + c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:5"],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a *= b + c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:5"],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a /= b + c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:5"],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a %= b + c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:5"],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = (b + c);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = (b ^ c);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}


/**
 * @section Toy Code focusing on postfix and prefix incr/decr
 */


TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = ++a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:4:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15"],
        "definition":["file.cpp:3:9","file.cpp:4:15"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = --a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:4:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15"],
        "definition":["file.cpp:3:9","file.cpp:4:15"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = a++;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:4:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:13"],
        "definition":["file.cpp:3:9","file.cpp:4:13"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = a--;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:4:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:13"],
        "definition":["file.cpp:3:9","file.cpp:4:13"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = b - ++a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:15"],
        "definition":["file.cpp:3:9","file.cpp:6:15"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = b - --a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:15"],
        "definition":["file.cpp:3:9","file.cpp:6:15"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = b - a++;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:13"],
        "definition":["file.cpp:3:9","file.cpp:6:13"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = b - a--;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:13"],
        "definition":["file.cpp:3:9","file.cpp:6:13"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = b++ - a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:15"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:9"],
        "definition":["file.cpp:4:9","file.cpp:6:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = b-- - a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:15"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:9"],
        "definition":["file.cpp:4:9","file.cpp:6:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = ++b - a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:15"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:11"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:11"],
        "definition":["file.cpp:4:9","file.cpp:6:11"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    c = --b - a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"c":"file.cpp:6:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:15"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"c":"file.cpp:6:11"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:11"],
        "definition":["file.cpp:4:9","file.cpp:6:11"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}

/**
 * @section Logical Expressions
 * 
 * 
 * 
 */


TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b && c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b || c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = !b || c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:15"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b || !c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:15"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:15"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b | c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b & c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b != c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b == c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b < c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b <= c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b > c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b >= c;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = b > 4 ? c : d;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:9"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:17"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:17"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[{"a":"file.cpp:8:21"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:21"],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = (b > 4) ? c : d;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:19"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:19"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[{"a":"file.cpp:8:23"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:23"],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    a = (b > 4) ? c : d;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:8:5"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:8:10"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:10"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[{"a":"file.cpp:8:19"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:19"],
        "definition":["file.cpp:5:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[{"a":"file.cpp:8:23"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:23"],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}

/**
 * @section Conditional Expressions
 * 
 * 
 * 
 */

TEST_CASE( TestName(), "[srcslice]" ) {
    std::cout << INFO << " Testing Conditional Expressions" << std::endl;
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (a > b) {
        c = 4;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:9:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}



TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (a < b) {
        c = 4;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:9:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}



TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (a && b) {
        c = 4;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:9:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}



TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (a || b) {
        c = 4;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:9:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}



TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (a < b) {
        c = 4;
    } else
        d = 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:9:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9","file.cpp:11:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}



TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (a < b) {
        c = 4;
    } else if (c == 4)
        d = 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:9"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:13"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:10:16"],
        "definition":["file.cpp:5:9","file.cpp:9:9"]
    },
    "d-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"d",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:9","file.cpp:11:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}



TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    for (int i = 0; i < a; ++i) {
        b = i + 2;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:5:25"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9","file.cpp:6:9"]
    },
    "i-5-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:5:14",
        "dependence":[{"b":"file.cpp:6:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:21","file.cpp:5:30","file.cpp:6:13"],
        "definition":["file.cpp:5:14","file.cpp:5:30"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    while (a > b) {
        c = 4;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:6:12"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:16"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:7:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    do {
        c = 4;
    } while (a > b)
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:8:14"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:8:18"],
        "definition":["file.cpp:4:9"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:7:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    switch (a) {
        case 0:
            b = 4;
        break;
        default:
            c = 6;
        break;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:6:13","file.cpp:7:14"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9","file.cpp:8:13"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:11:13"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    switch (a < 4) {
        case true:
            b = 4;
        break;
        default:
            c = 6;
        break;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:6:13","file.cpp:7:14"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9","file.cpp:8:13"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:11:13"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int c = 0;
    switch (a < 4) {
        case true:
            b = 4;
        break;
        default:
            c = 6;
        break;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:6:13","file.cpp:7:14"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9","file.cpp:8:13"]
    },
    "c-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9","file.cpp:11:13"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}


/**
 * @section Expressions with Pointers
 * 
 * 
 * 
 */


TEST_CASE( TestName(), "[srcslice]" ) {
    std::cout << INFO << " Testing Expressions with Pointers" << std::endl;
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15"],
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
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int* p = &a;
    b += *p;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:6:11"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:15","file.cpp:6:11"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:5"],
        "definition":["file.cpp:4:9","file.cpp:6:5"]
    },
    "p-5-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:5:10",
        "dependence":[{"b":"file.cpp:6:11"}],
        "aliases":[{"a":"file.cpp:5:15"}],
        "calls":[],
        "use":["file.cpp:6:11"],
        "definition":["file.cpp:5:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    *p = 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15"],
        "definition":["file.cpp:3:9","file.cpp:5:6"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:6"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    *p += 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:6"],
        "definition":["file.cpp:3:9","file.cpp:5:6"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:6"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    (*p) += 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:7"],
        "definition":["file.cpp:3:9","file.cpp:5:7"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:7"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    *p -= 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:6"],
        "definition":["file.cpp:3:9","file.cpp:5:6"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:6"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    (*p) -= 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:7"],
        "definition":["file.cpp:3:9","file.cpp:5:7"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:7"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    ++(*p);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:9"],
        "definition":["file.cpp:3:9","file.cpp:5:9"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:9"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    --(*p);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:9"],
        "definition":["file.cpp:3:9","file.cpp:5:9"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:9"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    (*p)++;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:7"],
        "definition":["file.cpp:3:9","file.cpp:5:7"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:7"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    (*p)--;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:15","file.cpp:5:7"],
        "definition":["file.cpp:3:9","file.cpp:5:7"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:7"],
        "definition":["file.cpp:4:10"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    int b = *p - 2;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:5:14"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15","file.cpp:5:14"],
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
        "initial":"file.cpp:4:10",
        "dependence":[{"b":"file.cpp:5:14"}],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:14"],
        "definition":["file.cpp:4:10"]
    },
    "b-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    int b = 5 + *p;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:5:18"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15","file.cpp:5:18"],
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
        "initial":"file.cpp:4:10",
        "dependence":[{"b":"file.cpp:5:18"}],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:18"],
        "definition":["file.cpp:4:10"]
    },
    "b-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    int b = 5 + (*p)++;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:5:19"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15","file.cpp:5:19"],
        "definition":["file.cpp:3:9","file.cpp:5:19"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[{"b":"file.cpp:5:19"}],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:19"],
        "definition":["file.cpp:4:10"]
    },
    "b-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int* p = &a;
    int b = 5 + --(*p);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:5:21"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:15","file.cpp:5:21"],
        "definition":["file.cpp:3:9","file.cpp:5:21"]
    },
    "p-4-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:4:10",
        "dependence":[{"b":"file.cpp:5:21"}],
        "aliases":[{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":["file.cpp:5:21"],
        "definition":["file.cpp:4:10"]
    },
    "b-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int* p = &a;
    p = &b;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:5:15"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:10"],
        "definition":["file.cpp:4:9"]
    },
    "p-5-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:5:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:5:15"},{"b":"file.cpp:6:10"}],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:10","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int* p = &a;
    p = &b;
    *p += 5;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:5:15"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:10","file.cpp:7:6"],
        "definition":["file.cpp:4:9","file.cpp:7:6"]
    },
    "p-5-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:5:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:5:15"},{"b":"file.cpp:6:10"}],
        "calls":[],
        "use":["file.cpp:7:6"],
        "definition":["file.cpp:5:10","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    int b = 0;
    int* p = &a;
    p = &b;
    *p -= a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "dependence":[{"b":"file.cpp:7:11"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:15","file.cpp:7:11"],
        "definition":["file.cpp:3:9"]
    },
    "b-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:10","file.cpp:7:6"],
        "definition":["file.cpp:4:9","file.cpp:7:6"]
    },
    "p-5-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int*",
        "name":"p",
        "initial":"file.cpp:5:10",
        "dependence":[],
        "aliases":[{"a":"file.cpp:5:15"},{"b":"file.cpp:6:10"}],
        "calls":[],
        "use":["file.cpp:7:6"],
        "definition":["file.cpp:5:10","file.cpp:6:5"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 2;
    int* p = &a;
    int** mptr = &p;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "initial":"file.cpp:4:10",
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
        "initial":"file.cpp:5:11",
        "dependence":[],
        "aliases":[{"p":"file.cpp:5:19"},{"a":"file.cpp:4:15"}],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:11"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}


/**
 * @section Expressions with Arrays
 * 
 * 
 * 
 */


TEST_CASE( TestName(), "[srcslice]" ) {
std::cout << INFO << " Testing Expressions with C/C++ Static Arrays" << std::endl;
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[5] = {0,1,2,3,4};
    int i = 0;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[5] = {0,1,2,3,4};
    int i = 0;
    int b = a[i];
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[{"b":"file.cpp:5:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:13"],
        "definition":["file.cpp:3:9"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:15"],
        "definition":["file.cpp:4:9"]
    },
    "b-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[5] = {0,1,2,3,4};
    int i = 0;
    a[i] = 1;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:5:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:7"],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[5] = {0,1,2,3,4};
    int i = 0;
    a[i] += 1;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:5"],
        "definition":["file.cpp:3:9","file.cpp:5:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:7"],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[5] = {0,1,2,3,4};
    int i = 0;
    a[i] += i;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:5"],
        "definition":["file.cpp:3:9","file.cpp:5:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[{"a":"file.cpp:5:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:7","file.cpp:5:13"],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[5] = {0,1,2,3,4};
    int i = 0;
    int k = 0;
    a[i % k] = 4;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:6:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:7"],
        "definition":["file.cpp:4:9"]
    },
    "k-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"k",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:11"],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}


/**
 * @section Expressions with Multi-Dimensional Arrays
 * 
 * 
 * 
 */


TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[2][2] = {{1,2},{3,4}};
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[][]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[2][2] = {{1,2},{3,4}};
    int i = 0;
    int j = 0;
    a[i][j] = 10;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[][]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:6:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:7"],
        "definition":["file.cpp:4:9"]
    },
    "j-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"j",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:10"],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[2][2] = {{1,2},{3,4}};
    int i = 0;
    int j = 0;
    a[i][j] -= 10;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[][]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:5"],
        "definition":["file.cpp:3:9","file.cpp:6:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:7"],
        "definition":["file.cpp:4:9"]
    },
    "j-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"j",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:10"],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a[2][2] = {{1,2},{3,4}};
    int i = 0;
    int j = 0;
    a[i][2*j] -= 10;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int[][]",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:5"],
        "definition":["file.cpp:3:9","file.cpp:6:5"]
    },
    "i-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"i",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:7"],
        "definition":["file.cpp:4:9"]
    },
    "j-5-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"j",
        "initial":"file.cpp:5:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:12"],
        "definition":["file.cpp:5:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}

/**
 * @section Expressions with Function Calls
 * 
 * 
 * 
 */


TEST_CASE( TestName(), "[srcslice]" ) {
std::cout << INFO << " Testing Expressions with Function Calls" << std::endl;
        // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int a) {
    return a + 4;
}
int main() {
    int x = 0;
    int y = foo(x);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:13"]
    },
    "x-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:6:9",
        "dependence":[{"y":"file.cpp:7:17"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"file.cpp:2:1",
            "invoke":"file.cpp:7:13"
        }],
        "use":["file.cpp:2:13","file.cpp:3:12","file.cpp:7:17"],
        "definition":["file.cpp:6:9"]
    },
    "y-7-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:7:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:7:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int a, int c) {
    return a + 4 + c;
}
int main() {
    int x = 0;
    int k = 0;
    int y = foo(x,k);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:13"]
    },
    "c-2-20":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"c",
        "initial":"file.cpp:2:20",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:20"],
        "definition":["file.cpp:2:20"]
    },
    "k-7-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"k",
        "initial":"file.cpp:7:9",
        "dependence":[{"y":"file.cpp:8:19"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"2",
            "definitionPosition":"file.cpp:2:1",
            "invoke":"file.cpp:8:13"
        }],
        "use":["file.cpp:2:20","file.cpp:3:20","file.cpp:8:19"],
        "definition":["file.cpp:7:9"]
    },
    "x-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:6:9",
        "dependence":[{"y":"file.cpp:8:17"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"file.cpp:2:1",
            "invoke":"file.cpp:8:13"
        }],
        "use":["file.cpp:2:13","file.cpp:3:12","file.cpp:8:17"],
        "definition":["file.cpp:6:9"]
    },
    "y-8-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:8:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:8:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int x = 0;
    int y = foo(x);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "x-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:3:9",
        "dependence":[{"y":"file.cpp:4:17"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"",
            "invoke":"file.cpp:4:13"
        }],
        "use":["file.cpp:4:17"],
        "definition":["file.cpp:3:9"]
    },
    "y-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int x = 0;
    int y = foo(x + 4);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "x-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:3:9",
        "dependence":[{"y":"file.cpp:4:17"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"",
            "invoke":"file.cpp:4:13"
        }],
        "use":["file.cpp:4:17"],
        "definition":["file.cpp:3:9"]
    },
    "y-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int a) {
    return a + 4;
}
int main() {
    int x = 0;
    int y = foo(++x);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:13"]
    },
    "x-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:6:9",
        "dependence":[{"y":"file.cpp:7:19"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"file.cpp:2:1",
            "invoke":"file.cpp:7:13"
        }],
        "use":["file.cpp:2:13","file.cpp:3:12","file.cpp:7:19"],
        "definition":["file.cpp:6:9","file.cpp:7:19"]
    },
    "y-7-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:7:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:7:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int x = 0;
    int y = foo(x += 3, 7);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "x-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:3:9",
        "dependence":[{"y":"file.cpp:4:17"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"",
            "invoke":"file.cpp:4:13"
        }],
        "use":["file.cpp:4:17"],
        "definition":["file.cpp:3:9","file.cpp:4:17"]
    },
    "y-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int x = 0, z = 6;
    int y = foo(x += 3, z);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "x-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:3:9",
        "dependence":[{"y":"file.cpp:4:17"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"1",
            "definitionPosition":"",
            "invoke":"file.cpp:4:13"
        }],
        "use":["file.cpp:4:17"],
        "definition":["file.cpp:3:9","file.cpp:4:17"]
    },
    "z-3-16":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"z",
        "initial":"file.cpp:3:16",
        "dependence":[{"y":"file.cpp:4:25"}],
        "aliases":[],
        "calls":[{
            "functionName":"foo",
            "parameter":"2",
            "definitionPosition":"",
            "invoke":"file.cpp:4:13"
        }],
        "use":["file.cpp:4:25"],
        "definition":["file.cpp:3:16"]
    },
    "y-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int a) {
    return a + 4;
}
int bar(int b) {
    return b + 4;
}
int main() {
    int x = 0;
    int y = foo(bar(x));
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:13"]
    },
    "b-5-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"bar",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:5:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:12"],
        "definition":["file.cpp:5:13"]
    },
    "x-9-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:9:9",
        "dependence":[{"y":"file.cpp:10:21"}],
        "aliases":[],
        "calls":[{
            "functionName":"bar",
            "parameter":"1",
            "definitionPosition":"file.cpp:5:1",
            "invoke":"file.cpp:10:17"
        }],
        "use":["file.cpp:5:13","file.cpp:6:12","file.cpp:10:21"],
        "definition":["file.cpp:9:9"]
    },
    "y-10-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:10:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:10:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}


/**
 * @section C/C++ targeted expressions -> cout/cin/cerr
 * 
 * These expressions use the >> and << operators
 * these are not bitwise operations
 */


TEST_CASE( TestName(), "[srcslice]" ) {
std::cout << INFO << " Testing Expressions with C++ stdio" << std::endl;
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    std::cout << a << std::endl;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:18"],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    cout << a << std::endl;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:13"],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0, b = 0;
    std::cout << a << b << std::endl;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:18"],
        "definition":["file.cpp:3:9"]
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
        "use":["file.cpp:4:23"],
        "definition":["file.cpp:3:16"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    std::cerr << a << std::endl;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:18"],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    cerr << a << std::endl;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:13"],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0, b = 0;
    std::cerr << a << b << std::endl;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:4:18"],
        "definition":["file.cpp:3:9"]
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
        "use":["file.cpp:4:23"],
        "definition":["file.cpp:3:16"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    std::cin >> a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:4:17"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    cin >> a;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:4:12"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0, b = 0;
    std::cin >> a >> b;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":[],
        "definition":["file.cpp:3:9","file.cpp:4:17"]
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
        "use":[],
        "definition":["file.cpp:3:16","file.cpp:4:22"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}


/**
 * @section Expressions with Object usage
 * 
 * 
 * 
 */


TEST_CASE( TestName(), "[srcslice]" ) {
std::cout << INFO << " Testing Expressions with Objects" << std::endl;
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    Toy a;
    std::string s = a.name;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"Toy",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[{"s":"file.cpp:4:21"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:21"],
        "definition":["file.cpp:3:9"]
    },
    "s-4-17":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"std::string",
        "name":"s",
        "initial":"file.cpp:4:17",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:17"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    Toy* a;
    std::string s = a->name;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"Toy*",
        "name":"a",
        "initial":"file.cpp:3:10",
        "dependence":[{"s":"file.cpp:4:21"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:21"],
        "definition":["file.cpp:3:10"]
    },
    "s-4-17":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"std::string",
        "name":"s",
        "initial":"file.cpp:4:17",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:17"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}




TEST_CASE( TestName(), "[srcslice]" ) {
        // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    Toy* a;
    std::string s = (*a).name;
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-10":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"Toy*",
        "name":"a",
        "initial":"file.cpp:3:10",
        "dependence":[{"s":"file.cpp:4:23"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:4:23"],
        "definition":["file.cpp:3:10"]
    },
    "s-4-17":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"std::string",
        "name":"s",
        "initial":"file.cpp:4:17",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:17"]
    }
    })"_json;

    REQUIRE( CompareJson(sourceCode, TestName(false), produced, expected) );
}