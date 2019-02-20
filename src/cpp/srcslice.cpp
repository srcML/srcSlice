#include <srcslicepolicy.hpp>
int main(int argc, char** argv){
        if(argc < 2){
        	std::cerr<<"Syntax: ./srcslice [srcML file name]"<<std::endl;
        	return 0;
        }
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
        srcSAXController control(argv[1]);
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
        control.parse(&handler); //Start parsing
        for(auto it : profileMap){
        	for(auto profile : it.second){
            	profile.PrintProfile();
        	}
        }
}