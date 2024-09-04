#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool testStatus = false;

TEST_CASE( "Pass-By-Value Test 1", "[srcslice]" ) {
    sourceCode = "int bar(int a) {\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
    DebugOutput(testStatus, "Pass-By-Value Test 1", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 2", "[srcslice]" ) {
    sourceCode = "int bar(int a) {\n"
                "    a = 12;\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
    DebugOutput(testStatus, "Pass-By-Value Test 2", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 3", "[srcslice]" ) {
    sourceCode = "int bar(int a) {\n"
                "    a = 12;\n"
                "    std::cout << a << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
    DebugOutput(testStatus, "Pass-By-Value Test 3", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 4", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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
    DebugOutput(testStatus, "Pass-By-Value Test 4", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 5", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    b = 4;\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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
    DebugOutput(testStatus, "Pass-By-Value Test 5", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 6", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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
    DebugOutput(testStatus, "Pass-By-Value Test 6", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 7", "[srcslice]" ) {
    sourceCode = "int bar(int a, int b) {\n"
                "    a = 24;\n"
                "    b = 4;\n"
                "    std::cout << b << std::endl;\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ { \"r\": 7 } ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
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

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [ { \"r\": 7 } ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5, 7 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ { \"r\": 7 } ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
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

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
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
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"b\",\n"
            "    \"dependentVariables\": [ { \"r\": 7 } ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5, 7 ],\n"
            "    \"definition\": [ 1, 4 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"bar\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [ { \"r\": 7 } ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pass-By-Value Test 12", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}