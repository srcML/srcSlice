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
    bool debugMode = false, feedbackMode = false, showControlEdges = false;

    std::ostringstream ss;
    ss << "Number of concurrent threads [Default " << threadCount << "]";

    CLI::App app{"srcSlice (srcML Slicing Tool)"};
    // Options hold extra data
    app.add_option  ("-i, --input",   inputFile,                    "Name of the srcML input file [Must be built using the --position and --hash flags]")
        ->required()
        ->type_name("srcML FILE");
    app.add_flag    ("-c, --control-edges", showControlEdges,       "Display Control-Edges of the Slice");
    app.add_option  ("-o, --output",  outputFile,                   "Name of the JSON output file [Stdout is Default]");
    app.add_option  ("-t, --threads", threadCount,               ss.str())
        ->default_val(threadCount);
    app.add_flag    ("-v, --verbose", debugMode,                    "Display Debug Info when Slicing");
    app.add_flag    ("-p, --progress", feedbackMode,                "Display Feedback Progress Bars");
    
    CLI11_PARSE(app, argc, argv);

    try {
        SrcSliceHandler srcSliceHandler(inputFile.c_str(), debugMode, feedbackMode, showControlEdges, threadCount);

        auto sliceProfileMap = srcSliceHandler.GetProfileMap();

        size_t totalElements = sliceProfileMap.size();
        size_t currIndex = -1;
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

        for (auto& profile : sliceProfileMap) {
            ++currIndex;
            
            for (auto& slice : profile.second) {
                if (slice.containsDeclaration) {
                    std::string name(slice.variableName + '-' + slice.initialPosition.ToNameString() + '-' + slice.checksum);
                    sliceOutput << "\"" << name << "\":{" << std::endl;
                    sliceOutput << slice;
                    sliceOutput << "}," << std::endl;
                }
            }
        }

        // remove trailing comma
        std::string stream2string = sliceOutput.str();
        stream2string.resize(stream2string.length() - 2);

        // closing of the entire JSON object
        stream2string += "\n}";

        // write to either stdout or output file
        if (!outputFile.empty()) {
            std::cout << "Writing to :: " << outputFile << std::endl;
            outFile << stream2string;
        } else {
            std::cout << stream2string;
        }
    } catch (std::string errormsg) {
        std::cout << "\033[31m" << errormsg << "\033[0m" << std::endl;
        return 3;
    }

    // ensure if we do have an output file we close it
    // alert user that an output file has been created
    if (!outputFile.empty()) {
        std::cout << "Output Saved to :: " << outputFile << std::endl;
        outFile.close();
    }
}