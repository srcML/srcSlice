#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <SrcSlicePolicy.hpp>
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
    	decltypedata = *policy->Data<DeclTypePolicy::DeclTypeData>();
        datatotest.push_back(decltypedata);
    }

	void runTests()
	{
		assert(datatotest.size() == 5);
		assert(datatotest[0].nameoftype == "int");
		assert(datatotest[0].nameofidentifier == "abc");
		assert(datatotest[0].linenumber == 1);
		assert(datatotest[0].isConst == false);
		assert(datatotest[0].isReference == true);
		assert(datatotest[0].isPointer == false);
		assert(datatotest[0].isStatic == false);
		assert(datatotest[0].namespaces.empty());

		assert(datatotest[1].nameoftype == "Object");
		assert(datatotest[1].nameofidentifier == "onetwothree");
		assert(datatotest[1].linenumber == 1);
		assert(datatotest[1].isConst == false);
		assert(datatotest[1].isReference == false);
		assert(datatotest[1].isPointer == false);
		assert(datatotest[1].isStatic == false);
		assert(datatotest[1].namespaces.empty());

		assert(datatotest[2].nameoftype == "Object");
		assert(datatotest[2].nameofidentifier == "DoReiMe");
		assert(datatotest[2].linenumber == 1);
		assert(datatotest[2].isConst == false);
		assert(datatotest[2].isReference == false);
		assert(datatotest[2].isPointer == true);
		assert(datatotest[2].isStatic == true);
		assert(datatotest[2].namespaces.empty());

		assert(datatotest[3].nameoftype == "Object");
		assert(datatotest[3].nameofidentifier == "aybeecee");
		assert(datatotest[3].linenumber == 1);
		assert(datatotest[3].isConst == true);
		assert(datatotest[3].isReference == false);
		assert(datatotest[3].isPointer == true);
		assert(datatotest[3].isStatic == false);
		assert(datatotest[3].namespaces.empty());

		assert(datatotest[4].nameoftype == "vector");
		assert(datatotest[4].nameofidentifier == "spaces");
		assert(datatotest[4].linenumber == 2);
		assert(datatotest[4].isConst == false);
		assert(datatotest[4].isReference == false);
		assert(datatotest[4].isPointer == false);
		assert(datatotest[4].isStatic == false);
		assert(datatotest[4].namespaces.size() == 2);
	}

    protected:
    void *DataInner() const override 
    {
        return (void*)0; 
    }

private:
	// FunctionSliceProfilePolicy declpolicyP;
	// SrcSlicePolicy declpolicyS;

	DeclTypePolicy::DeclTypeData decltypedata;

	std::vector<DeclTypePolicy::DeclTypeData> datatotest;
};

int main(int argc, char **filename)
{
	std::string codestr = "void foo(){int& abc; Object<int> onetwothree; static Object* DoReiMe; const Object* aybeecee;\n nlp::std::vector<std::string> spaces;}";
	std::string srcmlstr = StringToSrcML(codestr);

    funcProfSrcSliceTest decltypedata;
    
    srcSAXController control(srcmlstr);
    
    srcSAXEventDispatch::srcSAXEventDispatcher<SrcSlicePolicy> handler {&decltypedata};

    control.parse(&handler); 
    
    decltypedata.runTests();
}