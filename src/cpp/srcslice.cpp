#include <srcslicepolicy.hpp>

bool validFlag(const char *arg)
{
    // flags all have - as the first char, so we want to skip non-flags
    if (arg[0] != '-')
        return true;

    // strcmp all possible flags
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
                 "./srcslice [srcML file name] [-t target] [-M Manual] [-h help] [-o output file name]\n"
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
        // ensure the argv[1] is a possible filename

        if (argc > 1)
        {
            // File check before creating the SrcSlicePolicy against
            // something that does not exist

            std::ifstream inFile(argv[1]);

            if (!inFile.is_open())
            {
                std::cerr << "Bad File: " << "\033[31m" << argv[1] << "\033[0m" << " - Does not Exist!" << std::endl;
                return 3;
            }
        } else
            {
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
                std::cerr << "Bad Argument :: " << "\033[31m" << argv[i] << "\033[0m" << std::endl;
                Usage();
                return 1;
            } else
                {
                    // Check for Duplicate Parameters/Flags

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

            // For some reason I couldnt get `argv[i] == "-h"` to return true
            // usage of strcmp (string compare) is working nicely though!
            // When both strings are equal strcmp will return a 0

            // Display help when requested in the command at any position
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                Usage();
                return 0;
            }
        }
    }

    if (argc < 2 || argc > 5)
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

                std::cout << "Output Saved to :: " << argv[i + 1] << std::endl;
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

    // Set up output save file if needed
    if (argi != -1)
        outFile = std::ofstream(argv[argi]);
    else
        outFile.close();

    // Main Operations
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    SrcSlicePolicy *cat = new SrcSlicePolicy(&profileMap);
    srcSAXController control(argv[1]);
    srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
    control.parse(&handler); // Start parsing

    for (auto it : profileMap)
    {
        for (auto profile : it.second)
        {
            if (profile.containsDeclaration)
            {
                profile.SetJsonOut(jsonOutput);

                // added the ostream overload to srcslicepolicy.hpp
                if (!hasOutFile)
                {
                    std::cout << profile;
                } else
                    {
                        outFile << profile;
                    }
            }
        }
    }

    // ensure if we do have an output file we close it
    if (argi != -1) outFile.close();
}