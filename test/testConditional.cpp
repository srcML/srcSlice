#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

std::string sourceCode = "", input = "", output = "";
bool testStatus = false;

TEST_CASE( "Conditional Test 1", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    char z = 'G';\n"
                "    if (z == '!') {\n"
                "        return 1;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simpleIfStmt.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"simpleIfStmt.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"char\",\n"
            "    \"name\":\"z\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 1", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 2", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simpleDoWhileStmt.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"simpleDoWhileStmt.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4,5],\n"
            "    \"definition\":[2,4]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 2", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 3", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    while (tmp < 10) {\n"
                "        tmp++;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simpleWhileStmt.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"simpleWhileStmt.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[2,4]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 3", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 4", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    for (int i = 0; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "    }\n"
                "    return 0;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "simpleForStmt.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"simpleForStmt.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2,3],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 4", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 5", "[srcslice]" ) {
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

    input = FetchSlices(sourceCode.c_str(), "simpleSwitchStmt.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"simpleSwitchStmt.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"c\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4,7,10],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 5", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 6", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (mode > 0) {\n"
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
            "    \"name\":\"mode\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 6", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 7", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (mode > 0) {\n"
                "        std::cout << mode << std::endl;\n"
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
            "    \"name\":\"mode\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 7", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 8", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int mode = 0;\n"
                "    if (mode > 0) {\n"
                "        std::cout << mode << std::endl;\n"
                "    } else {\n"
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
            "    \"name\":\"mode\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4],\n"
            "    \"definition\":[2,6]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 8", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 9", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int a = 2;\n"
                "    if (a < 0) {\n"
                "        std::cout << \"bad number\" << std::endl;\n"
                "    }\n"
                "    a = 5;\n"
                "    std::cout << a << std::endl;\n"
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
            "    \"name\":\"a\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,7],\n"
            "    \"definition\":[2,6]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 9", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 10", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int k = 5;\n"
                "    for (int i = k; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
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
            "    \"name\":\"k\",\n"
            "    \"dependence\":[{\"i\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 10", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 11", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int k = 5;\n"
                "    for (int i = k; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "    }\n"
                "    std::cout << k << std::endl;\n"
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
            "    \"name\":\"k\",\n"
            "    \"dependence\":[{\"i\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,6],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 11", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 12", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int k = 5;\n"
                "    for (int i = k; i < 10; ++i) {\n"
                "        std::cout << i << std::endl;\n"
                "        k += i;\n"
                "    }\n"
                "    std::cout << k << std::endl;\n"
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
            "    \"dependence\":[{\"k\":5}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4,5],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"k\",\n"
            "    \"dependence\":[{\"i\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,5,7],\n"
            "    \"definition\":[2,5]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 12", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 13", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    std::cout << tmp << std::endl;\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4,5,6],\n"
            "    \"definition\":[2,4]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 13", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 14", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        std::cout << tmp << std::endl;\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    std::cout << tmp << std::endl;\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4,5,6,7],\n"
            "    \"definition\":[2,5]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 14", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 15", "[srcslice]" ) {
    sourceCode = "int LinearSearch(int* array, int size, int key) {\n"
                "    for (int i = 0; i < size; ++i) {\n"
                "       if (array[i] == key)\n"
                "            return i;\n"
                "    }\n"
                "    return -1;\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"LinearSearch\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"i\",\n"
            "    \"dependence\":[{\"array\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2,3,4],\n"
            "    \"definition\":[2]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"LinearSearch\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[1]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"LinearSearch\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"size\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"LinearSearch\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"array\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[1]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 15", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 16", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    if (tmp < 10) {\n"
                "       if (tmp > 5) {\n"
                "           std::cout << tmp << std::endl;\n"
                "       }\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4,5],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 16", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 17", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    if (tmp < 10) {\n"
                "       if (tmp > 5) {\n"
                "           std::cout << tmp << std::endl;\n"
                "       } else {\n"
                "           std::cout << tmp << std::endl;\n"
                "       }\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4,5,7],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 17", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 18", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    if (tmp < 10) {\n"
                "       if (tmp > 5) {\n"
                "           std::cout << tmp << std::endl;\n"
                "       } else if (tmp <= 5) {\n"
                "           std::cout << tmp << std::endl;\n"
                "       }\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4,5,6,7],\n"
            "    \"definition\":[2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 18", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 19", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    do {\n"
                "        if (tmp >= 5) {\n"
                "            std::cout << tmp << std::endl;\n"
                "        }\n"
                "        tmp++;\n"
                "    } while (tmp < 10);\n"
                "    std::cout << tmp << std::endl;\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4,5,7,8,9],\n"
            "    \"definition\":[2,7]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 19", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Conditional Test 20", "[srcslice]" ) {
    sourceCode = "int main() {\n"
                "    int tmp = 0;\n"
                "    while (tmp < 10) {\n"
                "        if (tmp >= 5) {\n"
                "            std::cout << tmp << std::endl;\n"
                "        }\n"
                "        tmp++;\n"
                "    }\n"
                "    std::cout << tmp << std::endl;\n"
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
            "    \"name\":\"tmp\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3,4,5,7,9],\n"
            "    \"definition\":[2,7]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Conditional Test 20", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}