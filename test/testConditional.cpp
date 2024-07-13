#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool verbose = false;

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

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pointer Test 4 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pointer Test 4 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }
    
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

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pointer Test 5 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pointer Test 5 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }
    
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

    if (verbose) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << "Pointer Test 6 :: Input" << "\033[0m" << std::endl;
        std::cout << input.c_str() << std::endl << std::endl;
        std::cout << "\033[33m" << "Pointer Test 6 :: Output" << "\033[0m" << std::endl;
        std::cout << output.c_str() << std::endl;
        std::cout << "======================================================" << std::endl;
    }
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}