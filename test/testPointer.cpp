#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool verbose = false;

TEST_CASE( "Pointer Test 1", "[srcslice]" ) {
    verbose = PromptVerbose();

    sourceCode = "int main() {\n"
                "    std::string* strPtr;\n"
                "}";

    input = FetchSlices(sourceCode, "simplePointer.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simplePointer.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"strPtr\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pointer Test 1 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pointer Test 1 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 2", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    std::string* strPtr;\n"
                "    std::string key = \"abc1234!\";\n"
                "    strPtr = &key;\n"
                "}";

    input = FetchSlices(sourceCode, "simplePointerDef.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simplePointerDef.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [ \"strPtr\" ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"simplePointerDef.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"strPtr\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [  ],\n"
            "    \"definition\": [ 2, 4 ]\n"
            "}\n"
            "}\n";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pointer Test 2 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pointer Test 2 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 3", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    std::string* strPtr;\n"
                "    std::string key = \"abc1234!\";\n"
                "    strPtr = &key;\n"
                "    std::cout << *strPtr << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "simplePointerUse.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simplePointerUse.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [ \"strPtr\" ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"simplePointerUse.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"strPtr\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 2, 4 ]\n"
            "}\n"
            "}\n";

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pointer Test 3 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pointer Test 3 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}