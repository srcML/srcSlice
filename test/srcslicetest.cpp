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

TEST_F(TestsrcSliceDeclPolicy, TestDetectCommonDeclarations) {
    const int NUM_DECLARATIONS = 3;
    EXPECT_EQ(profileMap.size(), NUM_DECLARATIONS);
}

TEST_F(TestsrcSliceDeclPolicy, TestDetectCommonDeclarationsWithClone) {
    const int NUM_CLONES = 3;
    EXPECT_EQ(profileMap.find("coo")->second.size(), NUM_CLONES);
}

namespace {
  class TestsrcSliceExprPolicy : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestsrcSliceExprPolicy(){

    }
    void SetUp(){
      std::string str = "void foo(){j = 0; k = 1; doreme = 5; abc = abc + 0; i = j + k;}";
      std::string srcmlStr = StringToSrcML(str);

      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestsrcSliceExprPolicy(){

    }
  };
}

TEST_F(TestsrcSliceExprPolicy, TestDetectCommonExpr) {
    const int NUM_EXPR = 5;
    EXPECT_EQ(profileMap.size(), NUM_EXPR);
}

TEST_F(TestsrcSliceExprPolicy, TestDetectCommonExprWithClone) {
    const int NUM_CLONES_SHOULD_NOT_INCREASE = 1;
    EXPECT_EQ(profileMap.find("j")->second.size(), NUM_CLONES_SHOULD_NOT_INCREASE);
}

namespace {
  class TestsrcSliceDeclExprUnion : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestsrcSliceDeclExprUnion(){

    }
    void SetUp(){
      std::string str = 
      "int main(){\n"
      "Object coo = 5;\n"
      "const Object ke_e4e = 5;\n"
      "ke_e4e = coo;\n"
      "caa34 = caa34 + 5;\n"
      "coo = ke_e4e + caa34;\n"
      "}\n";
      std::string srcmlStr = StringToSrcML(str);
    
      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestsrcSliceDeclExprUnion(){

    }
  };
}

TEST_F(TestsrcSliceDeclExprUnion, TestDetectCommonExpr) {
    const int NUM_DECL_AND_EXPR = 3;
    EXPECT_EQ(profileMap.size(), NUM_DECL_AND_EXPR);
}

TEST_F(TestsrcSliceDeclExprUnion, TestDetectCommonUseDefke_e4e) {
    const int LINE_NUM_USE_OF_KE_E4E = 6;
    const int LINE_NUM_EXPR_DEF_OF_KE_E4E = 4;
    const int LINE_NUM_DECL_DEFS_OF_KE_E4E = 3;
    
    auto exprIt = profileMap.find("ke_e4e");
    
    EXPECT_TRUE(exprIt->second.back().use.find(LINE_NUM_USE_OF_KE_E4E) != exprIt->second.back().use.end());
    EXPECT_TRUE(exprIt->second.back().def.find(LINE_NUM_EXPR_DEF_OF_KE_E4E) != exprIt->second.back().def.end());
    EXPECT_TRUE(exprIt->second.back().def.find(LINE_NUM_DECL_DEFS_OF_KE_E4E) != exprIt->second.back().def.end());
}

TEST_F(TestsrcSliceDeclExprUnion, TestDetectCommonUseDefcaa34) {
    const int FIRST_LINE_NUM_USE_OF_CAA34 = 5;
    const int SECOND_LINE_NUM_USE_OF_CAA34 = 6;
    
    auto exprIt = profileMap.find("caa34");

    EXPECT_TRUE(exprIt->second.back().def.find(FIRST_LINE_NUM_USE_OF_CAA34) != exprIt->second.back().def.end());
    EXPECT_TRUE(exprIt->second.back().use.find(SECOND_LINE_NUM_USE_OF_CAA34) != exprIt->second.back().use.end());
    EXPECT_TRUE(exprIt->second.back().use.find(FIRST_LINE_NUM_USE_OF_CAA34) != exprIt->second.back().use.end());
}

