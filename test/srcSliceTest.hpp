#ifndef SRCSLICETEST_HEADER
#define SRCSLICETEST_HEADER

#include <iostream>
#include <srcslicepolicy.hpp>
#include <sstream>
#include <srcml.h>
#include "../src/catch2/catch.hpp"

std::string StringToSrcML(std::string str, const char* fileName);
std::string FetchSlices(const std::string cppSource, const char* fileName);
bool PromptVerbose();
void DebugOutput(bool verboseMode, bool testFailed, const char* testName, const std::string& inputStr, const std::string& outputStr);

#endif