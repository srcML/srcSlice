// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file main.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

#include <srcslicehandler.hpp>

// determine default max threads
int defaultThreads() {
    int processorCount = CPUCount();
    if (processorCount < 2)
        return 2;
    else if (processorCount == 2)
        return 3;
    else
        return processorCount + 2;
}

int main(int argc, char **argv)
{
    // search for the output flag and set a marker
    std::string inputFile = "", outputFile = "";
    int threadCount = defaultThreads();
    std::ofstream outFile;
    bool verboseMode = false, feedbackMode = false,
        showControlEdges = false, expandCalls = false;

    std::ostringstream ss;
    ss << "Number of concurrent threads [Default " << threadCount << "]";

    CLI::App app{"srcSlice (srcML Slicing Tool)"};
    // Options hold extra data
    app.add_option  ("input",   inputFile,                          "Name of the srcML input file [Must be built using the --position and --hash flags]")
        ->required()
        ->type_name("srcML FILE");
    app.add_option  ("-o, --output",  outputFile,                   "Name of the JSON output file [Stdout is Default]");
    app.add_option  ("-t, --threads", threadCount,                  ss.str())
        ->default_val(threadCount);
    
    app.add_flag    ("-c, --control-edges", showControlEdges,       "Display Control-Edges of the Slice");
    app.add_flag    ("-e, --expand-calls", expandCalls,             "Generate more details in cfunction entries");
    app.add_flag    ("-v, --verbose", verboseMode,                    "Display Debug Info when Slicing");
    app.add_flag    ("-p, --progress", feedbackMode,                "Display Feedback Progress Bars");
    
    CLI11_PARSE(app, argc, argv);

    CliInfo info = {
        inputFile, outputFile,
        threadCount,
        showControlEdges,
        expandCalls,
        verboseMode,
        feedbackMode
    };

    try {
        // check if input file exists
        std::ifstream srcmlFile(inputFile);
        if (!srcmlFile.is_open()) {
            throw std::runtime_error("Input file does not exist");
        }
        srcmlFile.close();

        if (showControlEdges) {
            std::cout << "[\033[33mWARNING\033[0m] Computing control-edges may increase the time required to complete the program\n";
        }

        SrcSliceHandler srcSliceHandler(info);

        auto sliceProfileMap = srcSliceHandler.GetProfileMap();

        std::ostringstream sliceOutput;

        if (!outputFile.empty()) {
            // allow output file to be written in
            outFile = std::ofstream(outputFile);
        } else {
            // close output file
            outFile.close();
        }

        // opening of the entire JSON object
        sliceOutput << "{" << std::endl;
        bool writtenSlices = false;

        for (auto& profiles : sliceProfileMap) {
            for (auto& slice : profiles.second) {
                if (!slice.containsDeclaration)
                    continue;

                // if we need more call details in the call entries
                // we need to set the respective bool true
                slice.expandCalls = expandCalls;

                writtenSlices = true;
                std::string name(slice.variableName + '-' + slice.declPosition.ToNameString() + '-' + slice.checksum);

                sliceOutput << "\"" << name << "\":{" << std::endl;
                sliceOutput << slice;
                sliceOutput << "}," << std::endl;
            }
        }

        // remove trailing comma
        std::string stream2string = sliceOutput.str();
        sliceOutput.clear();

        // remove trailing comma
        if (writtenSlices) {
            stream2string.resize(stream2string.length() - 2);
        }

        // closing of the entire JSON object
        stream2string += "\n}";

        // write to either stdout or output file
        if (!outputFile.empty()) {
            std::cout << "Writing to :: " << outputFile << std::endl;
            outFile << stream2string;
        } else {
            std::cout << stream2string;
        }
    } catch (const std::exception& e) {
        std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
        return 3;
    }

    // ensure if we do have an output file we close it
    // alert user that an output file has been created
    if (!outputFile.empty()) {
        std::cout << "Output Saved to :: " << outputFile << std::endl;
        outFile.close();
    }
}