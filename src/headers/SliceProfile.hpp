#ifndef SLICEPROFILE_HPP
#define SLICEPROFILE_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
class SliceProfile;
typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<unsigned int, VarMap> FunctionVarMap;
typedef std::unordered_map<std::string, FunctionVarMap> FileFunctionVarMap;
typedef std::pair<std::string, unsigned int> NameLineNumberPair;
typedef std::pair<unsigned int, unsigned int> HashedNameLineNumberPair;
typedef std::pair<std::string, std::string> TypeNamePair;

struct NameLineNumberPairHash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<U>()(x.second);
  }
};
struct FunctionArgtypeArgnumHash {
public:
  template <typename T>
  std::size_t operator()(const T x) const
  {
    return std::hash<unsigned int>()(x.GetFunctionUID());
  }
};
struct FunctionData{
    FunctionData(){
        functionNumber = 0;
        functionLineNumber = 0;
    }
    void clear(){
        returnType.clear();
        functionName.clear();
    }
    bool operator==(const FunctionData& d) const{
        return d.functionName == functionName; //need to define proper op== for the hash
    }
    unsigned int GetFunctionUID() const{
        return functionNumber + params.size()+ functionHash;
    }
    std::string returnType;
    std::string functionName;

    std::vector<unsigned int> params; //size of vector is # of arguments. String is type of argument.
    
    unsigned int functionNumber; //obtained from adding hashes of types of arguments TODO: CHANGE NAME
    unsigned int functionHash; //obtained from hashing name of function TODO: CHANGE NAME
    unsigned int functionLineNumber;
};



struct ClassProfile{
    std::string className;
    std::unordered_set<std::string> memberVariables;
    std::unordered_set<FunctionData, FunctionArgtypeArgnumHash> memberFunctions; //need to handle overloads. Can't be string.
};

class SliceProfile{
	public:
		SliceProfile() = default;
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn, unsigned int sline, std::string name, bool alias = 0, bool global = 0):
        index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name),isGlobal(global) {
			slines.insert(sline);
            dereferenced = false;
		}

		unsigned int index;
		unsigned int file;
		unsigned int function;
		
        std::unordered_set<std::string>::iterator lastInsertedAlias;

		bool potentialAlias;
        bool dereferenced;

        bool isGlobal;
        bool visited;

		std::string variableName;
		std::string variableType;
		std::unordered_set<std::string> memberVariables;
		std::unordered_set<unsigned int> slines; //Deprecated
        
        std::set<unsigned int> def;
        std::set<unsigned int> use;
		
        std::unordered_set<HashedNameLineNumberPair, NameLineNumberPairHash> cfunctions;
		std::unordered_set<std::string> dvars;//maybe hash
		std::unordered_set<std::string> aliases;//maybe hash
};



struct SystemDictionary{
	//per file system dictionary
	/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
	FileFunctionVarMap dictionary;
	VarMap globalMap;
    std::unordered_map<std::string, ClassProfile> classTable;
	std::unordered_map<unsigned int, std::string> functionTable;
    std::unordered_map<unsigned int, std::string> typeTable;
    std::vector<std::pair<unsigned int, unsigned int>> controledges;
};
#endif