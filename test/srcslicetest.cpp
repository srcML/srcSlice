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
    
    EXPECT_TRUE(exprIt->second.back().uses.find(LINE_NUM_USE_OF_KE_E4E) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_EXPR_DEF_OF_KE_E4E) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_DECL_DEFS_OF_KE_E4E) != exprIt->second.back().definitions.end());
}

TEST_F(TestsrcSliceDeclExprUnion, TestDetectCommonUseDefcaa34) {
    const int FIRST_LINE_NUM_USE_OF_CAA34 = 5;
    const int SECOND_LINE_NUM_USE_OF_CAA34 = 6;
    
    auto exprIt = profileMap.find("caa34");

    EXPECT_TRUE(exprIt->second.back().definitions.find(FIRST_LINE_NUM_USE_OF_CAA34) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(SECOND_LINE_NUM_USE_OF_CAA34) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(FIRST_LINE_NUM_USE_OF_CAA34) != exprIt->second.back().uses.end());
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

    EXPECT_TRUE(callIt->second.back().definitions.find(CALL_USAGE_LINE) != callIt->second.back().uses.end());
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

    EXPECT_TRUE(callIt->second.back().definitions.find(CALL_USAGE_LINE) != callIt->second.back().uses.end());
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
    
    EXPECT_TRUE(exprIt->second.back().uses.find(FIRST_LINE_NUM_USE_OF_KE_E4E) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(SECOND_LINE_NUM_USE_OF_KE_E4E) != exprIt->second.back().uses.end());

    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_EXPR_DEF_OF_KE_E4E) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_DECL_DEFS_OF_KE_E4E) != exprIt->second.back().definitions.end());
}

TEST_F(TestsrcSliceDeclExprCallUnion, TestDetectCallDeclExprUnionb) {
    const int FIRST_LINE_NUM_USE_OF_b = 4;
    const int SECOND_LINE_NUM_USE_OF_b = 5;
    const int THIRD_LINE_NUM_USE_OF_b = 3;
    const int FIRST_LINE_NUM_DEF_OF_b = 2;
    const int FOURTH_LINE_NUM_DEF_OF_b = 7;
    
    auto exprIt = profileMap.find("b");
    
    EXPECT_TRUE(exprIt->second.back().uses.find(FIRST_LINE_NUM_USE_OF_b) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(SECOND_LINE_NUM_USE_OF_b) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(THIRD_LINE_NUM_USE_OF_b) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(FOURTH_LINE_NUM_DEF_OF_b) != exprIt->second.back().uses.end());

    EXPECT_TRUE(exprIt->second.back().cfunctions.front().first == "Bar-Foo");
    EXPECT_TRUE(exprIt->second.back().cfunctions.front().second == "1-1");

    EXPECT_TRUE(exprIt->second.back().cfunctions.at(1).first == "Foo");
    EXPECT_TRUE(exprIt->second.back().cfunctions.at(1).second == "2");

    EXPECT_TRUE(exprIt->second.back().cfunctions.back().first == "Bam");
    EXPECT_TRUE(exprIt->second.back().cfunctions.back().second == "1");

    EXPECT_TRUE(exprIt->second.back().definitions.find(FIRST_LINE_NUM_DEF_OF_b) != exprIt->second.back().uses.end());
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

namespace {
  class TestParamSliceDetection : public ::testing::Test{
  public:
    std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
    TestParamSliceDetection(){

    }
    void SetUp(){
      std::string str = 
      "int main(int k, double* j, float l[]){\n"
      "j = k;\n"
      "l = 0;\n"
      "}\n";
      std::string srcmlStr = StringToSrcML(str);
    
      SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
      srcSAXController control(srcmlStr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
      control.parse(&handler);
    }
    void TearDown(){

    }
    ~TestParamSliceDetection(){

    }
  };
}

TEST_F(TestParamSliceDetection, TestParamsK) {
    const int LINE_NUM_DEF_OF_K = 1;
    const int LINE_NUM_USE_OF_K = 2;
    auto exprIt = profileMap.find("k");
    
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_DEF_OF_K) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().uses.find(LINE_NUM_USE_OF_K) != exprIt->second.back().uses.end());
    EXPECT_TRUE(exprIt->second.back().aliases.find("j") != exprIt->second.back().aliases.end());
}
TEST_F(TestParamSliceDetection, TestParamsJ) {
    const int LINE_NUM_DEF_OF_J = 1;
    const int LINE_NUM_SECOND_DEF_OF_J = 2;
    auto exprIt = profileMap.find("j");
    
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_DEF_OF_J) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_SECOND_DEF_OF_J) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().potentialAlias);
}
TEST_F(TestParamSliceDetection, TestParamsL) {
    const int LINE_NUM_DEF_OF_L = 1;
    const int LINE_NUM_SECOND_DEF_OF_L = 3;
    auto exprIt = profileMap.find("l");
    
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_DEF_OF_L) != exprIt->second.back().definitions.end());
    EXPECT_TRUE(exprIt->second.back().definitions.find(LINE_NUM_SECOND_DEF_OF_L) != exprIt->second.back().definitions.end());
}

