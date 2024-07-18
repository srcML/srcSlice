#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool verbose = false;

TEST_CASE( "Pass-By-Reference Test 1", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "void Square(int& n) {\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"n\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 1 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 1 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 2", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "void Square(int& n) {\n"
                "    n *= n;\n"
                "}\n";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"n\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 2 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 2 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 3", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "std::string Concat(std::string& left, std::string& right) {\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
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
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 3 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 3 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 4", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "std::string Concat(std::string& left, std::string& right) {\n"
                "    std::string result;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"result\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"right\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 4 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 4 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 5", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "std::string Concat(std::string& left, std::string& right) {\n"
                "    std::string result = left + right;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"result\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"right\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 5 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 5 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 6", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "std::string Concat(std::string& left, std::string& right) {\n"
                "    std::string result = left + right;\n"
                "    return result;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"result\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 2 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"right\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "},\n"
            "\"slice_2\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"Square\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 6 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Reference Test 6 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}