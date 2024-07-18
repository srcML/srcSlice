#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool verbose = false;

TEST_CASE( "Pass-By-Value Test 1", "[srcslice]" ) {
    verbose = PromptVerbose();

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
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 1 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 1 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Reference Test 2", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "int bar(int a) {\n"
                "    return ++a;\n"
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
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1, 2 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 2 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 2 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Value Test 3", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "int sums(int a, int b) {\n"
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
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 3 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 3 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pass-By-Value Test 4", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "int sums(int a, int b) {\n"
                "    return a + b;\n"
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
            "    \"use\": [ 2 ],\n"
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
            "    \"use\": [ 2 ],\n"
            "    \"definition\": [ 1 ]\n"
            "}\n"
            "}";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 4 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pass-By-Value Test 4 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}