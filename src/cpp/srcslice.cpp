#include <srcslicepolicy.hpp>
int main(){
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
        srcSAXController control("telegram.cpp.xml");
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
        control.parse(&handler); //Start parsing
        for(auto it : profileMap){
            std::cerr<<it.first<<" "<<it.second.size()<<std::endl;
        }
}