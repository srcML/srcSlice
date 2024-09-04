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

std::vector<std::string> Split(const std::string& str) {
    std::vector<std::string> splitLines;
    int startOfLine = 0;
    int endOfLine = str.find('\n', startOfLine);

    // Split the lines before the final
    while (endOfLine != -1) {
        splitLines.push_back(str.substr(startOfLine, endOfLine-startOfLine));

        startOfLine = ++endOfLine;
        endOfLine = str.find('\n', startOfLine);
    }

    // Split the final line
    startOfLine = str.rfind('\n', startOfLine) + 1;
    splitLines.push_back(str.substr(startOfLine));

    return splitLines;
}

void StrLineCmp(const std::string& leftHandSide, const std::string& rightHandSide) {
    std::vector<std::string> leftSideLines = Split(leftHandSide);
    std::vector<std::string> rightSideLines = Split(rightHandSide);

    // compare line by line via strcmp
    for (size_t i = 0; i < rightSideLines.size(); ++i) {
        // Check if leftHandSide has less lines
        if (i >= leftSideLines.size()) {
            // lefthandside is missing lines
            std::cout << "\033[31m" << "[-] " << rightSideLines[i] << "\033[0m" << std::endl;
            continue;
        }

        if ( leftSideLines[i] != rightSideLines[i] ) {
            // Lines Do Not Match
            std::cout << "\033[31m" << "|  " << leftSideLines[i] << "\033[0m" << std::endl;
            std::cout << "\033[31m" << "-->" << "\033[32m" << rightSideLines[i] << "\033[0m" << std::endl;
        } else
        {
            // Lines Match
            std::cout << "\033[0m" << leftSideLines[i] << "\033[0m" << std::endl;
        }
    }
}

void DebugOutput(bool testStatus, const char* testName, const std::string& inputStr, const std::string& outputStr, std::string srcCode) {
    // if the test passed dont run extra logic
    if (testStatus) return;

    // only debug output when a test fails

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

    std::cout << "======================================================" << std::endl;
    std::cout << "\033[33m" << testName << " :: Test Source Code" << "\033[0m" << std::endl;
    std::cout << "\033[0m" << srcCode << std::endl << std::endl;

    // Diff Style Output
    StrLineCmp(inputStr, outputStr);

    std::cout << "======================================================" << std::endl;
}