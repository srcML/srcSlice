#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool testStatus = false;

TEST_CASE( "Pass-By-Value Test 1", "[srcslice]" ) {
    sourceCode = "int bar(int a) {\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
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
            "    \"use\":[],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 1", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 2", "[srcslice]" ) {
    sourceCode = "int bar(int a) {\n"
                "    a = 12;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
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
            "    \"use\":[],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 2", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 3", "[srcslice]" ) {
    sourceCode = "int bar(int a) {\n"
                "    a = 12;\n"
                "    std::cout << a << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
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
            "    \"use\":[3],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 3", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 4", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[1]\n"
            "},\n"
            "\"slice_1\":{\n"
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
            "    \"use\":[],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 4", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 5", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    b = 4;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[1,2]\n"
            "},\n"
            "\"slice_1\":{\n"
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
            "    \"use\":[],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 5", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 6", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[1,2]\n"
            "},\n"
            "\"slice_1\":{\n"
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
            "    \"use\":[],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 6", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 7", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[1,3]\n"
            "},\n"
            "\"slice_1\":{\n"
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
            "    \"use\":[],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 7", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 8", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[1,4]\n"
            "},\n"
            "\"slice_1\":{\n"
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
            "    \"use\":[3],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 8", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 9", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[7]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[1,4]\n"
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
            "    \"use\":[3],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 9", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 10", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[7]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[1,4]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,7],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 10", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 11", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a + b;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[7]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5,7],\n"
            "    \"definition\":[1,4]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,7],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 11", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 12", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a + b;\n"
                "    std::cout << r << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[8],\n"
            "    \"definition\":[7]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5,7],\n"
            "    \"definition\":[1,4]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,7],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 12", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 13", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a + b;\n"
                "    std::cout << r << std::endl;\n"
                "}\n"
                "int main() {\n" // 10
                "   int x = 10;\n"
                "   int y = 11;\n"
                "   bar(x,y);\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[{\"functionName\":\"bar\",\"parameter\":\"1\",\"definitionLine\":\"1\"}],\n"
            "    \"use\":[1,2,3,7,13],\n"
            "    \"definition\":[11]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"y\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[{\"functionName\":\"bar\",\"parameter\":\"2\",\"definitionLine\":\"1\"}],\n"
            "    \"use\":[1,4,5,7,13],\n"
            "    \"definition\":[12]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[8],\n"
            "    \"definition\":[7]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5,7],\n"
            "    \"definition\":[1,4]\n"
            "},\n"
            "\"slice_4\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":7}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,7],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 13", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 14", "[srcslice]" ) {
    sourceCode = "class MyClass {\n"
                "public:\n"
                "    int bar(int a, int b) {\n"
                "        a = 24;\n"
                "        std::cout << a << std::endl;\n"
                "        b = 4;\n"
                "        std::cout << b << std::endl;\n"
                "\n"
                "        int r = a + b;\n"
                "        std::cout << r << std::endl;\n"
                "   }\n"
                "};";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"MyClass\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[10],\n"
            "    \"definition\":[9]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"MyClass\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":9}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[7,9],\n"
            "    \"definition\":[3,6]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"MyClass\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":9}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5,9],\n"
            "    \"definition\":[3,4]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 14", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 15", "[srcslice]" ) {
    sourceCode = "namespace MyNameSpace {\n"
                "    int bar(int a, int b) {\n"
                "        a = 24;\n"
                "        std::cout << a << std::endl;\n"
                "        b = 4;\n"
                "        std::cout << b << std::endl;\n"
                "\n"
                "        int r = a + b;\n"
                "        std::cout << r << std::endl;\n"
                "   }\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[9],\n"
            "    \"definition\":[8]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":8}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[6,8],\n"
            "    \"definition\":[2,5]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":8}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4,8],\n"
            "    \"definition\":[2,3]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 15", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 16", "[srcslice]" ) {
    sourceCode = "const float pi = 3.14;\n"
                "int main() {\n"
                "    float radius = 10;\n"
                "    float area = pi * (radius * radius);\n"
                "    std::cout << \"Circle Area -> \" << area << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"area\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[4]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"radius\",\n"
            "    \"dependence\":[{\"area\":4}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"pi\",\n"
            "    \"dependence\":[{\"area\":4}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 16", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 17", "[srcslice]" ) {
    sourceCode = "namespace MyNameSpace {\n"
                "    int bar(int a, int b) {\n"
                "        a = 24;\n"
                "        std::cout << a << std::endl;\n"
                "        b = 4;\n" // 5
                "        std::cout << b << std::endl;\n"
                "\n"
                "        int r = a + b;\n"
                "        std::cout << r << std::endl;\n"
                "   }\n" // 10
                "}\n"
                "int main() {\n"
                "   int x = 10;\n"
                "   int y = 11;\n"
                "   MyNameSpace::bar(x,y);\n" // 15
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[{\"functionName\":\"bar\",\"parameter\":\"1\",\"definitionLine\":\"2\"}],\n"
            "    \"use\":[2,3,4,8,15],\n"
            "    \"definition\":[13]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"y\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[{\"functionName\":\"bar\",\"parameter\":\"2\",\"definitionLine\":\"2\"}],\n"
            "    \"use\":[2,5,6,8,15],\n"
            "    \"definition\":[14]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[9],\n"
            "    \"definition\":[8]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":8}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[6,8],\n"
            "    \"definition\":[2,5]\n"
            "},\n"
            "\"slice_4\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":8}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4,8],\n"
            "    \"definition\":[2,3]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 17", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 18", "[srcslice]" ) {
    sourceCode = "const char* arch = \"arm64\";\n"
                "namespace MyNameSpace {\n"
                "    int bar(int a, int b) {\n"
                "        a = 24;\n"
                "        std::cout << a << std::endl;\n"
                "        b = 4;\n" // 6
                "        std::cout << b << std::endl;\n"
                "\n"
                "        int r = a + b;\n"
                "        std::cout << arch << \" | \" << r << std::endl;\n"
                "   }\n" // 11
                "}\n"
                "int main() {\n"
                "   int x = 10;\n"
                "   int y = 11;\n"
                "   MyNameSpace::bar(x,y);\n" // 16
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"x\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[{\"functionName\":\"bar\",\"parameter\":\"1\",\"definitionLine\":\"3\"}],\n"
            "    \"use\":[3,4,5,9,16],\n"
            "    \"definition\":[14]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"y\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[{\"functionName\":\"bar\",\"parameter\":\"2\",\"definitionLine\":\"3\"}],\n"
            "    \"use\":[3,6,7,9,16],\n"
            "    \"definition\":[15]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[10],\n"
            "    \"definition\":[9]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":9}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[7,9],\n"
            "    \"definition\":[3,6]\n"
            "},\n"
            "\"slice_4\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[\"MyNameSpace\"],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":9}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5,9],\n"
            "    \"definition\":[3,4]\n"
            "},\n"
            "\"slice_5\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"char*\",\n"
            "    \"name\":\"arch\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[10],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 18", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 19", "[srcslice]" ) {
    sourceCode = "std::string log_ok = \"[+] \";\n"
                "class MyClass {\n"
                "public:\n"
                "    int bar(int a, int b) {\n"
                "        a = 24;\n"
                "        std::cout << log_ok << a << std::endl;\n"
                "        b = 4;\n"
                "        std::cout << log_ok << b << std::endl;\n"
                "\n"
                "        int r = a + b;\n"
                "        std::cout << log_ok << r << std::endl;\n"
                "   }\n"
                "};";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"MyClass\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[11],\n"
            "    \"definition\":[10]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"MyClass\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependence\":[{\"r\":10}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[8,10],\n"
            "    \"definition\":[4,7]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"MyClass\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependence\":[{\"r\":10}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[6,10],\n"
            "    \"definition\":[4,5]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"std::string\",\n"
            "    \"name\":\"log_ok\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[6,8,11],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 19", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 20", "[srcslice]" ) {
    sourceCode = "float radius = 0, PI = 3.14;\n"
                "int main() {\n"
                "    std::cin >> radius;\n"
                "    std::cout << \"[*] Circumference -> \" << 2 * radius * PI << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"PI\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[1]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"radius\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[1,3]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 20", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 21", "[srcslice]" ) {
    sourceCode = "float radius = 0, PI = 3.14;\n"
                "int main() {\n"
                "    radius = rand() % 20 + 1;\n"
                "    std::cout << \"[*] Circumference -> \" << 2 * radius * PI << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"PI\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[1]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"\",\n"
            "    \"type\":\"float\",\n"
            "    \"name\":\"radius\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[1,3]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 21", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}