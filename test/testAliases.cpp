#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool testStatus = false;

TEST_CASE( "Aliases Test 1", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int a = 3;\n"
                "    int* x = &a;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":3}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 1", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 2", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int a = 3;\n"
                "    int b = 13;\n"
                "    int* x = &a;\n"
                "    x = &b;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":4},{\"b\":5}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[4,5]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 2", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 3", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int a = 3;\n"
                "    int b = 13;\n"
                "    int* x = &a;\n"
                "    x = &b;\n"
                "    int* y = x;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"y\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":4},{\"b\":5},{\"x\":6}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[6]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":4},{\"b\":5}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[6],\n"
            "    \"definition\":[4,5]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 3", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 4", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int a = 3;\n"
                "    int* x = &a;\n"
                "    int** p = &x;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int**\",\n"
            "    \"name\":\"p\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":3},{\"x\":4}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[4]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":3}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 4", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 5", "[srcslice]" ) {
    sourceCode = "void bar(int a) {\n"
                "    int* x = &a;\n"
                "    int** p = &x;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int**\",\n"
            "    \"name\":\"p\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":2},{\"x\":3}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":2}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[2]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 5", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 6", "[srcslice]" ) {
    sourceCode = "int foo(int a) {\n"
                "    int* x = &a;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"a\":2}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[2]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 6", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 7", "[srcslice]" ) {
    sourceCode = "int foo(int* ptr) {\n"
                "    int* ptr2 = ptr;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"ptr2\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"ptr\":2}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[2]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"ptr\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 7", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Aliases Test 8", "[srcslice]" ) {
    sourceCode = "int foo(int& num) {\n"
                "    int* ptr = &num;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"ptr\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"num\":2}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[2]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int&\",\n"
            "    \"name\":\"num\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Aliases Test 8", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}