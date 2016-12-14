#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <srcSliceProfile.hpp>
#include <cassert>

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
	std::string srcmlstr = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><unit xmlns=\"http://www.srcML.org/srcML/src\" xmlns:cpp=\"http://www.srcML.org/srcML/cpp\" revision=\"0.9.5\" language=\"C++\" filename=\"testFile.hpp\"><function><type><name>void</name></type> <name>foo</name><parameter_list>()</parameter_list><block>{<decl_stmt><decl><type><name>int</name><modifier>&amp;</modifier></type> <name>abc</name></decl>;</decl_stmt> <decl_stmt><decl><type><name><name>Object</name><argument_list type=\"generic\">&lt;<argument><expr><name>int</name></expr></argument>&gt;</argument_list></name></type> <name>onetwothree</name></decl>;</decl_stmt> <decl_stmt><decl><specifier>static</specifier> <type><name>Object</name><modifier>*</modifier></type> <name>DoReiMe</name></decl>;</decl_stmt> <decl_stmt><decl><type><specifier>const</specifier> <name>Object</name><modifier>*</modifier></type> <name>aybeecee</name></decl>;</decl_stmt><decl_stmt><decl><type><name><name>nlp</name><operator>::</operator><name>std</name><operator>::</operator><name><name>vector</name><argument_list type=\"generic\">&lt;<argument><expr><name><name>std</name><operator>::</operator><name>string</name></name></expr></argument>&gt;</argument_list></name></name></type> <name>spaces</name></decl>;</decl_stmt>}</block></function></unit>";
	std::cerr<<"out"<<std::endl;
    TestSrcSlice srcslicedata;    
    srcSAXController control(srcmlstr);
    srcSAXEventDispatch::srcSAXEventDispatcher<srcSliceProfilePolicy> handler {&srcslicedata};
    std::cerr<<"Before"<<std::endl; 
    control.parse(&handler); //Start parsing
    std::cout << "HERE HERE HERE HERE" << std::endl;
    srcslicedata.RunTest();
}
