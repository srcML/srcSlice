#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
// #include <unordered_map>
// #include <unordered_set>
#include "../headers/SrcSlicePolicy.hpp"
//#include <SrcSlicePolicy.hpp>
//#include <FunctionSliceProfilePolicy.hpp>
#include <cassert>
#include <srcml.h>
#include <fstream>
#include <streambuf>
#include <iostream>

std::string filetostring (const std::string file){
  std::ifstream t(file);
  std::string str;
  
  t.seekg(0, std::ios::end); 
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);
  
  str.assign((std::istreambuf_iterator<char>(t)),
              std::istreambuf_iterator<char>());
  return str;
}

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

struct SrcSlice : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener, public srcSAXEventDispatch::EventListener {

  SrcSlice(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners)
  {

  };

  ~SrcSlice(){};

  void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override 
    {
      // SSPolicyData = *policy->Data<SrcSlicePolicy::SliceProfileSet>();
      // for(auto e : SSPolicyData.varNameProf) {
      //   FSprofiles.push_back(e.second);
      // }
      FSPpolicyData = *policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfileMap>();
      for(auto e : FSPpolicyData.dataset)
          FSprofiles.push_back(e.second);

    }

  void *DataInner() const override 
    {
        return (void*)0; 
    }

  void display() {

    for(auto profile : FSprofiles) {
      std::cerr << "var: " << profile.identifierName << " defined on lines: ";
      for(auto ln : profile.def) {
        std::cerr << ln << ", ";
      }
      std::cerr << "\n";

      std::cerr << "var: " << profile.identifierName << " used on lines: ";
      for(auto ln : profile.use) {
        std::cerr << ln << ", ";
      }
      std::cerr << "\n";
      
      std::cerr << "var: " << profile.identifierName << " has some effect on: ";
      for(auto dvar : profile.dvars) {
        std::cerr << dvar << ", ";
      } 
      std::cerr << "\n\n";
    }





    // for(auto e : srcData.varNameProf) {

    //   // std::string msg = "";
      
    //   // std::string fileName = e.first;
    //   // std::string identifierName = e.second.identifierName;
    //   // std::string identifierType = e.second.identifierType;
    //   // std::string functionName = "functionName";

    //   // std::string::size_type i = fileName.find(identifierName);
    //   // while (i != std::string::npos) {
    //   //   fileName.erase(i, identifierName.length());
    //   //   i = fileName.find(identifierName, i);
    //   // }

    //   // std::string::size_type j = fileName.find(identifierType);
    //   // while (j != std::string::npos) {
    //   //   fileName.erase(j, identifierType.length());
    //   //   j = fileName.find(identifierType, j);
    //   // }

    //   // msg = msg + fileName + ",";
    //   // msg = msg + functionName + ",";
    //   // msg = msg + identifierName + ",";

    //   // std::cout << msg << "\n";

    //   // std::cerr << "key: " << e.first << "\n";

    //   // std::cerr << "\n\nIDN{" << e.second.identifierName << "}";
    //   // std::cerr << "IDT{" << e.second.identifierType << "}";

    //   // std::cerr << e.second.def.size() << "--";

    //         //       std::string identifierName;
    //         // std::string identifierType;

    //         // std::set<unsigned int> def;
    //         // std::set<unsigned int> use;
    // }
  }

private:
  //SrcSlicePolicy::SliceProfileSet srcData;

  // SrcSlicePolicy::SliceProfileSet SSPolicyData;

  FunctionSliceProfilePolicy::FunctionSliceProfileMap FSPpolicyData;

  std::vector<FunctionSliceProfilePolicy::FunctionSliceProfile> FSprofiles;


};

int main(int argc, char** argv){

  // std::ifstream codestream(argv[1]);
  // std::string srcmlstr;

  // codestream.seekg(0, std::ios::end);   
  // srcmlstr.reserve(codestream.tellg());
  // codestream.seekg(0, std::ios::beg);
  
  // srcmlstr.assign((std::istreambuf_iterator<char>(codestream)), std::istreambuf_iterator<char>());
  // std::string codestr = "int main() {\n int a = 0;\nint b = a + 1;\n int c = a + b + 1;\n }\n";
  // std::string srcmlstr = StringToSrcML(codestr);
  std::string srcmlstr = filetostring(argv[1]);
  std::cerr << srcmlstr << std::endl;

  SrcSlice srcslicedata;
  srcSAXController control(srcmlstr);
  srcSAXEventDispatch::srcSAXEventDispatcher<FunctionSliceProfilePolicy> handler {&srcslicedata};
  control.parse(&handler); //Start parsing
  srcslicedata.display();


  // for(auto i : srcslicedata.data.varNameProf) {
  //   std::cout << i.second.identifierName << "\n";
  //   std::cout << "def loc: " << i.second.def.size() << "\n";
  //   std::cout << "use loc: " << i.second.use.size() << "\n";
  //   std::cout << "test def: " << srcslicedata.functionSliceData->dataset[i.second.identifierName].def.size() << "\n";
  //   std::cout << "test use: " << srcslicedata.functionSliceData->dataset[i.second.identifierName].use.size() << "\n";
  // }

  // for(auto i : srcslicedata.functionSliceData->dataset) {

  //   std::cout << " . " << "\n";
  // }
  // std::cout << srcslicedata.data.varNameProf.begin()->second.linenumber << "\n";
  // srcslicedata.Test();
}