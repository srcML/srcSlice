#include "srcSliceTest.hpp"

bool PromptVerbose() {
    char resp = 0;
    std::cout << "Do you want to run tests with Verbose Output? [N/y] : ";
    while (resp != 'n' && resp != 'N' && resp != 'y' && resp != 'Y') {
        std::cin >> resp;
    }
    
    return (resp == 'Y' || resp == 'y');
}

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
    // std::cout << "\033[32m" << "[+] Displaying Slice Results for :: " << fileName << "\033[0m" << std::endl;
    // std::cout << stream2string << std::endl;

    return stream2string;
}

void DebugOutput(bool verboseMode, bool testStatus, const char* testName, const std::string& inputStr, const std::string& outputStr) {
    if (verboseMode) {
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << testName << " :: Input" << "\033[0m" << std::endl;
        if (testStatus) {
            std::cout << "\033[0m" << inputStr << std::endl << std::endl;
        } else
        {
            std::cout << "\033[31m" << inputStr << "\033[0m" << std::endl << std::endl;
        }
        std::cout << "\033[33m" << testName << " :: Output" << "\033[0m" << std::endl;
        if (testStatus) {
            std::cout << "\033[0m" << outputStr << std::endl << std::endl;
        } else
        {
            std::cout << "\033[31m" << outputStr << "\033[0m" << std::endl << std::endl;
        }
        std::cout << "======================================================" << std::endl;
    }
}