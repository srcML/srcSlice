#define CATCH_CONFIG_MAIN
#include <srcslicepolicy.hpp>
#include <sstream>
#include <srcml.h>
#include "../src/catch2/catch.hpp"

std::string StringToSrcML(std::string str, const char* fileName){ // Function by Cnewman
    struct srcml_archive* archive;
    struct srcml_unit* unit;
    size_t size = 0;

    char *ch = 0;

    archive = srcml_archive_create();
    srcml_archive_enable_option(archive, SRCML_OPTION_POSITION);
    srcml_archive_write_open_memory(archive, &ch, &size);

    unit = srcml_unit_create(archive);
    srcml_unit_set_language(unit, SRCML_LANGUAGE_CXX);
    srcml_unit_set_filename(unit, fileName);

    srcml_unit_parse_memory(unit, str.c_str(), str.size());
    srcml_archive_write_unit(archive, unit);
    
    srcml_unit_free(unit);
    srcml_archive_close(archive);
    srcml_archive_free(archive);

    ch[size-1] = 0;
    
    return std::string(ch);
}

std::string FetchSlices(const std::string cppSource, const char* fileName) {
    std::ostringstream output;
    std::string srcmlStr = StringToSrcML(cppSource, fileName);

    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    SrcSlicePolicy *cat = new SrcSlicePolicy(&profileMap);
    srcSAXController control(srcmlStr);
    srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
    control.parse(&handler); // Start parsing

    size_t totalElements = profileMap.size();
    size_t currIndex = 0, sliceIndex = 0;

    output << "{" << std::endl;
    for (auto it : profileMap) {
        ++currIndex;
        for (auto profile : it.second) {
            if (profile.containsDeclaration) {
                ++sliceIndex;
                profile.SetJsonOut(true);
                profile.SetSliceIndex(sliceIndex - 1);

                output << profile;
                if (currIndex != totalElements) {
                    std::cout << currIndex << "|" << totalElements << std::endl;
                    output << "," << std::endl;
                } else {
                    output << std::endl;
                }
            }
        }
    }
    output << "}" << std::endl;

    // Check for leading comma and remove it
    std::string stream2string = output.str();

    if (stream2string[stream2string.size() - 4] == ',')
    {
        stream2string.erase(stream2string.size() - 4, 1);
    }

    // debugging output
    std::cout << "\033[32m" << "[+] Displaying Slice Results for :: " << fileName << "\033[0m" << std::endl;
    std::cout << stream2string << std::endl;

    return stream2string;
}

TEST_CASE( "Checking Slice Profiles Definitions", "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                             "  int sum = 0;\n"
                             "}";

    std::string input = FetchSlices(sourceCode, "simpleDef.cpp");

    std::string output = "{\n"
                        "\"slice_0\" : {\n"
                        "    \"file\":\"simpleDef.cpp\",\n"
                        "    \"class\":\"\",\n"
                        "    \"function\":\"main\",\n"
                        "    \"type\":\"int\",\n"
                        "    \"name\":\"sum\",\n"
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

TEST_CASE( "Checking Slice Profiles Definitions 2", "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                            "    srand(time(nullptr));\n"
                            "    if (int r = rand() % 100 - 1; r % 2 == 0) {\n"
                            "        std::cout << r << std::endl;\n"
                            "    }\n"
                            "    return 0;\n"
                            "}";

    std::string input = FetchSlices(sourceCode, "ifStmtDef.cpp");

    std::string output = "{\n"
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
                        "    \"definition\": [ 3 ],\n"
                        "    \"controlEdges\": [ [3, 4] ]\n"
                        "}\n"
                        "}\n";
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}

TEST_CASE( "Checking Slice Profiles Definitions 3", "[srcslice]" ) {
    std::string sourceCode = "int main() {\n"
                            "    const char str[] = \"hellworld!\";\n"
                            "}";

    std::string input = FetchSlices(sourceCode, "simpleStaticArray.cpp");

    std::string output = "{\n"
                        "\"slice_0\" : {\n"
                        "    \"file\":\"simpleStaticArray.cpp\",\n"
                        "    \"class\":\"\",\n"
                        "    \"function\":\"main\",\n"
                        "    \"type\":\"char\",\n"
                        "    \"name\":\"str\",\n"
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

TEST_CASE( "Checking Slice Profiles Definitions 4", "[srcslice]" ) {
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
                        "    \"aliases\": [ \"strPtr\" ],\n"
                        "    \"calledFunctions\": [  ],\n"
                        "    \"use\": [ 4 ],\n"
                        "    \"definition\": [ 3 ],\n"
                        "    \"controlEdges\": [ [3, 4] ]\n"
                        "},\n"
                        "\"slice_1\" : {\n"
                        "    \"file\":\"simplePointerDef.cpp\",\n"
                        "    \"class\":\"\",\n"
                        "    \"function\":\"main\",\n"
                        "    \"type\":\"string\",\n"
                        "    \"name\":\"strPtr\",\n"
                        "    \"dependentVariables\": [  ],\n"
                        "    \"aliases\": [ \"strPtr\" ],\n"
                        "    \"calledFunctions\": [  ],\n"
                        "    \"use\": [  ],\n"
                        "    \"definition\": [ 2, 4 ],\n"
                        "    \"controlEdges\": [ [2, 4] ]\n"
                        "}\n"
                        "}\n";
    REQUIRE( strcmp(input.c_str(), output.c_str()) == 0 );
}