namespace {
    class TestComputeInterprocedural : public ::testing::Test{
    public:
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        TestComputeInterprocedural(){

        }
        void SetUp(){
            std::string str =
                    "void fun(int z){\n"
                    "z++;\n"
                    "}\n"
                    "void foo(int &x, int &y){\n"
                    "fun(x);\n"
                    "y++;\n"
                    "}\n"
                    "int main() {\n"
                    "int sum = 0;\n"
                    "int i = sum;\n"
                    "while(i<=10){\n"
                    "foo(sum, i);\n"
                    "}\n"
                    "std::cout<<sum;\n"
                    "}\n";
        std::string srcmlStr = StringToSrcML(str);

            SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
            srcSAXController control(srcmlStr);
            srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
            control.parse(&handler);
        }
        void TearDown(){

        }
        ~TestComputeInterprocedural(){

        }
    };
}

TEST_F(TestComputeInterprocedural, TestSLines) {
    for(auto use : profileMap.find("x")->second.back().uses){
        EXPECT_TRUE(profileMap.find("sum")->second.back().uses.find(use) != profileMap.find("sum")->second.back().uses.end());
    }
    for(auto def : profileMap.find("x")->second.back().definitions){
        EXPECT_TRUE(profileMap.find("sum")->second.back().definitions.find(def) != profileMap.find("sum")->second.back().definitions.end());
    }
}

namespace {
    class TestComputeInterproceduralMultipleNestings : public ::testing::Test{
    public:
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        TestComputeInterproceduralMultipleNestings(){

        }
        void SetUp(){
            std::string str =
                    "void fun(int z);\n"
                    "\n"
                    "void loop(int a) {\n"
                    "    a++;\n"
                    "    fun(a);\n"
                    "}\n"
                    "\n"
                    "void fun(int z) {\n"
                    "    if(z < 10) {\n"
                    "        loop(z);\n"
                    "    }\n"
                    "}\n"
                    "\n"
                    "void bar(int y) {\n"
                    "    fun(y);\n"
                    "}\n"
                    "\n"
                    "void foo(int &x) {\n"
                    "    bar(x);\n"
                    "}\n"
                    "\n"
                    "int main() {\n"
                    "    int sum = 0;\n"
                    "    foo(sum);\n"
                    "}";
            std::string srcmlStr = StringToSrcML(str);

            SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
            srcSAXController control(srcmlStr);
            srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
            control.parse(&handler);
        }
        void TearDown(){

        }
        ~TestComputeInterproceduralMultipleNestings(){

        }
    };
}

TEST_F(TestComputeInterproceduralMultipleNestings, TestSLines) {
    for(auto use : profileMap.find("x")->second.back().uses){
        EXPECT_TRUE(profileMap.find("sum")->second.back().uses.find(use) != profileMap.find("sum")->second.back().uses.end());
    }
    for(auto def : profileMap.find("x")->second.back().definitions){
        EXPECT_TRUE(profileMap.find("sum")->second.back().definitions.find(def) != profileMap.find("sum")->second.back().definitions.end());
    }
}

namespace {
    class TestComputeControlPaths : public ::testing::Test{
    public:
        std::unordered_map<std::string, std::vector<SliceProfile>> profileMap;
        TestComputeControlPaths(){

        }
        void SetUp(){
            std::string str =
                    "int main() {\n"
                    "    int sum = 0;\n"
                    "    int i = 1;\n"
                    "    while(i<=10){\n"
                    "        sum = sum + i;\n"
                    "        i++;\n"
                    "    }\n"
                    "    std::cout<<sum;\n"
                    "    std::cout<<i;\n"
                    "}";
            std::string srcmlStr = StringToSrcML(str);

            SrcSlicePolicy* cat = new SrcSlicePolicy(&profileMap);
            srcSAXController control(srcmlStr);
            srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat});
            control.parse(&handler);
        }
        void TearDown(){

        }
        ~TestComputeControlPaths(){

        }
    };
}

TEST_F(TestComputeControlPaths, TestSLines) {
    EXPECT_TRUE(profileMap.find("sum")->second.back().controlEdges.find(std::make_pair(2, 5)) != profileMap.find("sum")->second.back().controlEdges.end());
    EXPECT_TRUE(profileMap.find("sum")->second.back().controlEdges.find(std::make_pair(2, 8)) != profileMap.find("sum")->second.back().controlEdges.end());
    EXPECT_TRUE(profileMap.find("sum")->second.back().controlEdges.find(std::make_pair(5, 8)) != profileMap.find("sum")->second.back().controlEdges.end());

    //EXPECT_TRUE(profileMap.find("i")->second.back().controlEdges.find(std::make_pair(3, 4)) != profileMap.find("i")->second.back().controlEdges.end());
    //EXPECT_TRUE(profileMap.find("i")->second.back().controlEdges.find(std::make_pair(4, 5)) != profileMap.find("i")->second.back().controlEdges.end());
    //EXPECT_TRUE(profileMap.find("i")->second.back().controlEdges.find(std::make_pair(4, 9)) != profileMap.find("i")->second.back().controlEdges.end());
    EXPECT_TRUE(profileMap.find("i")->second.back().controlEdges.find(std::make_pair(5, 6)) != profileMap.find("i")->second.back().controlEdges.end());
    EXPECT_TRUE(profileMap.find("i")->second.back().controlEdges.find(std::make_pair(6, 9)) != profileMap.find("i")->second.back().controlEdges.end());


}
