#include <srcslicepolicy.hpp>

bool validFlag(const char *arg)
{
    // flags all have - as the first char, so we want to skip non-flags
    if (arg[0] != '-')
        return true;

    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
        return true;
    if (strcmp(arg, "-j") == 0 || strcmp(arg, "--json") == 0)
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
                 "            -j / --json              JSON output\n"
              << std::endl;
}

int main(int argc, char **argv)
{
    bool hasOutFile = false, jsonOutput = false;

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
                    // Check for Duplicate Parameters/Flags

                    // Detect output file flag
                    if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && !hasOutFile)
                    {
                        if (!hasOutFile)
                        {
                            hasOutFile = true;
                        } else
                            {
                                std::cerr << "\033[31m" << "Duplicate Parameters!" << "\033[0m" << std::endl;
                                Usage();
                                return 6;
                            }
                    }

                    // Detect JSON output flag
                    if ((strcmp(argv[i], "-j") == 0 || strcmp(argv[i], "--json") == 0))
                    {
                        if (!jsonOutput)
                        {
                            jsonOutput = true;
                        } else
                            {
                                std::cerr << "\033[31m" << "Duplicate Parameters!" << "\033[0m" << std::endl;
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
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        SrcSlicePolicy *cat = new SrcSlicePolicy(&profileMap);
        srcSAXController control(argv[1]);
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
        control.parse(&handler); // Start parsing

        // Set up output save file if needed
        if (argi != -1)
            outFile = std::ofstream(argv[argi]);
        else
            outFile.close();

        
        if (jsonOutput)
        {
            if (argi != -1)
                outFile << "{" << std::endl;
            else
                std::cout << "{" << std::endl;
        }

        size_t totalElements = profileMap.size();
        size_t currIndex = 0, sliceIndex = 0;
        std::ostringstream sliceOutput;

        for (auto it : profileMap)
        {
            ++currIndex;
            
            for (auto profile : it.second)
            {
                if (profile.containsDeclaration)
                {
                    ++sliceIndex;
                    profile.SetJsonOut(jsonOutput);
                    profile.SetSliceIndex(sliceIndex - 1);

                    sliceOutput << profile;
                    if ( jsonOutput && (currIndex != totalElements))
                        sliceOutput << "," << std::endl;
                    else
                        sliceOutput << std::endl;
                }
            }
        }

        if (jsonOutput)
        {
            sliceOutput << "}" << std::endl;
        }

        // Check for leading comma and remove it
        std::string stream2string = sliceOutput.str();

        if (stream2string[stream2string.size() - 4] == ',')
        {
            stream2string.erase(stream2string.size() - 4, 1);
        }

        if (hasOutFile)
        {
            outFile << stream2string;
        } else
            {
                std::cout << stream2string;
            }
    } catch (std::string errormsg) {
        std::cout << "\033[31m" << errormsg << "\033[0m" << std::endl;
        return 3;
    }

    // ensure if we do have an output file we close it
    if (argi != -1) {
        std::cout << "Output Saved to :: " << argv[argi] << std::endl;
        outFile.close();
    }
}