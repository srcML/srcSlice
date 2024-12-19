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
            "\"slice_0\":{\n"
            "    \"file\":\"simplePointer.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"std::string*\",\n"
            "    \"name\":\"strPtr\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[2]\n"
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
            "\"slice_0\":{\n"
            "    \"file\":\"simplePointerDef.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"std::string\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"simplePointerDef.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"std::string*\",\n"
            "    \"name\":\"strPtr\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"key\":4}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[],\n"
            "    \"definition\":[2,4]\n"
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
            "\"slice_0\":{\n"
            "    \"file\":\"simplePointerUse.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"std::string\",\n"
            "    \"name\":\"key\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[3]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"simplePointerUse.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"std::string*\",\n"
            "    \"name\":\"strPtr\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"key\":4}],\n"
            "    \"calls\":[],\n"
            "    \"use\":[5],\n"
            "    \"definition\":[2,4]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pointer Test 3", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 4", "[srcslice]" ) {
    sourceCode = "void updateValue(int* nptr) {\n"
                "   *nptr += 5;\n"
                "}\n"
                "int main() {\n"
                "   int num1 = 10;\n"
                "   int* ptr1 = &num1;\n"
                "   updateValue(ptr1);\n"
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"ptr1\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"num1\":6}],\n"
            "    \"calls\":[{\"functionName\":\"updateValue\",\"parameter\":\"1\",\"definitionLine\":\"1\"}],\n"
            "    \"use\":[1,2,7],\n"
            "    \"definition\":[2,6]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num1\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[6],\n"
            "    \"definition\":[5]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"updateValue\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"nptr\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1,2]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pointer Test 4", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Pointer Test 5", "[srcslice]" ) {
    sourceCode = "void swapPointers(int* left, int* right) {\n" // 1
                "   int temp = *left;\n" // 2
                "   *left = *right;\n" // 3
                "   *right = temp;\n" // 4
                "}\n" // 5
                "int main() {\n" // 6
                "   int num1 = 1;\n" // 7
                "   int* ptr1 = &num1;\n" // 8
                "   int num2 = 2;\n" // 9
                "   int* ptr2 = &num2;\n" // 10
                "   swapPointers(ptr1, ptr2);\n" // 11
                "}";

    input = FetchSlices(sourceCode.c_str(), "file.cpp");

    output = "{\n"
            "\"slice_0\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"ptr2\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"num2\":10}],\n"
            "    \"calls\":[{\"functionName\":\"swapPointers\",\"parameter\":\"2\",\"definitionLine\":\"1\"}],\n"
            "    \"use\":[1,3,11],\n"
            "    \"definition\":[4,10]\n"
            "},\n"
            "\"slice_1\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num2\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[10],\n"
            "    \"definition\":[9]\n"
            "},\n"
            "\"slice_2\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"ptr1\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[{\"num1\":8}],\n"
            "    \"calls\":[{\"functionName\":\"swapPointers\",\"parameter\":\"1\",\"definitionLine\":\"1\"}],\n"
            "    \"use\":[1,2,11],\n"
            "    \"definition\":[3,8]\n"
            "},\n"
            "\"slice_3\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"main\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"num1\",\n"
            "    \"dependence\":[],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[8],\n"
            "    \"definition\":[7]\n"
            "},\n"
            "\"slice_4\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swapPointers\",\n"
            "    \"type\":\"int\",\n"
            "    \"name\":\"temp\",\n"
            "    \"dependence\":[{\"right\":4}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[4],\n"
            "    \"definition\":[2]\n"
            "},\n"
            "\"slice_5\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swapPointers\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"right\",\n"
            "    \"dependence\":[{\"left\":3}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[3],\n"
            "    \"definition\":[1,4]\n"
            "},\n"
            "\"slice_6\":{\n"
            "    \"file\":\"file.cpp\",\n"
            "    \"language\":\"C++\",\n"
            "    \"namespace\":[],\n"
            "    \"class\":\"\",\n"
            "    \"function\":\"swapPointers\",\n"
            "    \"type\":\"int*\",\n"
            "    \"name\":\"left\",\n"
            "    \"dependence\":[{\"temp\":2}],\n"
            "    \"aliases\":[],\n"
            "    \"calls\":[],\n"
            "    \"use\":[2],\n"
            "    \"definition\":[1,3]\n"
            "}\n"
            "}\n";

    testStatus = (strcmp(input.c_str(), output.c_str()) == 0);
    DebugOutput(testStatus, "Pointer Test 5", input, output, sourceCode);
    
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}