#include <srcslicepolicy.hpp>
int main(){
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
        srcSAXController control("kdevelop.cpp.xml");
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
        control.parse(&handler); //Start parsing
        for(auto it : profileMap){
        	for(auto profile : it.second){
            	profile.PrintProfile();
        	}
        }
}