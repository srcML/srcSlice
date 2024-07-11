#define CATCH_CONFIG_MAIN
#include "./srcslicetest.hpp"

TEST_CASE( "Pointer Test 1", "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                            "    std::string* strPtr;\n"
                            "}";

    std::string input = FetchSlices(sourceCode, "simplePointer.cpp");

    std::string output = "{\n"
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
                         "    \"definition\": [ 2 ],\n"
                         "    \"controlEdges\": [  ]\n"
                         "}\n"
                         "}\n";
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 2", "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                            "    std::string* strPtr;\n"
                            "    std::string key = \"abc1234!\";\n"
                            "    strPtr = &key;\n"
                            "}";

    std::string input = FetchSlices(sourceCode, "simplePointerDef.cpp");

    std::string output = "{\n"
                         "\"slice_0\" : {\n"
                         "    \"file\":\"simplePointerDef.cpp\",\n"
                         "    \"class\":\"\",\n"
                         "    \"function\":\"main\",\n"
                         "    \"type\":\"string\",\n"
                         "    \"name\":\"key\",\n"
                         "    \"dependentVariables\": [  ],\n"
                         "    \"aliases\": [ strPtr ],\n"
                         "    \"calledFunctions\": [  ],\n"
                         "    \"use\": [ 4 ],\n"
                         "    \"definition\": [ 3 ],\n"
                         "    \"controlEdges\": [  ]\n"
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
                         "    \"definition\": [ 2, 4 ],\n"
                         "    \"controlEdges\": [  ]\n"
                         "},\n"
                         "}\n";
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 3", "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                            "    std::string* strPtr;\n"
                            "    std::string key = \"abc1234!\";\n"
                            "    strPtr = &key;\n"
                            "    std::cout << *strPtr << std::endl;\n"
                            "}";

    std::string input = FetchSlices(sourceCode, "simplePointerDef.cpp");

    std::string output = "{\n"
                         "\"slice_0\" : {\n"
                         "    \"file\":\"simplePointerDef.cpp\",\n"
                         "    \"class\":\"\",\n"
                         "    \"function\":\"main\",\n"
                         "    \"type\":\"string\",\n"
                         "    \"name\":\"key\",\n"
                         "    \"dependentVariables\": [  ],\n"
                         "    \"aliases\": [ strPtr ],\n"
                         "    \"calledFunctions\": [  ],\n"
                         "    \"use\": [ 4 ],\n"
                         "    \"definition\": [ 3 ],\n"
                         "    \"controlEdges\": [  ]\n"
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
                         "    \"use\": [ 5 ],\n"
                         "    \"definition\": [ 2, 4 ],\n"
                         "    \"controlEdges\": [  ]\n"
                         "},\n"
                         "}\n";
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}