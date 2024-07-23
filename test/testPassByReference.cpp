#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
int verbose = -1;
bool testStatus = false;

TEST_CASE( "Pass-By-Reference Test 1", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "void foo(int& a) {\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 1", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 2", "[srcslice]" ) {
    sourceCode = "void foo(int& a) {\n"
                "    std::cout << a << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 2", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 3", "[srcslice]" ) {
    sourceCode = "void foo(int& a) {\n"
                "    std::cout << a << std::endl;\n"
                "    a = 8;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 3", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 4", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 4", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 5", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    b = 24;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 5", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 6", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    b = 24;\n"
                "    std::cout << b << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 6", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 7", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    a = 4;\n"
                "    b = 24;\n"
                "    std::cout << b << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 7", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 8", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    a = 4;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 24;\n"
                "    std::cout << b << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 8", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

/*
    Aliases and Dependent Variables need to be reviewed from this point
    to the end of file
*/

TEST_CASE( "Pass-By-Reference Test 9", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    a = 4;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 24;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 7 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"r\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 9", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 10", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    a = 4;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 24;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a + b;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 7 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [ \"r\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5, 7 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"r\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 10", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 11", "[srcslice]" ) {
    sourceCode = "void foo(int& a, int& b) {\n"
                "    a = 4;\n"
                "    std::cout << a << std::endl;\n"
                "    b = 24;\n"
                "    std::cout << b << std::endl;\n"
                "\n"
                "    int r = a + b;\n"
                "    std::cout << r << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 8 ],\n"
            "    \"definition\": [ 7 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [ \"r\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5, 7 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"r\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 11", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 12", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"foo\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 12", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 13", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 13", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 14", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 14", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 15", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 15", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 16", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 16", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 17", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}\n"
                "int main() {\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 17", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 18", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}\n"
                "int main() {\n"
                "    int bsd = 24;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bsd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 7 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_3\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 18", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 19", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}\n"
                "int main() {\n"
                "    int bsd = 24;\n"
                "    int ssd = 7;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bsd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 9 ],\n"
            "    \"definition\": [ 7 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"ssd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 9 ],\n"
            "    \"definition\": [ 8 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_3\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_4\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 19", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 20", "[srcslice]" ) {
    sourceCode = "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}\n"
                "int main() {\n"
                "    int bsd = 24;\n"
                "    int ssd = 7;\n"
                "    swap(bsd, ssd);\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bsd\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"1\"} ],\n"
            "    \"use\": [ 1, 2, 9 ],\n"
            "    \"definition\": [ 3, 7 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"ssd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"2\"} ],\n"
            "    \"use\": [ 1, 3, 9 ],\n"
            "    \"definition\": [ 4, 8 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_3\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_4\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 20", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 21", "[srcslice]" ) {
    sourceCode = "void bar(int& num) {\n"
                "}\n"
                "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}\n"
                "int main() {\n"
                "    int bsd = 24;\n"
                "    int ssd = 7;\n"
                "    swap(bsd, ssd);\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bsd\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"1\"} ],\n"
            "    \"use\": [ 3, 4, 11 ],\n"
            "    \"definition\": [ 5, 9 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"ssd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"2\"} ],\n"
            "    \"use\": [ 3, 5, 11 ],\n"
            "    \"definition\": [ 6, 10 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 6 ],\n"
            "    \"definition\": [ 4 ]\n"
            "},\n"
            "\"slice_3\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 3, 5 ]\n"
            "},\n"
            "\"slice_4\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 3, 6 ]\n"
            "},\n"
            "\"slice_5\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 21", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 22", "[srcslice]" ) {
    sourceCode = "void bar(int& num) {\n"
                "    std::cout << num << std::endl;\n"
                "}\n"
                "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "}\n"
                "int main() {\n"
                "    int bsd = 24;\n"
                "    int ssd = 7;\n"
                "    swap(bsd, ssd);\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bsd\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"1\"} ],\n"
            "    \"use\": [ 4, 5, 12 ],\n"
            "    \"definition\": [ 6, 10 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"ssd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"2\"} ],\n"
            "    \"use\": [ 4, 6, 12 ],\n"
            "    \"definition\": [ 7, 11 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 7 ],\n"
            "    \"definition\": [ 5 ]\n"
            "},\n"
            "\"slice_3\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 4, 6 ]\n"
            "},\n"
            "\"slice_4\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 6 ],\n"
            "    \"definition\": [ 4, 7 ]\n"
            "},\n"
            "\"slice_5\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 22", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 23", "[srcslice]" ) {
    sourceCode = "void bar(int& num) {\n"
                "    std::cout << num << std::endl;\n"
                "}\n"
                "void swap(int& a, int& b) {\n"
                "    int temp = a;\n"
                "    a = b;\n"
                "    b = temp;\n"
                "    bar(temp);\n"
                "}\n"
                "int main() {\n"
                "    int bsd = 24;\n"
                "    int ssd = 7;\n"
                "    swap(bsd, ssd);\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bsd\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"1\"} ],\n"
            "    \"use\": [ 4, 5, 13 ],\n"
            "    \"definition\": [ 6, 11 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"ssd\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"swap\", \"parameterNumber\": \"2\"} ],\n"
            "    \"use\": [ 4, 6, 13 ],\n"
            "    \"definition\": [ 7, 12 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [ {\"functionName\": \"bar\", \"parameterNumber\": \"1\"} ],\n"
            "    \"use\": [ 1, 2, 7, 8 ],\n"
            "    \"definition\": [ 5 ]\n"
            "},\n"
            "\"slice_3\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ \"temp\" ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 4, 6 ]\n"
            "},\n"
            "\"slice_4\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swap\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 6 ],\n"
            "    \"definition\": [ 4, 7 ]\n"
            "},\n"
            "\"slice_5\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Pass-By-Reference Test 23", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}