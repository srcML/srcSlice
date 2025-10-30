#include "srcSliceTest.hpp"

std::string StringToSrcML(std::string str, const char* fileName){ // Function by Cnewman
    struct srcml_archive* archive;
    struct srcml_unit* unit;
    
    size_t size = 0;
    char* ch;

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

    // ensure the final char is null-terminator
    ch[size-1] = 0;

    // Copy the buffer content to a string so we
    // can deallocate the buffer created by:
    // `srcml_archive_write_open_memory`
    std::string output;
    output.append(ch,size);

    srcml_memory_free(ch);
    
    return output;
}

std::string FetchSlices(const std::string cppSource, bool findControlEdges) {
    std::ostringstream output;
    std::string srcmlStr = StringToSrcML(cppSource, "file.cpp");

    SrcSliceHandler srcSliceHandler(srcmlStr, findControlEdges);
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap = srcSliceHandler.GetProfileMap();

    size_t totalElements = profileMap.size();
    size_t currIndex = 0;

    output << "{" << std::endl;
    for (auto& profiles : profileMap) {
        ++currIndex;
        for (auto& slice : profiles.second)
        {
            if (slice.containsDeclaration)
            {
                // write out the start of the json object
                std::string name(slice.variableName + '-' + slice.initialPosition.ToNameString());
                output << "\"" << name << "\":{" << std::endl;

                // print out content of the SliceProfile
                output << slice;

                // write out the end of the json object
                if (currIndex != totalElements)
                    output << "}," << std::endl;
                else
                    output << "}" << std::endl;
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

    return stream2string;
}

void PrintErr(const std::string testName, const std::string msg) {
    std::cout << ERR << " [" << testName << "] Error: " << msg << std::endl;
}
void PrintInfo(const std::string msg) {
    std::cout << INFO << " " << msg << std::endl;
}
void PrintOk(const std::string msg) {
    std::cout << OK << " " << msg << std::endl;
}

bool CheckNamespace(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedNamespaces = produced[sliceId]["namespace"];
        auto expectedNamespaces = expected[sliceId]["namespace"];
    
        // check types
        if (!producedNamespaces.is_array() || !expectedNamespaces.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedNamespaces.size() != expectedNamespaces.size()) {
            std::ostringstream osmsg;
            osmsg << "Namespace arrays for " << sliceId << " are different sizes" <<
            "\n |___ Produced " << producedNamespaces << ", expected " << expectedNamespaces;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over namespace string array
        for (const auto& ns : expectedNamespaces) {
            // check if produced namespaces are missing expected namespaces
            auto it = std::find(producedNamespaces.begin(), producedNamespaces.end(), ns);
            if (it == producedNamespaces.end()) {
                std::ostringstream osmsg;
                osmsg << "Missing Namespace -> " << ns <<
                "\n |___ Produced " << producedNamespaces << ", expected " << expectedNamespaces;
                PrintErr(testName, osmsg.str());
                return false;
            }
        }
    
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[-] Caught Exception: " << e.what() << std::endl;
        return false;
    }
}
bool CheckDependence(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedDependence = produced[sliceId]["dependence"];
        auto expectedDependence = expected[sliceId]["dependence"];
    
        // check types
        if (!producedDependence.is_array() || !expectedDependence.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedDependence.size() != expectedDependence.size()) {
            std::ostringstream osmsg;
            osmsg << "Dependence arrays for " << sliceId << " are different sizes" <<
            "\n |___ Produced " << producedDependence << ", expected " << expectedDependence;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over dependence array, ie: [{"b":{"start":"2:9","end":"2:9"}}]
        for (const auto& dep : expectedDependence) {
            // check if produced dependencies are missing expected dependencies
            auto it = std::find(producedDependence.begin(), producedDependence.end(), dep);
            if (it == producedDependence.end()) {
                std::ostringstream ossmsg;
                ossmsg << "Missing Dependence -> " << dep <<
                "\n |___ Produced " << producedDependence << ", expected " << expectedDependence;
                PrintErr(testName, ossmsg.str());
                return false;
            }
        }
    
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[-] Caught Exception: " << e.what() << std::endl;
        return false;
    }
}
bool CheckAliases(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedAliases = produced[sliceId]["aliases"];
        auto expectedAliases = expected[sliceId]["aliases"];
    
        // check types
        if (!producedAliases.is_array() || !expectedAliases.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedAliases.size() != expectedAliases.size()) {
            std::ostringstream osmsg;
            osmsg << "Aliases arrays for " << sliceId << " are different sizes" <<
            "\n |___ Produced " << producedAliases << ", expected " << expectedAliases;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over dependence array, ie: [{"c":{"start":"2:9","end":"2:9"}}]
        for (const auto& alias : expectedAliases) {
            // check if produced aliases are missing expected aliases
            auto it = std::find(producedAliases.begin(), producedAliases.end(), alias);
            if (it == producedAliases.end()) {
                std::ostringstream ossmsg;
                ossmsg << "Missing Alias -> " << alias;
                PrintErr(testName, ossmsg.str());
                return false;
            }
        }
    
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[-] Caught Exception: " << e.what() << std::endl;
        return false;
    }
}
bool CheckCalls(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedCalls = produced[sliceId]["calls"];
        auto expectedCalls = expected[sliceId]["calls"];
    
        // check types
        if (!producedCalls.is_array() || !expectedCalls.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedCalls.size() != expectedCalls.size()) {
            std::ostringstream osmsg;
            osmsg << "Calls arrays for " << sliceId << " are different sizes" <<
            "\n |___ Produced " << producedCalls << ", expected " << expectedCalls;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over calls array
        // ie: [{"functionName":"fuzz","parameter":"1","definitionPosition":{"start":"2:9","end":"2:9"},"invoke":{"start":"2:9","end":"2:9"}}]
        for (const auto& call : expectedCalls) {
            // check if produced calls are missing expected calls
            auto it = std::find(producedCalls.begin(), producedCalls.end(), call);
            if (it == producedCalls.end()) {
                std::ostringstream ossmsg;
                ossmsg << "Missing Call -> " << call <<
                "\n |___ Produced " << producedCalls;
                PrintErr(testName, ossmsg.str());
                return false;
            }
        }
    
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[-] Caught Exception: " << e.what() << std::endl;
        return false;
    }
}
bool CheckUses(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedUses = produced[sliceId]["use"];
        auto expectedUses = expected[sliceId]["use"];
    
        // check types
        if (!producedUses.is_array() || !expectedUses.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedUses.size() != expectedUses.size()) {
            std::ostringstream osmsg;
            osmsg << "Uses for " << sliceId << " arrays are different sizes" <<
            "\n |___ Produced " << producedUses << ", expected " << expectedUses;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over use array, ie: []
        for (const auto& use : expectedUses) {
            // check if produced uses are missing expected uses
            auto it = std::find(producedUses.begin(), producedUses.end(), use);
            if (it == producedUses.end()) {
                std::ostringstream ossmsg;
                ossmsg << "Missing Use -> " << use <<
                "\n |___ Produced " << producedUses;
                PrintErr(testName, ossmsg.str());
                return false;
            }
        }
    
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[-] Caught Exception: " << e.what() << std::endl;
        return false;
    }
}
bool CheckDefs(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedDefs = produced[sliceId]["definition"];
        auto expectedDefs = expected[sliceId]["definition"];
    
        // check types
        if (!producedDefs.is_array() || !expectedDefs.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedDefs.size() != expectedDefs.size()) {
            std::ostringstream osmsg;
            osmsg << "Definitions for " << sliceId << " arrays are different sizes" <<
            "\n |___ Produced " << producedDefs << ", expected " << expectedDefs;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over definition array, ie: []
        for (const auto& def : expectedDefs) {
            // check if produced is missing any expected defs
            auto it = std::find(producedDefs.begin(), producedDefs.end(), def);
            if (it == producedDefs.end()) {
                std::ostringstream ossmsg;
                ossmsg << "Missing Definition -> " << def <<
                "\n |___ Produced " << producedDefs;
                PrintErr(testName, ossmsg.str());
                return false;
            }
        }
    
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[-] Caught Exception: " << e.what() << std::endl;
        return false;
    }
}

bool CompareJson(const std::string testName, const json& produced, const json& expected) {
    // check if all keys (varName-line-col strings) are present
    for (auto& [key,value] : expected.items()) {
        if (!produced.contains(key)) {
            std::string msg = "Missing Slice Data -> ";
            msg += key;
            PrintErr(testName, msg);
            std::cout << " |____ Produced -> " << produced << std::endl;
            std::cout << " |____ Expected -> " << expected << std::endl;
            return false;
        }
    }

    // check slice data contents
    for (auto& [sliceId,sliceValue] : expected.items()) {
        // compare attributes of sliceValue
        /*
        EXAMPLE
            "file":"file.cpp",
            "language":"C++",
            "namespace":["Organizer"],
            "class":"Preparer",
            "function":"main",
            "type":"int",
            "name":"a",
            "initial":{"start":"2:9","end":"2:9"},
            "dependence":[{"b":{"start":"2:9","end":"2:9"}}],
            "aliases":[{"c":{"start":"2:9","end":"2:9"}}],
            "calls":[{"functionName":"fuzz","parameter":"1","definitionPosition":{"start":"2:9","end":"2:9"},"invoke":{"start":"2:9","end":"2:9"}}],
            "use":[{"start":"2:9","end":"2:9"}],
            "definition":[{"start":"2:9","end":"2:9"}]
        */

        // Simple attribute comparison
        for (const std::string& attribute : {"file","language","class","function","type","name","initial"}) {
            auto& producedData = produced[sliceId][attribute];
            auto& expectedData = expected[sliceId][attribute];
    
            if (producedData != expectedData) {
                std::ostringstream osmsg;
                osmsg << "Mismatching '" << attribute << "' attributes" <<
                "\n |____ Produced " << producedData << ", expected " << expectedData;
                PrintErr(testName, osmsg.str());
    
                return false;
            }
        }

        // Complex attribute comparision
        if (!CheckUses(testName, sliceId, produced, expected)) {
            return false;
        }

        if (!CheckDefs(testName, sliceId, produced, expected)) {
            return false;
        }

        if (!CheckCalls(testName, sliceId, produced, expected)) {
            return false;
        }

        if (!CheckDependence(testName, sliceId, produced, expected)) {
            return false;
        }

        if (!CheckAliases(testName, sliceId, produced, expected)) {
            return false;
        }

        if (!CheckNamespace(testName, sliceId, produced, expected)) {
            return false;
        }
    }

    return true;
}