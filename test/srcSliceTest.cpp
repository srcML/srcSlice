#include "srcSliceTest.hpp"

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

int PromptVerbose() {
    int mode = 0;
    std::cout << ":::: Select Verbose Mode ::::" << std::endl;
    std::cout << "    0 - None" << std::endl;
    std::cout << "    1 - Display Only Failed Cases" << std::endl;
    std::cout << "    2 - Full Verbose" << std::endl << std::endl;

    std::cout << "Enter Mode > ";
    std::cin >> mode;

    while (mode < 0 || mode > 2) {
        std::cout << "Enter Mode > ";
        std::cin >> mode;
    }
    std::cout << ":::::::::::::::::::::::::::::" << std::endl << std::endl;
    
    return mode;
}

void DebugOutput(int verboseMode, bool testStatus, const char* testName, const std::string& inputStr, const std::string& outputStr, std::string srcCode) {
    // Format srcCode to include line number for readability on verbose
    int startOfLine = 0;
    int endOfLine = srcCode.find('\n', startOfLine);
    int lineNumber = 0;
    int whiteSpaceLength = 5;
    
    // prepends line numbers before the final line
    while (endOfLine != -1) {
        ++lineNumber;
        
        std::string prependStr = std::to_string(lineNumber);
        
        for (int i = (lineNumber / 10); i < whiteSpaceLength; ++i) {
            prependStr += " ";
        }
        
        srcCode.insert(startOfLine, prependStr);
        endOfLine += prependStr.size();

        startOfLine = ++endOfLine;
        endOfLine = srcCode.find('\n', startOfLine);
    }

    // prepends the final line
    startOfLine = srcCode.rfind('\n', startOfLine) + 1;
    std::string prependStr = std::to_string(++lineNumber);
        
    for (int i = (lineNumber / 10); i < whiteSpaceLength; ++i) {
        prependStr += " ";
    }
    
    srcCode.insert(startOfLine, prependStr);

    if (verboseMode == 1) { // only show fails
        if (!testStatus) {
            std::cout << "======================================================" << std::endl;
            std::cout << "\033[33m" << testName << " :: Test Source Code" << "\033[0m" << std::endl;
            std::cout << "\033[0m" << srcCode << std::endl << std::endl;

            std::cout << "\033[33m" << testName << " :: Current Output" << "\033[0m" << std::endl;
            std::cout << "\033[0m" << inputStr << std::endl << std::endl;

            std::cout << "\033[33m" << testName << " :: Expected Output" << "\033[0m" << std::endl;
            std::cout << "\033[0m" << outputStr << std::endl << std::endl;
            std::cout << "======================================================" << std::endl;
        }
    } else if (verboseMode == 2) { // full verbose output | uses ANSI to show fails in red text
        std::cout << "======================================================" << std::endl;
        std::cout << "\033[33m" << testName << " :: Test Source Code" << "\033[0m" << std::endl;
        if (testStatus) {
            std::cout << "\033[0m" << srcCode << std::endl << std::endl;
        } else
        {
            std::cout << "\033[31m" << srcCode << "\033[0m" << std::endl << std::endl;
        }

        std::cout << "\033[33m" << testName << " :: Current Output" << "\033[0m" << std::endl;
        if (testStatus) {
            std::cout << "\033[0m" << inputStr << std::endl << std::endl;
        } else
        {
            std::cout << "\033[31m" << inputStr << "\033[0m" << std::endl << std::endl;
        }

        std::cout << "\033[33m" << testName << " :: Expected Output" << "\033[0m" << std::endl;
        if (testStatus) {
            std::cout << "\033[0m" << outputStr << std::endl << std::endl;
        } else
        {
            std::cout << "\033[31m" << outputStr << "\033[0m" << std::endl << std::endl;
        }
        std::cout << "======================================================" << std::endl;
    }
}