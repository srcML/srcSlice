#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

class SliceProfile{
	public:
		SliceProfile();	
	private:
		int index;
		int file;
		int function;
		std::string variableName;
		std::vector<int> slines;
		std::vector<int> cfunctions;
		std::deque<std::string> dvars;//maybe hash
		std::deque<std::string> aliases;//maybe hash
};
class SystemDictionary{
	public:
		SystemDictionary();
	
	private:
		/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
		std::unordered_map<unsigned int, std::deque<std::pair<unsigned int, std::deque<SliceProfile>>>> sDictionary;
		std::unordered_map<unsigned int, std::string> fileNames;
		std::unordered_map<unsigned int, std::string> functionNames;
};