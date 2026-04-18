// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcSliceTest.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

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

/**
 * Converts a multi-file string setup into a srcML multi-unit composed archive
 */
std::string StringsToArchive(std::vector<std::string> contents, std::vector<std::string> filenames) {
    std::string archive_str;
    if (filenames.size() != contents.size()) return archive_str;

    struct srcml_archive* archive;
    struct srcml_unit* unit;
    
    size_t size = 0;
    char* ch;

    archive = srcml_archive_create();
    srcml_archive_enable_option(archive, SRCML_OPTION_POSITION);
    srcml_archive_write_open_memory(archive, &ch, &size);

    for (size_t i = 0; i < filenames.size(); ++i) {
        unit = srcml_unit_create(archive);
        srcml_unit_set_language(unit, SRCML_LANGUAGE_CXX);
        srcml_unit_set_filename(unit, filenames.at(i).c_str());

        srcml_unit_parse_memory(unit, contents.at(i).c_str(), contents.at(i).size());
        srcml_archive_write_unit(archive, unit);
        srcml_unit_free(unit);
    }

    srcml_archive_close(archive);
    srcml_archive_free(archive);

    ch[size-1] = 0;
    archive_str.append(ch,size);
    srcml_memory_free(ch);

    return archive_str;
}

std::string RunSrcSlice(std::string srcml, bool computeControlEdges) {
    std::ostringstream output;
    SrcSliceHandler srcSliceHandler(srcml, computeControlEdges);
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap = srcSliceHandler.GetProfileMap();

    output << "{" << std::endl;
    bool writtenSlices = false;

    for (auto& profiles : profileMap) {
        for (auto& slice : profiles.second) {
            if (!slice.containsDeclaration)
                continue;

            writtenSlices = true;
            // write out the start of the json object
            std::string name(slice.variableName + '-' + slice.declPosition.ToNameString());

            output << "\"" << name << "\":{" << std::endl;
            output << slice;
            output << "}," << std::endl;
        }
    }

    // remove trailing comma
    std::string stream2string = output.str();
    output.clear();

    // remove trailing comma
    if (writtenSlices) {
        stream2string.resize(stream2string.length() - 2);
    }

    // closing of the entire JSON object
    stream2string += "\n}";

    return stream2string;
}

std::string FetchSlices(std::string cppSource, bool findControlEdges) {
    std::string srcml = StringToSrcML(cppSource, "file.cpp");
    return RunSrcSlice(srcml, findControlEdges);
}

std::string FetchSlices(std::vector<std::string> contents, std::vector<std::string> filenames, bool findControlEdges) {
    std::string srcmlStr = StringsToArchive(contents, filenames);
    return RunSrcSlice(srcmlStr, findControlEdges);
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

std::string TestName(std::string testName) {
    tName = testName + " " + std::to_string(testNum);
    ++testNum;
    return tName;
}
std::string GetTestName() {
    return tName;
}
void ResetCount() {
    testNum = 1;
}

bool CheckCtrlEdges(const std::string testName, const std::string sliceId, const json& produced, const json& expected) {
    try {
        auto producedEdges = produced[sliceId]["controlEdges"];
        auto expectedEdges = expected[sliceId]["controlEdges"];
    
        // check types
        if (!producedEdges.is_array() || !expectedEdges.is_array()) {
            std::string msg = "Incorrect JSON data-type (not array)";
            PrintErr(testName, msg);
            return false;
        }
    
        // check sizes
        if (producedEdges.size() != expectedEdges.size()) {
            std::ostringstream osmsg;
            osmsg << "Control-Edge arrays for " << sliceId << " are different sizes" <<
            "\n |___ Produced " << producedEdges << ", expected " << expectedEdges;
            PrintErr(testName, osmsg.str());
            return false;
        }
    
        // iterate over controlEdges: [["file.cpp:2:9","file.cpp:4:9"]]
        for (const auto& edgeGroup : expectedEdges) {
            // check if the edgeGroup is within the producedEdges array
            auto it = std::find(producedEdges.begin(), producedEdges.end(), edgeGroup);
            if (it == producedEdges.end()) {
                std::ostringstream ossmsg;
                ossmsg << "(" << sliceId << ") Missing Control-Edge -> " << edgeGroup <<
                "\n |___ Produced " << producedEdges << ", expected " << expectedEdges;
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
                osmsg << "(" << sliceId << ") Missing Namespace -> " << ns <<
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
                ossmsg << "(" << sliceId << ") Missing Dependence -> " << dep <<
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
                ossmsg << "(" << sliceId << ") Missing Alias -> " << alias;
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
                ossmsg << "(" << sliceId << ") Missing Call -> " << call <<
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
                ossmsg << "(" << sliceId << ") Missing Use -> " << use <<
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
                ossmsg << "(" << sliceId << ") Missing Definition -> " << def <<
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

bool CompareJson(const std::string sourceCode, const std::string testName, const json& produced, const json& expected, bool checkEdges) {
    auto printSource = [&sourceCode](){
        std::cout << "==============================" << std::endl;
        std::cout << sourceCode << std::endl;
        std::cout << "==============================" << std::endl;
    };

    // check if all keys (varName-line-col strings) are present
    for (auto& [key,value] : expected.items()) {
        if (!produced.contains(key)) {
            std::string msg = "Missing Slice Data -> ";
            msg += key;
            PrintErr(testName, msg);

            std::cout << " |____ Produced -> ";
            for (auto& [key,value] : produced.items())
                std::cout << key << " | ";
            std::cout << std::endl;
            
                std::cout << " |____ Expected -> ";
            for (auto& [key,value] : expected.items())
                std::cout << key << " | ";
            std::cout << std::endl;
                
            printSource();
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
            "decl":{"start":"2:9","end":"2:9"},
            "dependence":[{"b":{"start":"2:9","end":"2:9"}}],
            "aliases":[{"c":{"start":"2:9","end":"2:9"}}],
            "calls":[{"functionName":"fuzz","parameter":"1","definitionPosition":{"start":"2:9","end":"2:9"},"invoke":{"start":"2:9","end":"2:9"}}],
            "use":[{"start":"2:9","end":"2:9"}],
            "definition":[{"start":"2:9","end":"2:9"}]
        */

        // Simple attribute comparison
        for (const std::string& attribute : {"file","language","class","function","type","name","decl"}) {
            auto& producedData = produced[sliceId][attribute];
            auto& expectedData = expected[sliceId][attribute];
    
            if (producedData != expectedData) {
                std::ostringstream osmsg;
                osmsg << "(" << sliceId << ") Mismatching '" << attribute << "' attributes" <<
                "\n |____ Produced " << producedData << ", expected " << expectedData;
                PrintErr(testName, osmsg.str());
                printSource();
                return false;
            }
        }

        // Complex attribute comparision
        if (!CheckUses(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }

        if (!CheckDefs(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }

        if (!CheckCalls(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }

        if (!CheckDependence(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }

        if (!CheckAliases(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }

        if (!CheckNamespace(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }

        if (checkEdges && !CheckCtrlEdges(testName, sliceId, produced, expected)) {
            printSource();
            return false;
        }
    }

    return true;
}