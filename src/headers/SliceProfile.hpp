#ifndef SLICEPROFILE_HPP
#define SLICEPROFILE_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <queue>
class SliceProfile;
typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<unsigned int, VarMap> FunctionVarMap;
typedef std::unordered_map<std::string, FunctionVarMap> FileFunctionVarMap;

typedef std::pair<std::string, unsigned int> NameLineNumberPair;
typedef std::pair<unsigned int, unsigned int> HashedNameLineNumberPair;

struct NameLineNumberPairHash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<U>()(x.second); //TODO: This doesn't seem like it should work well
  }
};
struct TypeNamePair{
    TypeNamePair(unsigned t, std::string n): type(t), name(n){}
    TypeNamePair(std::string n): type(0), name(n){}
    bool operator==(const TypeNamePair& tnp) const{
        return name == name;
    }
    unsigned int type;
    std::string name;
};
struct TypeNamePairHash {
public:
  template <typename T>
  std::size_t operator()(const T x) const
  {
    return std::hash<std::string>()(x.name); //Hash on the name in the pair.
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
    std::unordered_map<TypeNamePair, std::unordered_set<unsigned int>, TypeNamePairHash> memberVariables;
    std::unordered_set<FunctionData, FunctionArgtypeArgnumHash> memberFunctions; //need to handle overloads. Can't be string.
};

class SliceProfile{
	public:
		SliceProfile() = default;
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn, unsigned int sline, std::string name, std::string type, bool alias = 0, bool global = 0):
        index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name), variableType(type), isGlobal(global) {
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
        std::unordered_map<TypeNamePair, std::unordered_set<unsigned int>, TypeNamePairHash> memberVariableLineNumberMap; //{<member name, linenumber>, {linenumber}} and size of set is the number of times member was used. This map only ontains first level member useage data mem->dat
		//When an object has member variables, they can also have their own member variables. To deal with this, we create a list from the line number for this sp. Can then look up which variables were dereference.

        std::unordered_map<unsigned int, std::map<unsigned int, std::string>> lineNumberMemberVariableExtMap; //{linenumber, {type, name}} Primary purpose of this is to act as more fine-grain information for memberVariableLineNumberMap. It contains all data about member function calls of arbitrarily long chains mem->data->mem2->data
        
        std::unordered_set<HashedNameLineNumberPair, NameLineNumberPairHash> cfunctions;
		std::unordered_set<std::string> dvars;//maybe hash
		std::unordered_set<std::string> aliases;//maybe hash
};



class SystemDictionary{

	public:
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