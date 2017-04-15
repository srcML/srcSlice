#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <FunctionSliceProfilePolicy.hpp>
#include <cassert>
#include <srcml.h>

std::string StringToSrcML(std::string str)
{
	struct srcml_archive* archive;
	struct srcml_unit* unit;
	size_t size = 0;

	char *ch = new char[str.size()];

	archive = srcml_archive_create();
	srcml_archive_enable_option(archive, SRCML_OPTION_POSITION);
	srcml_archive_write_open_memory(archive, &ch, &size);

	unit = srcml_unit_create(archive);
	srcml_unit_set_language(unit, SRCML_LANGUAGE_CXX);
	srcml_unit_set_filename(unit, "testsrcType.cpp");

	srcml_unit_parse_memory(unit, str.c_str(), str.size());
	srcml_archive_write_unit(archive, unit);
	
	srcml_unit_free(unit);
	srcml_archive_close(archive);
	srcml_archive_free(archive);
	//TrimFromEnd(ch, size);
	return std::string(ch);
}

class funcProfSrcSliceTest : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener
{
public:
	~funcProfSrcSliceTest(){};

	funcProfSrcSliceTest(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){};

	void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override 
    {
    	FSPpolicyData = *policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfileMap>();
    	for(auto e : FSPpolicyData.dataset)
        	datatotest.push_back(e.second);
    }

	void runTests()
	{
		assert(datatotest.size() == 5);

		auto it0 = FSPpolicyData.dataset.find("abc");
		if(it0 != FSPpolicyData.dataset.end())
		{
			assert((*it0).second.identifierType == "int");
			assert((*it0).second.linenumber == 1);
			assert((*it0).second.isConst == false);
			assert((*it0).second.isReference == true);
			assert((*it0).second.isPointer == false);
			assert((*it0).second.isStatic == false);
		}
		else
			assert(false);

		auto it1 = FSPpolicyData.dataset.find("onetwothree");
		if(it1 != FSPpolicyData.dataset.end())
		{
			assert((*it1).second.identifierType == "Object");
			assert((*it1).second.linenumber == 1);
			assert((*it1).second.isConst == false);
			assert((*it1).second.isReference == false);
			assert((*it1).second.isPointer == false);
			assert((*it1).second.isStatic == false);
		}
		else
			assert(false);

		auto it2 = FSPpolicyData.dataset.find("DoReiMe");
		if(it2 != FSPpolicyData.dataset.end())
		{
			assert((*it2).second.identifierType == "Object");
			assert((*it2).second.linenumber == 1);
			assert((*it2).second.isConst == false);
			assert((*it2).second.isReference == false);
			assert((*it2).second.isPointer == true);
			assert((*it2).second.isStatic == true);
		}
		else
			assert(false);

		auto it3 = FSPpolicyData.dataset.find("aybeecee");
		if(it3 != FSPpolicyData.dataset.end())
		{
			assert((*it3).second.identifierType == "Object");
			assert((*it3).second.linenumber == 1);
			assert((*it3).second.isConst == true);
			assert((*it3).second.isReference == false);
			assert((*it3).second.isPointer == true);
			assert((*it3).second.isStatic == false);
		}
		else
			assert(false);

		auto it4 = FSPpolicyData.dataset.find("spaces");
		if(it4 != FSPpolicyData.dataset.end())
		{
			assert((*it4).second.identifierType == "vector");
			assert((*it4).second.linenumber == 2);
			assert((*it4).second.isConst == false);
			assert((*it4).second.isReference == false);
			assert((*it4).second.isPointer == false);
			assert((*it4).second.isStatic == false);
		}
		else
			assert(false);
	}

    protected:
    void *DataInner() const override 
    {
        return (void*)0; 
    }

private:
	FunctionSliceProfilePolicy::FunctionSliceProfileMap FSPpolicyData;

	std::vector<FunctionSliceProfilePolicy::FunctionSliceProfile> datatotest;
};

int main(int argc, char **filename)
{
	std::string codestr = "void foo(){int& abc; Object<int> onetwothree; static Object* DoReiMe; const Object* aybeecee;\n nlp::std::vector<std::string> spaces;}";
	std::string srcmlstr = StringToSrcML(codestr);

    funcProfSrcSliceTest fpsstestdata;
    
    srcSAXController control(srcmlstr);
    
    srcSAXEventDispatch::srcSAXEventDispatcher<FunctionSliceProfilePolicy> handler {&fpsstestdata};

    control.parse(&handler); 
    
    fpsstestdata.runTests();
}