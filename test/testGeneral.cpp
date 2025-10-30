#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"


/**
 * @section Basic Slicing Cases
 * Tests Slicing against basic source
 * Simple base-cases
 */

TEST_CASE( "General Test 1", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":[{"start":"2:9","end":"2:9"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 1", produced, expected) );
}

TEST_CASE( "General Test 2", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:13","end":"3:13"}],
        "definition":[{"start":"2:9","end":"2:9"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 2", produced, expected) );
}

TEST_CASE( "General Test 3", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":[{"start":"2:9","end":"2:9"},{"start":"3:5","end":"3:5"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 3", produced, expected) );
}

TEST_CASE( "General Test 4", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"bar","parameter":"1","definitionPosition":{},"invoke":{"start":"3:5","end":"3:10"}}],
        "use":[{"start":"3:9","end":"3:9"}],
        "definition":[{"start":"2:9","end":"2:9"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 4", produced, expected) );
}

TEST_CASE( "General Test 5", "[srcslice]" ) {
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
        "initial":{"start":"3:13","end":"3:13"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":[{"start":"3:13","end":"3:13"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 5", produced, expected) );
}

TEST_CASE( "General Test 6", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[{"b":{"start":"3:13","end":"3:13"}}],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:13","end":"3:13"}],
        "definition":[{"start":"2:9","end":"2:9"}]
    },
    "b-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":{"start":"3:9","end":"3:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":[{"start":"3:9","end":"3:9"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 6", produced, expected) );
}

TEST_CASE( "General Test 7", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:7","end":"3:7"}],
        "definition":[{"start":"2:9","end":"2:9"},{"start":"3:7","end":"3:7"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 7", produced, expected) );
}

TEST_CASE( "General Test 8", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:5","end":"3:5"}],
        "definition":[{"start":"2:9","end":"2:9"},{"start":"3:5","end":"3:5"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 8", produced, expected) );
}

TEST_CASE( "General Test 9", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:5","end":"3:5"}],
        "definition":[{"start":"2:9","end":"2:9"},{"start":"3:5","end":"3:5"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 9", produced, expected) );
}

TEST_CASE( "General Test 10", "[srcslice]" ) {
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
        "initial":{"start":"2:9","end":"2:9"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":[{"start":"2:9","end":"2:9"},{"start":"3:12","end":"3:12"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 10", produced, expected) );
}

TEST_CASE( "General Test 11", "[srcslice]" ) {
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
        "initial":{"start":"4:13","end":"4:13"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":[{"start":"4:13","end":"4:13"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 11", produced, expected) );
    std::cout << OK << " Passed!" << std::endl;
}




/**
 * @section Interprocedural Slicing Cases
 * Tests Interprocedural implementation against pass-by-value and pass-by-reference
 */

TEST_CASE( "General Test 12", "[srcslice]" ) {
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
        "initial":{"start":"6:9","end":"6:9"},
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":{"start":"2:1","end":"4:1"},"invoke":{"start":"7:5","end":"7:10"}}],
        "use":[{"start":"2:13","end":"2:13"},{"start":"3:12","end":"3:12"},{"start":"7:9","end":"7:9"}],
        "definition":[{"start":"6:9","end":"6:9"}]
    },
    "b-2-13":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"b",
        "initial":{"start":"2:13","end":"2:13"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:12","end":"3:12"}],
        "definition":[{"start":"2:13","end":"2:13"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 12", produced, expected) );
}

TEST_CASE( "General Test 13", "[srcslice]" ) {
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
        "initial":{"start":"6:9","end":"6:9"},
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":{"start":"2:1","end":"4:1"},"invoke":{"start":"7:5","end":"7:10"}}],
        "use":[{"start":"2:14","end":"2:14"},{"start":"3:12","end":"3:12"},{"start":"7:9","end":"7:9"}],
        "definition":[{"start":"6:9","end":"6:9"}]
    },
    "b-2-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int&",
        "name":"b",
        "initial":{"start":"2:14","end":"2:14"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:12","end":"3:12"}],
        "definition":[{"start":"2:14","end":"2:14"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 13", produced, expected) );
}

TEST_CASE( "General Test 14", "[srcslice]" ) {
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
        "initial":{"start":"6:9","end":"6:9"},
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":{"start":"2:1","end":"4:1"},"invoke":{"start":"7:5","end":"7:10"}}],
        "use":[{"start":"2:15","end":"2:15"},{"start":"3:5","end":"3:5"},{"start":"7:9","end":"7:9"}],
        "definition":[{"start":"3:5","end":"3:5"},{"start":"6:9","end":"6:9"}]
    },
    "b-2-15":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int&",
        "name":"b",
        "initial":{"start":"2:15","end":"2:15"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:5","end":"3:5"}],
        "definition":[{"start":"2:15","end":"2:15"},{"start":"3:5","end":"3:5"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 14", produced, expected) );
}

TEST_CASE( "General Test 15", "[srcslice]" ) {
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
        "initial":{"start":"6:9","end":"6:9"},
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":{"start":"2:1","end":"4:1"},"invoke":{"start":"7:5","end":"7:11"}}],
        "use":[{"start":"2:15","end":"2:15"},{"start":"3:14","end":"3:14"},{"start":"7:10","end":"7:10"}],
        "definition":[{"start":"6:9","end":"6:9"}]
    },
    "b-2-15":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int*",
        "name":"b",
        "initial":{"start":"2:15","end":"2:15"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:14","end":"3:14"}],
        "definition":[{"start":"2:15","end":"2:15"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 15", produced, expected) );
}

TEST_CASE( "General Test 16", "[srcslice]" ) {
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
        "initial":{"start":"6:9","end":"6:9"},
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":{"start":"2:1","end":"4:1"},"invoke":{"start":"7:5","end":"7:11"}}],
        "use":[{"start":"2:15","end":"2:15"},{"start":"3:6","end":"3:6"},{"start":"7:10","end":"7:10"}],
        "definition":[{"start":"6:9","end":"6:9"}]
    },
    "b-2-15":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int*",
        "name":"b",
        "initial":{"start":"2:15","end":"2:15"},
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[{"start":"3:6","end":"3:6"}],
        "definition":[{"start":"2:15","end":"2:15"}]
    }
    })"_json;

    REQUIRE( CompareJson("General Test 16", produced, expected) );
}