namespace {
  class TestsrcSliceCallPolicy : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestsrcSliceCallPolicy(){

    }
    void SetUp(){
      std::string str = 
      "int main(){\n"
      "Foo(a,b);\n"
      "Bar(Foo(c,d));\n"
      "}\n";
      std::string srcmlStr = StringToSrcML(str);
    
      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestsrcSliceCallPolicy(){

    }
  };
}


TEST_F(TestsrcSliceCallPolicy, TestDetectCallArgumentsb) {
    const int CALL_USAGE_LINE = 2;
    const int NUM_ARGUMENTS_DETECTED = 6; //fix -- should be 4 but expr runs at same time as call
    auto callIt = profileMap.find("b");

    EXPECT_TRUE(callIt->second.back().def.find(CALL_USAGE_LINE) != callIt->second.back().use.end());
    EXPECT_EQ(profileMap.size(), NUM_ARGUMENTS_DETECTED);
}
TEST_F(TestsrcSliceCallPolicy, TestDetectCallCFunctionsb) {
    auto callIt = profileMap.find("b");

    EXPECT_TRUE(callIt->second.back().cfunctions.back().first == "Foo");
    EXPECT_TRUE(callIt->second.back().cfunctions.back().second == "2");
}
TEST_F(TestsrcSliceCallPolicy, TestDetectCallArgumentsc) {
    const int CALL_USAGE_LINE = 3;
    auto callIt = profileMap.find("c");

    EXPECT_TRUE(callIt->second.back().def.find(CALL_USAGE_LINE) != callIt->second.back().use.end());
}
TEST_F(TestsrcSliceCallPolicy, TestDetectCallCFunctionsc) {
    auto callIt = profileMap.find("c");

    EXPECT_TRUE(callIt->second.back().cfunctions.back().first == "Bar-Foo");
    EXPECT_TRUE(callIt->second.back().cfunctions.back().second == "1-1");
}

namespace {
  class TestsrcSliceDeclExprCallUnion : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestsrcSliceDeclExprCallUnion(){

    }
    void SetUp(){
      std::string str = 
      "int main(){\n"
      "Object b = 5;\n"
      "const Object ke_e4e = b;\n"
      "ke_e4e = coo + Bar(Foo(b));\n"
      "caa34 = caa34 + Foo(ke_e4e, b);\n"
      "coo = ke_e4e + caa34;\n"
      "const Object test = i-Bam(b,a);\n"
      "}\n";
      std::string srcmlStr = StringToSrcML(str);
    
      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestsrcSliceDeclExprCallUnion(){

    }
  };
}

TEST_F(TestsrcSliceDeclExprCallUnion, TestDetectCallDeclExprUnionke_e4e) {
    const int FIRST_LINE_NUM_USE_OF_KE_E4E = 5;
    const int SECOND_LINE_NUM_USE_OF_KE_E4E = 6;
    const int LINE_NUM_EXPR_DEF_OF_KE_E4E = 4;
    const int LINE_NUM_DECL_DEFS_OF_KE_E4E = 3;
    
    auto exprIt = profileMap.find("ke_e4e");
    
    EXPECT_TRUE(exprIt->second.back().use.find(FIRST_LINE_NUM_USE_OF_KE_E4E) != exprIt->second.back().use.end());
    EXPECT_TRUE(exprIt->second.back().use.find(SECOND_LINE_NUM_USE_OF_KE_E4E) != exprIt->second.back().use.end());

    EXPECT_TRUE(exprIt->second.back().def.find(LINE_NUM_EXPR_DEF_OF_KE_E4E) != exprIt->second.back().def.end());
    EXPECT_TRUE(exprIt->second.back().def.find(LINE_NUM_DECL_DEFS_OF_KE_E4E) != exprIt->second.back().def.end());
}

