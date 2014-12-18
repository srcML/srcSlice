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
typedef std::pair<std::string, std::string> TypeNamePair;

struct FunctionData{
    FunctionData(){
        functionNumber = 0;
        functionLineNumber = 0;
    }
    void clear(){
        returnType.clear();
        functionName.clear();
    }
    std::string returnType;
    std::string functionName;        
    
    
    unsigned int functionNumber;
    unsigned int functionLineNumber;
};

struct NameLineNumberPairHash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<U>()(x.second);
  }
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
		
		std::unordered_set<unsigned int> slines; //Deprecated
        
        std::set<unsigned int> def;
        std::set<unsigned int> use;
		
        std::unordered_set<NameLineNumberPair, NameLineNumberPairHash> cfunctions;
		std::unordered_set<std::string> dvars;//maybe hash
		std::unordered_set<std::string> aliases;//maybe hash
};



class SystemDictionary{

	public:
		//per file system dictionary
		/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
		FileFunctionVarMap dictionary;
		VarMap globalMap;
		std::unordered_map<unsigned int, std::string> fileTable;
		std::unordered_map<unsigned int, FunctionData> functionTable;
        std::vector<std::pair<unsigned int, unsigned int>> controledges;
};
#endif