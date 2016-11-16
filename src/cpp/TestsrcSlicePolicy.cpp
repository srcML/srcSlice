#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <cassert>

class TestDeclType : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:
        ~TestDeclType(){}
        TestDeclType(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){}
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            decltypedata = *policy->Data<DeclTypePolicy::DeclTypeData>();
            datatotest.push_back(decltypedata);
        }
		void RunTest(){

		}
    protected:
        void * DataInner() const override {
            return (void*)0; //To silence the warning
        }
    private:

        DeclTypePolicy declpolicy;
        DeclTypePolicy::DeclTypeData decltypedata;
        std::vector<DeclTypePolicy::DeclTypeData> datatotest;
};

int main(int argc, char** filename){
	std::string codestr = "void foo(){int& abc; Object<int> onetwothree; static Object* DoReiMe; const Object* aybeecee;\n nlp::std::vector<std::string> spaces;}";
	std::string srcmlstr = codestr;
	std::cerr<<"out"<<std::endl;
    TestDeclType decltypedata;
    srcSAXController control(srcmlstr);
    srcSAXEventDispatch::srcSAXEventDispatcher<DeclTypePolicy> handler {&decltypedata};
    control.parse(&handler); //Start parsing
    decltypedata.RunTest();
}