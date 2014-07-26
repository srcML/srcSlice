#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

class SliceProfile{
	public:
		SliceProfile() = default;
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn, unsigned int sline){
			index = idx;
			file = fle;
			function = fcn;
			slines.push_back(sline);
		}
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn){
			index = idx;
			file = fle;
			function = fcn;
		}
		SliceProfile(unsigned int num){
			index = num;
			file = num;
			function = num;
		}
		unsigned int index;
		unsigned int file;
		unsigned int function;
		std::string variableName;
		std::vector<unsigned int> slines;
		std::vector<std::pair<unsigned int, unsigned int>> cfunctions;
		std::deque<unsigned int> dvars;//maybe hash
		std::deque<unsigned int> aliases;//maybe hash
};
class SystemDictionary{
	public:
		//per file system dictionary
		/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
		std::unordered_map<unsigned int, std::unordered_map<unsigned int, std::unordered_map<unsigned int, SliceProfile>>> dictionary;
		
		std::unordered_map<unsigned int, std::string> fileTable;
		
		std::unordered_map<unsigned int, std::string> functionTable;
		std::unordered_map<unsigned int, std::string> nameTable;

		std::unordered_map<std::string, unsigned int> reverseNameTable;
		std::unordered_map<std::string, unsigned int> reverseFunctionTable;
};