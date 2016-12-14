#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <srcSliceProfile.hpp>
#include <cassert>
#include <fstream>
#include <streambuf>
class TestSrcSlice : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:
        ~TestSrcSlice(){}
        TestSrcSlice(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){}
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            srcslicedata = *policy->Data<srcSliceProfilePolicy::SrcProfile>();
            datatotest.push_back(srcslicedata);
        }
		void RunTest(){
            std::cout << datatotest.size() << std::endl;
            
            for(int i = 0; i < 1/*datatotest.size()*/; i++)
            {
                std::cout << "Test works" << std::endl;
                //std::cout << "Type: " << datatotest[i].nameoftype << "\nName: " << datatotest[i].nameofidentifier << std::endl;
            }

            
		}
    protected:
        void * DataInner() const override {
            return (void*)0; //To silence the warning
        }
    private:
        srcSliceProfilePolicy::SrcProfile srcslicedata;
        std::vector<srcSliceProfilePolicy::SrcProfile> datatotest;
};

int main(int argc, char** filename){
	std::string codestr = "void foo(){int& abc; Object<int> onetwothree; static Object* DoReiMe; const Object* aybeecee;\n nlp::std::vector<std::string> spaces;}";
    
    std::ifstream codestream(filename[1]);
    std::string srcmlstr;
    
    codestream.seekg(0, std::ios::end);   
    srcmlstr.reserve(codestream.tellg());
    codestream.seekg(0, std::ios::beg);
    
    srcmlstr.assign((std::istreambuf_iterator<char>(codestream)),
                std::istreambuf_iterator<char>());
    
    TestSrcSlice srcslicedata;    
    srcSAXController control(srcmlstr);
    srcSAXEventDispatch::srcSAXEventDispatcher<srcSliceProfilePolicy> handler {&srcslicedata};
    control.parse(&handler); //Start parsing
    srcslicedata.RunTest();
}