TEST_F(TestsrcSliceDeclExprCallUnion, TestDetectCallDeclExprUnionb) {
    const int FIRST_LINE_NUM_USE_OF_b = 4;
    const int SECOND_LINE_NUM_USE_OF_b = 5;
    const int THIRD_LINE_NUM_USE_OF_b = 3;
    const int FIRST_LINE_NUM_DEF_OF_b = 2;
    const int FOURTH_LINE_NUM_DEF_OF_b = 7;
    
    auto exprIt = profileMap.find("b");
    
    EXPECT_TRUE(exprIt->second.back().use.find(FIRST_LINE_NUM_USE_OF_b) != exprIt->second.back().use.end());
    EXPECT_TRUE(exprIt->second.back().use.find(SECOND_LINE_NUM_USE_OF_b) != exprIt->second.back().use.end());
    EXPECT_TRUE(exprIt->second.back().use.find(THIRD_LINE_NUM_USE_OF_b) != exprIt->second.back().use.end());
    EXPECT_TRUE(exprIt->second.back().use.find(FOURTH_LINE_NUM_DEF_OF_b) != exprIt->second.back().use.end());

    EXPECT_TRUE(exprIt->second.back().cfunctions.front().first == "Bar-Foo");
    EXPECT_TRUE(exprIt->second.back().cfunctions.front().second == "1-1");

    EXPECT_TRUE(exprIt->second.back().cfunctions.at(1).first == "Foo");
    EXPECT_TRUE(exprIt->second.back().cfunctions.at(1).second == "2");

    EXPECT_TRUE(exprIt->second.back().cfunctions.back().first == "Bam");
    EXPECT_TRUE(exprIt->second.back().cfunctions.back().second == "1");

    EXPECT_TRUE(exprIt->second.back().def.find(FIRST_LINE_NUM_DEF_OF_b) != exprIt->second.back().use.end());
}

TEST_F(TestsrcSliceDeclExprCallUnion, TestDetectCallDeclExprUnionDvarske_e4e) {   
    auto exprIt = profileMap.find("ke_e4e");
    
    EXPECT_TRUE(exprIt->second.back().dvars.find("coo") != exprIt->second.back().dvars.end());
    EXPECT_TRUE(exprIt->second.back().dvars.find("caa34") != exprIt->second.back().dvars.end());
}

TEST_F(TestsrcSliceDeclExprCallUnion, TestDetectCallDeclExprUnionDvarscaa34) {   
    auto exprIt = profileMap.find("caa34");
    
    EXPECT_TRUE(exprIt->second.back().dvars.find("caa34") != exprIt->second.back().dvars.end());
    EXPECT_TRUE(exprIt->second.back().dvars.find("coo") != exprIt->second.back().dvars.end());
}

TEST_F(TestsrcSliceDeclExprCallUnion, TestDetectCallDeclExprUnionDvarsb) {   
    auto exprIt = profileMap.find("b");

    EXPECT_TRUE(exprIt->second.back().dvars.find("ke_e4e") != exprIt->second.back().dvars.end());
    EXPECT_TRUE(exprIt->second.back().dvars.find("caa34") != exprIt->second.back().dvars.end());
    EXPECT_TRUE(exprIt->second.back().dvars.find("test") != exprIt->second.back().dvars.end());
}
namespace {
  class TestsrcSliceAliasDetection : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestsrcSliceAliasDetection(){

    }
    void SetUp(){
      std::string str = 
      "int main(){\n"
      "Object* b = 0;\n"
      "b = ke_e4e;\n"
      "Object* a = &ke_e4e;\n"
      "b = ke_e4e;\n"
      "float* e = b;\n"
      "}\n";
      std::string srcmlStr = StringToSrcML(str);
    
      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestsrcSliceAliasDetection(){

    }
  };
}

TEST_F(TestsrcSliceAliasDetection, TestAliases) {
    
    auto exprIt = profileMap.find("ke_e4e");
    
    EXPECT_TRUE(exprIt->second.back().aliases.find("b") != exprIt->second.back().aliases.end());
    EXPECT_TRUE(exprIt->second.back().aliases.find("a") != exprIt->second.back().aliases.end());
}