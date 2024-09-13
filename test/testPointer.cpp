#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool testStatus = false;

TEST_CASE( "Pointer Test 1", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    std::string* strPtr;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simplePointer.cpp");

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

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pointer Test 1", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 2", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    std::string* strPtr;\n"
                "    std::string key = \"abc1234!\";\n"
                "    strPtr = &key;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simplePointerDef.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simplePointerDef.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [ { \"strPtr\": 4 } ],\n"
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

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pointer Test 2", input, output, sourceCode);

    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 3", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    std::string* strPtr;\n"
                "    std::string key = \"abc1234!\";\n"
                "    strPtr = &key;\n"
                "    std::cout << *strPtr << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simplePointerUse.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simplePointerUse.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"string\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [ { \"strPtr\": 4 } ],\n"
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

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pointer Test 3", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}