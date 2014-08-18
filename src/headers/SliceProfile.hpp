#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct ExprStmt{
    ExprStmt(){
        ln = 0;
        opeq = false;
    }
    void clear(){
        lhs.clear();
        op.clear();
        rhs.clear();
    }
    bool opeq; //flag to tell me when I've seen an assignment op
    std::string lhs;
    std::string op;
    std::string rhs;
    unsigned int ln;
};
struct DeclStmt{
    DeclStmt(){
        ln = 0;
        potentialAlias = false;
    }
    void clear(){
        type.clear();
        name.clear();
        potentialAlias = false;
    }    
    bool potentialAlias; //flag to tell me when the last name could be used as an alias.
    std::string type;
    std::string name;
    unsigned int ln;
};
struct FunctionData{
    FunctionData(){
        functionNumber = 0;
        functionLineNumber = 0;
    }
    void clear(){
        returnType.clear();
        functionName.clear();
        arguments.clear();
        exprstmt.clear();
        declstmt.clear();
        arg.clear();
    }
    std::string returnType;
    std::string functionName;        
    std::vector<DeclStmt> arguments;
    DeclStmt arg;

    ExprStmt exprstmt; //might be better to remove this and declstmt and have them separate from this class
    DeclStmt declstmt;
    
    std::string op;
    
    unsigned int functionNumber;
    unsigned int functionLineNumber;
};
class SliceProfile{
	public:
		SliceProfile() = default;
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn, unsigned int sline, std::string name, bool alias = 0){
			index = idx;
			file = fle;
			function = fcn;
			slines.insert(sline);
			potentialAlias = alias;
			variableName = name;
			isAlias = false;
		}
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn, std::string name, bool alias){
			index = idx;
			file = fle;
			function = fcn;
			potentialAlias = alias;
			variableName = name;
			isAlias = false;
		}
		SliceProfile(unsigned int num){
			index = num;
			file = num;
			function = num;
			isAlias = false;
		}
		unsigned int index;
		unsigned int file;
		unsigned int function;
		
        std::unordered_set<std::string>::iterator lastInsertedAlias;

		bool isAlias;
		bool potentialAlias;

		std::string variableName;
		std::string variableType;
		
		std::unordered_set<unsigned int> slines;
		std::vector<std::pair<std::string, unsigned int>> cfunctions;
		std::unordered_set<std::string> dvars;//maybe hash
		std::unordered_set<std::string> aliases;//maybe hash
};

typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<unsigned int, VarMap> FunctionVarMap;
typedef std::unordered_map<unsigned int, FunctionVarMap> FileFunctionVarMap;
typedef std::pair<std::string, unsigned int> NameLineNumberPair;
typedef std::pair<std::string, std::string> TypeNamePair;

class SystemDictionary{

	public:
		//per file system dictionary
		/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
		FileFunctionVarMap dictionary;
		
		std::unordered_map<unsigned int, std::string> fileTable;
		
		std::unordered_map<unsigned int, FunctionData> functionTable;
};