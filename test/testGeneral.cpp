#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"


/**
 * @section Basic Slicing Cases
 * Tests Slicing against basic source
 * Simple base-cases
 */

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    ResetCount();
    std::cout << INFO << " Testing Basic Slicing" << std::endl;

    std::string sourceCode = "int main() {\n"
                             "    int a;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:2:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a;\n"
                             "    cout << a;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:13"],
        "definition":["file.cpp:2:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a;\n"
                             "    a = 5;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:2:9","file.cpp:3:5"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a;\n"
                             "    bar(a);\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"bar","parameter":"1","definitionPosition":"","invoke":"file.cpp:3:5"}],
        "use":["file.cpp:3:9"],
        "definition":["file.cpp:2:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "class Fuzz {\n"
                             "    Fuzz() {\n"
                             "        int a = 2;\n"
                             "    }\n"
                             "};";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-3-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Fuzz",
        "function":"Fuzz",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:13"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a;\n"
                             "    int b = a;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[{"b":"file.cpp:3:13"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:13"],
        "definition":["file.cpp:2:9"]
    },
    "b-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a = 5;\n"
                             "    ++a;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:7"],
        "definition":["file.cpp:2:9","file.cpp:3:7"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a = 5;\n"
                             "    a--;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:5"],
        "definition":["file.cpp:2:9","file.cpp:3:5"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a = 5;\n"
                             "    a += 5;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:5"],
        "definition":["file.cpp:2:9","file.cpp:3:5"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "    int a;\n"
                             "    cin >> a;\n"
                             "}";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-2-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:2:9","file.cpp:3:12"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = R"(
namespace mynamespace {
    void jar() {
        int a = 10;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-4-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":["mynamespace"],
        "class":"",
        "function":"jar",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:4:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:13"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}




/**
 * @section Interprocedural Slicing Cases
 * Tests Interprocedural implementation against pass-by-value and pass-by-reference
 */

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::cout << INFO << " Testing Interprocedural Slicing" << std::endl;

    // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int b) {
    return b + 5;
}
int main() {
    int a;
    foo(a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"file.cpp:7:5"}],
        "use":["file.cpp:2:13","file.cpp:3:12","file.cpp:7:9"],
        "definition":["file.cpp:6:9"]
    },
    "b-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:13"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int foo(int& b) {
    return b + 5;
}
int main() {
    int a;
    foo(a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"file.cpp:7:5"}],
        "use":["file.cpp:2:14","file.cpp:3:12","file.cpp:7:9"],
        "definition":["file.cpp:6:9"]
    },
    "b-2-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int&",
        "name":"b",
        "initial":"file.cpp:2:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:12"],
        "definition":["file.cpp:2:14"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
void foo(int& b) {
    b += 5;
}
int main() {
    int a;
    foo(a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"file.cpp:7:5"}],
        "use":["file.cpp:2:15","file.cpp:3:5","file.cpp:7:9"],
        "definition":["file.cpp:3:5","file.cpp:6:9"]
    },
    "b-2-15":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int&",
        "name":"b",
        "initial":"file.cpp:2:15",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:5"],
        "definition":["file.cpp:2:15","file.cpp:3:5"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
void foo(int* b) {
    cout << *b;
}
int main() {
    int a;
    foo(&a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "a-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"file.cpp:7:5"}],
        "use":["file.cpp:2:15","file.cpp:3:14","file.cpp:7:10"],
        "definition":["file.cpp:6:9"]
    },
    "b-2-15":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int*",
        "name":"b",
        "initial":"file.cpp:2:15",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:14"],
        "definition":["file.cpp:2:15"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
void foo(int* b) {
    *b = 5;
}
int main() {
    int a = 1;
    foo(&a);
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

    // LIMITATION - No current way to use dereference-chain info calcuated during function processing during interprocedural
    json expected = R"({
    "a-6-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:6:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"file.cpp:7:5"}],
        "use":["file.cpp:2:15","file.cpp:3:6","file.cpp:7:10"],
        "definition":["file.cpp:6:9"]
    },
    "b-2-15":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int*",
        "name":"b",
        "initial":"file.cpp:2:15",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:3:6"],
        "definition":["file.cpp:2:15"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}




/**
 * @section Conditional Slicing Cases
 * Tests Slicing basic conditionals
 */

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::cout << INFO << " Testing Conditional Slicing" << std::endl;

    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    for (int i = 0; i < 10; ++i) {
        std::cout << i << std::endl;
    }
}
)";

    json produced = json::parse(FetchSlices(sourceCode));

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
        "use":["file.cpp:3:21","file.cpp:3:31","file.cpp:4:22"],
        "definition":["file.cpp:3:14","file.cpp:3:31"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = R"(
int main() {
    int a = 0;
    if (a < 10) { cout << a << endl; }
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
        "use":["file.cpp:4:9","file.cpp:4:27"],
        "definition":["file.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = R"(
int main() {
    int a = 0;
    while (a < 10) { ++a; }
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
        "use":["file.cpp:4:12","file.cpp:4:24"],
        "definition":["file.cpp:3:9","file.cpp:4:24"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = R"(
int main() {
    int a = 0;
    do { ++a; } while (a < 10);
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
        "use":["file.cpp:4:12","file.cpp:4:24"],
        "definition":["file.cpp:3:9","file.cpp:4:12"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::string sourceCode = R"(
int main() {
    int a = 0;
    switch (a) {
        case 0:
            cout << a << endl;
        break;
        default:
            a = -1;
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
        "use":["file.cpp:4:13","file.cpp:5:14","file.cpp:6:21"],
        "definition":["file.cpp:3:9","file.cpp:9:13"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}




/**
 * @section Basic Slicing Cases
 * Tests Slicing against various constructs
 * such as try-catch, anonymous blocks, throws, ...
 */

 TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::cout << INFO << " Testing Class Slicing" << std::endl;

    // Raw-Strings C++11
    std::string sourceCode = R"(
class Item {
public:
    Item(): name("") {}
    Item(std::string name_) {
        name = name_;
    }
    Item(const Item& rhs) {
        name = rhs.name;
    }
private:
    std::string name;
};
)";

    json produced = json::parse(FetchSlices(sourceCode));

    json expected = R"({
    "name-12-17":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Item",
        "function":"",
        "type":"std::string",
        "name":"name",
        "initial":"file.cpp:12:17",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:4:13","file.cpp:6:9","file.cpp:9:9","file.cpp:12:17"]
    },
    "name_-5-22":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Item",
        "function":"Item",
        "type":"std::string",
        "name":"name_",
        "initial":"file.cpp:5:22",
        "dependence":[{"name":"file.cpp:6:16"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:6:16"],
        "definition":["file.cpp:5:22"]
    },
    "rhs-8-22":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Item",
        "function":"Item",
        "type":"Item&",
        "name":"rhs",
        "initial":"file.cpp:8:22",
        "dependence":[{"name":"file.cpp:9:16"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:9:16"],
        "definition":["file.cpp:8:22"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}




/**
 * @section Basic Slicing Cases
 * Tests Slicing against basic C++ Classes
 */

 TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    std::cout << INFO << " Testing Construct Slicing" << std::endl;

    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    {
        cout << a;
        int b = (a % 3 == 1) ? 4 : -2;
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
        "dependence":[{"b":"file.cpp:6:18"}],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:17","file.cpp:6:18"],
        "definition":["file.cpp:3:9"]
    },
    "b-6-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file.cpp:6:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:6:13"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}

TEST_CASE( TestName("General Test"), "[srcslice]" ) {
    // Raw-Strings C++11
    std::string sourceCode = R"(
int main() {
    int a = 0;
    {
        int a = 0;
        a = 5;
    }
    a = 10;
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
    "a-5-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:5:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:5:13","file.cpp:6:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson(sourceCode, testName, produced, expected) );
}