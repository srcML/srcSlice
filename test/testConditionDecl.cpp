#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

/*
======================================================
======================================================
======================================================



    NOTE |  Only if, for, switch statements support
            declaring variables inside their
            control/condition block



======================================================
======================================================
======================================================
*/

std::string sourceCode = "", input = "", output = "";
bool testStatus = false;

TEST_CASE( "Conditional Test 1", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    srand(time(nullptr));\n"
                "    if (int r = rand() % 100 - 1; r % 2 == 0) {\n"
                "        std::cout << r << std::endl;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "ifStmtDef.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"ifStmtDef.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"r\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[3]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 1", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 2", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    std::cout << mode << std::endl;\n"
                "    if (int k = 3; mode > 0) {\n"
                "        mode = k;\n"
                "    } else\n"
                "    {\n"
                "        mode = -1;\n"
                "    }\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependence\":[{\"mode\":5}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[4]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[2,5,8]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 2", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 3", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (int k = 3; mode > k) {\n"
                "        mode = k;\n"
                "    } else\n"
                "    {\n"
                "        mode = -1;\n"
                "    }\n"
                "    std::cout << mode << std::endl;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependence\":[{\"mode\":4}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"mode\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,9],\n"
            "    \"definition\":[2,4,7]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 3", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 4", "[srcslice]" ) {
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

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bmi\",\n"
            "    \"dependence\":[{\"i\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,8],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 4", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 5", "[srcslice]" ) {
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

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"bmi\",\n"
            "    \"dependence\":[{\"i\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,8,11,12],\n"
            "    \"definition\":[2,11]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 5", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}