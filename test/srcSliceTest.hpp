#ifndef SRCSLICETEST_HEADER
#define SRCSLICETEST_HEADER

#include <iostream>
#include <srcslicehandler.hpp>
#include <sstream>
#include <srcml.h>
#include "../src/catch2/catch.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

inline const char* ERR = "[\033[31m-\033[0m]";
inline const char* INFO = "[\033[33m*\033[0m]";
inline const char* OK = "[\033[32m+\033[0m]";

void PrintErr(const std::string testName, const std::string msg);
void PrintInfo(const std::string msg);
void PrintOk(const std::string msg);

std::string StringToSrcML(std::string str, const char* fileName);
std::string FetchSlices(const std::string cppSource, bool findControlEdges = false);

// modular checking functions
bool CheckNamespace(const std::string testName,const std::string sliceId, const json& produced, const json& expected);
bool CheckDependence(const std::string testName, const std::string sliceId, const json& produced, const json& expected);
bool CheckAliases(const std::string testName, const std::string sliceId, const json& produced, const json& expected);
bool CheckCalls(const std::string testName, const std::string sliceId, const json& produced, const json& expected);
bool CheckUses(const std::string testName, const std::string sliceId, const json& produced, const json& expected);
bool CheckDefs(const std::string testName, const std::string sliceId, const json& produced, const json& expected);
// primary check function
bool CompareJson(const std::string testName, const json& produced, const json& expected);

#endif