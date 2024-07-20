#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool verbose = false, testStatus = false;

TEST_CASE( "Const-Reference Test 1", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "int AddFive(const int& num) {\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"AddFive\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Const-Reference Test 1", input, output);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Const Items Test 2", "[srcslice]" ) {
    sourceCode = "int AddFive(const int& num) {\n"
                "    return num + 5;"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"AddFive\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Const-Reference Test 2", input, output);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Const Items Test 3", "[srcslice]" ) {
    sourceCode = "std::string Concat(const std::string& left, const std::string& right) {\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"AddFive\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"right\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"AddFive\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Const-Reference Test 3", input, output);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Const Items Test 4", "[srcslice]" ) {
    sourceCode = "std::string Concat(const std::string& left, const std::string& right) {\n"
                "    return left + right;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"AddFive\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"right\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"AddFive\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Const-Reference Test 4", input, output);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}