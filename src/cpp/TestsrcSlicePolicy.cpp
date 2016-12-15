#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <SrcSlicePolicy.hpp>
#include <FunctionSliceProfilePolicy.hpp>
#include <cassert>
#include <fstream>
#include <streambuf>
class TestSrcSlice : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:
        ~TestSrcSlice(){}
        TestSrcSlice(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){}
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            srcslicedata = *policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfile>();
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
        FunctionSliceProfilePolicy::FunctionSliceProfile srcslicedata;
        std::vector<FunctionSliceProfilePolicy::FunctionSliceProfile> datatotest;
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
    
    std::cerr<<srcmlstr<<std::endl;
    TestSrcSlice srcslicedata;    
    srcSAXController control(srcmlstr);
    srcSAXEventDispatch::srcSAXEventDispatcher<FunctionSliceProfilePolicy> handler {&srcslicedata};
    control.parse(&handler); //Start parsing
    srcslicedata.RunTest();
}
