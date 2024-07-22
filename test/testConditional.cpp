#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

/*
    Only if, for, switch statements support
    declaring variables inside their control/condition
    block
*/

std::string sourceCode = "", input = "", output = "";
bool verbose = false, testStatus = false;

TEST_CASE( "Conditional Test 1", "[srcslice]" ) {
    verbose = PromptVerbose();
    
    sourceCode = "int main() {\n"
                "    srand(time(nullptr));\n"
                "    if (int r = rand() % 100 - 1; r % 2 == 0) {\n"
                "        std::cout << r << std::endl;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode, "ifStmtDef.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"ifStmtDef.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 1", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 2", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    char z = 'G';\n"
                "    if (z == '!') {\n"
                "        return 1;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode, "simpleIfStmt.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simpleIfStmt.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"char\",\n"
            "    \"name\":\"z\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 2", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 3", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode, "simpleDoWhileStmt.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simpleDoWhileStmt.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"tmp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4, 5 ],\n"
            "    \"definition\": [ 2, 4 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 3", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 4", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    while (tmp < 10) {\n"
                "        tmp++;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode, "simpleWhileStmt.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simpleWhileStmt.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"tmp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 2, 4 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 4", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 5", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    for (int i = 0; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode, "simpleForStmt.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simpleForStmt.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 2, 3 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 5", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 6", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int c = 0;\n"
                "    switch (c) {\n"
                "        case 0:\n"
                "            break;\n"
                "\n"
                "        case 1:\n"
                "            break;\n"
                "\n"
                "        case 2:\n"
                "            break;\n"
                "\n"
                "        default:\n"
                "            break;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode, "simpleSwitchStmt.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"simpleSwitchStmt.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"c\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4, 7, 10 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 6", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 7", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (mode > 0) {\n"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 7", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 8", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (mode > 0) {\n"
                "        std::cout << mode << std::endl;\n"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 8", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 9", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (mode > 0) {\n"
                "        std::cout << mode << std::endl;\n"
                "    } else\n"
                "    {\n"
                "        mode = -1;"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 2, 7 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 9", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 10", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    std::cout << mode << std::endl;\n"
                "    if (int k = 3; mode > 0) {\n"
                "        mode = k;\n"
                "    } else\n"
                "    {\n"
                "        mode = -1;"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 5 ],\n"
            "    \"definition\": [ 4 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 2, 5, 8 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 10", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 11", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (int k = 3; mode > k) {\n"
                "        mode = k;\n"
                "    } else\n"
                "    {\n"
                "        mode = -1;"
                "    }\n"
                "    std::cout << mode << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 9 ],\n"
            "    \"definition\": [ 2, 4, 7 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 11", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 12", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int a = 2;\n"
                "    if (a < 0) {\n"
                "        std::cout << \"bad number\" << std::endl;\n"
                "    }\n"
                "    a = 5;\n"
                "    std::cout << a << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"a\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 7 ],\n"
            "    \"definition\": [ 2, 6 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 12", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 13", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int k = 5;\n"
                "    for (int i = k; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 13", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 14", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int k = 5;\n"
                "    for (int i = k; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "    }\n"
                "    std::cout << k << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 6 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 14", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 15", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int k = 5;\n"
                "    for (int i = k; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "        k += i;\n"
                "    }\n"
                "    std::cout << k << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 5, 7 ],\n"
            "    \"definition\": [ 2, 5 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 15", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 16", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    std::cout << tmp << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"tmp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4, 5, 6 ],\n"
            "    \"definition\": [ 2, 4 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 16", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 17", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        std::cout << tmp << std::endl;\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    std::cout << tmp << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"tmp\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4, 5, 6, 7 ],\n"
            "    \"definition\": [ 2, 5 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 17", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 18", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int bmi = 74;\n"
                "    switch(int i = bmi) {\n"
                "        case 10:\n"
                "            std::cout << \"you win\" << std::endl;\n"
                "        break;\n"
                "        default:\n"
                "            std::cout << bmi << std::endl;\n"
                "        break;\n"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bmi\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 8 ],\n"
            "    \"definition\": [ 2 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 18", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 19", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int bmi = 74;\n"
                "    switch(int i = bmi) {\n"
                "        case 10:\n"
                "            std::cout << \"you win\" << std::endl;\n"
                "        break;\n"
                "        default:\n"
                "            std::cout << bmi << std::endl;\n"
                "        break;\n"
                "    }\n"
                "    bmi -= 12;\n"
                "    std::cout << bmi << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode, "file.cpp");

    output = "{\n"
            "\"slice_0\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 4 ],\n"
            "    \"definition\": [ 3 ]\n"
            "},\n"
            "\"slice_1\" : {\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bmi\",\n"
            "    \"dependentVariables\": [  ],\n"
            "    \"aliases\": [  ],\n"
            "    \"calledFunctions\": [  ],\n"
            "    \"use\": [ 3, 8, 11, 12 ],\n"
            "    \"definition\": [ 2, 11 ]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(verbose, testStatus, "Conditional Test 19", input, output);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}