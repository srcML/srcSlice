#include <srcslicehandler.hpp>

bool validFlag(const char *arg)
{
    // flags all have - as the first char, so we want to skip non-flags
    if (arg[0] != '-')
        return true;

    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
        return true;
    if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0)
        return true;

    return false;
}

void Usage()
{
    std::cout << "Src-Slice (srcml Slicing Tool)\n"
                 "./srcslice [srcML file name] [-j json] [-h help] [-o output file name]\n"
                 "  \033[31m NOTE - srcML input file should be built using the --position flag! \033[0m \n"
                 "        Command Summary:\n"
                 "            -h / --help              Shows this Page\n"
                 "            -o / --output            Output File Name\n"
              << std::endl;
}

int main(int argc, char **argv)
{
    bool hasOutFile = false;

    {
        if (argc < 2) {
            std::cerr << "\033[31m" << "Not Enough Parameters!" << "\033[0m" << std::endl;
            Usage();
            return -1;
        }
    }

    {
        // Flag Parameter Handling
        for (int i = 1; i < argc; ++i)
        {
            if (!validFlag(argv[i]))
            {
                // Quit when bad flags are present
                std::cerr << "Bad Argument :: " << "\033[31m" << argv[i] << "\033[0m" << std::endl;
                Usage();
                return 1;
            } else
                {
                    // Check for DuplisrcSliceHandlere Parameters/Flags

                    // Detect output file flag
                    if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && !hasOutFile)
                    {
                        if (!hasOutFile)
                        {
                            hasOutFile = true;
                        } else
                            {
                                std::cerr << "\033[31m" << "DuplisrcSliceHandlere Parameters!" << "\033[0m" << std::endl;
                                Usage();
                                return 6;
                            }
                    }
                }

            // Display help when requested in the command at any position
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                Usage();
                return 0;
            }
        }
    }

    // Quit when too many parameters exist in the command
    if (argc > 5)
    {
        std::cerr << "\033[31m" << "Too Many Parameters!" << "\033[0m" << std::endl;
        Usage();
        return 2;
    }

    // search for the output flag and set a marker
    int argi = -1;
    std::ofstream outFile;

    for (int i = 0; i < argc; ++i)
    {
        if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0))
        {
            if (i + 1 < argc)
            {
                // extremely unlikely for a filename to start with '-'
                if (argv[i + 1][0] == '-')
                {
                    // error due to missing output name
                    std::cerr << "\033[31m" << "-o/--output takes one parameter!" << "\033[0m" << std::endl;
                    Usage();
                    return 5;
                }

                argi = i + 1;
                break;
            } else
                {
                    // error due to missing output name
                    std::cerr << "\033[31m" << "-o/--output takes one parameter!" << "\033[0m" << std::endl;
                    Usage();
                    return 5;
                }
        }
    }

    try {
        SrcSliceHandler srcSliceHandler(argv[1]);

        auto sliceProfileMap = srcSliceHandler.GetProfileMap();

        size_t totalElements = sliceProfileMap.size();
        size_t currIndex = 0, sliceIndex = 0;
        std::ostringstream sliceOutput;

        if (argi != -1) {
            // allow output file to be written in
            outFile = std::ofstream(argv[argi]);
        } else {
            // close output file
            outFile.close();
        }

        std::map<std::string, unsigned int> sliceProfileNames;

        // opening of the entire JSON object
        sliceOutput << "{" << std::endl;

        for (auto& profile : sliceProfileMap)
        {
            ++currIndex;
            
            for (auto& slice : profile.second)
            {
                if (slice.containsDeclaration)
                {
                    // write out the start of the json object
                    // build json slice profile name
                    // variableName_initDefLine_checksum_numericalTagger

                    std::string name(slice.variableName + '_' + std::to_string(slice.lineNumber) + '_' + slice.checksum);
                    ++sliceProfileNames[name]; // if the name exists in the map we will see the value increase
                    // if the numeric tag after substracting 1 is not 0 we need to include this tag
                    // in the slice name when we pipe to stdout
                    if (sliceProfileNames[name]-1 != 0) name += '_' + sliceProfileNames[name];

                    sliceOutput << "\"" << name << "\" : {" << std::endl;

                    // print out content of the SliceProfile
                    sliceOutput << slice;

                    // write out the end of the json object
                    if (currIndex != totalElements)
                        sliceOutput << "}," << std::endl;
                    else
                        sliceOutput << "}" << std::endl;
                }
            }
        }

        // closing of the entire JSON object
        sliceOutput << "}" << std::endl;

        // Check for leading comma and remove it
        std::string stream2string = sliceOutput.str();

        if (stream2string[stream2string.size() - 4] == ',') {
            stream2string.erase(stream2string.size() - 4, 1);
        }

        // write to either stdout or output file
        if (hasOutFile) {
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
    if (argi != -1) {
        std::cout << "Output Saved to :: " << argv[argi] << std::endl;
        outFile.close();
    }
}