#include <srcml.h>
#include <gtest/gtest.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <srcslicepolicy.hpp>

std::string StringToSrcML(std::string str){
    struct srcml_archive* archive;
    struct srcml_unit* unit;
    size_t size = 0;

    char *ch = 0;

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

    ch[size-1] = 0;
    
    return std::string(ch);
}

namespace {
  class TestsrcSliceDeclPolicy : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestsrcSliceDeclPolicy(){

    }
    void SetUp(){
      std::string str = "int main(){Object coo = 5; const Object ke_e4e = 5; static const Object caa34 = 5; Object coo = 5; Object coo = 5;}";
      std::string srcmlStr = StringToSrcML(str);
  
      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestsrcSliceDeclPolicy(){

    }
  };
}

TEST_F(TestsrcSliceDeclPolicy, TesstDetectCommonDeclarations) {
    const int NUM_DECLARATIONS = 3;
    EXPECT_EQ(profileMap.size(), NUM_DECLARATIONS);
}

TEST_F(TestsrcSliceDeclPolicy, TesstDetectCommonDeclarationsWithClone) {
    const int NUM_CLONES = 3;
    EXPECT_EQ(profileMap.find("coo")->second.size(), NUM_CLONES);
}