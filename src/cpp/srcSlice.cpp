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
    // FuncSigPolicy.AddListener(this);
    // InitializeEventHandlers();

  };

  ~SrcSlice(){};

  void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override 
    {

      sliceprofile = *policy->Data<SrcSlicePolicy::SliceProfileSet>();
      sliceprofiles.push_back(sliceprofile);

      /*
      FSPpolicyData = *policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfileMap>();
      for(auto e : FSPpolicyData.dataset)
          FSprofiles.push_back(e.second);
      */
      // sigdata = *policy->Data<FunctionSignaturePolicy::SignatureData>();
      // signaturedata.push_back(sigdata);

    }

  void *DataInner() const override 
    {
        return (void*)0; 
    }

  void getSlice(std::string s) {
    for(auto p : sliceprofiles) {
      for(auto fp : p.varNameProf) { 
        if(fp.second.identifierName == s ) {
          auto fslice = fp.second;

          std::cerr << "var: " << fslice.identifierName << " defined on lines: ";
          for(auto ln : fslice.def) {
            std::cerr << ln << ", ";
          }
          std::cerr << "\n";

          std::cerr << "var: " << fslice.identifierName << " used on lines: ";
          for(auto ln : fslice.use) {
            std::cerr << ln << ", ";
          }
          std::cerr << "\n";
      
          std::cerr << "var: " << fslice.identifierName << " has some effect on: ";
          for(auto dvar : fslice.dvars) {
            std::cerr << dvar << ", ";
          }
          std::cerr << "\n\n";
        }

      }
    }
  }

  void display_csv() {

    for(auto p : sliceprofiles) {
      for(auto s : p.varNameProf) {

        std::string info = s.first;
        std::string delimiter = "|";

        size_t pos = 0;
        std::vector<std::string> tokens;

        while ((pos = info.find(delimiter)) != std::string::npos) {
          std::string token = info.substr(0, pos);
          tokens.push_back(token);
          info.erase(0, pos + delimiter.length());
        }
        // only variable name is left
        tokens.push_back(info);

        std::string filename = tokens[0];
        std::string funcname = tokens[1];
        std::string varname  = tokens[2];

        std::cerr << filename + ',' + funcname + ',' + varname + ',';

        std::cerr << "def{";
        int def_count = 0;
        for(auto def : s.second.def) {
          std::cerr << def;
          def_count++;
          if(def_count != s.second.def.size())
            std::cerr << ',';
        }
        std::cerr << "},";

        std::cerr << "use{";
        int use_count = 0;
        for(auto use : s.second.use) {
          std::cerr << use;
          use_count++;
          if(use_count != s.second.use.size())
            std::cerr << ',';
        }
        std::cerr << "},";

        std::cerr << "dvars{";
        int dvar_count = 0;
        for(auto dvar : s.second.dvars) {
          std::cerr << dvar;
          dvar_count++;
          if(dvar_count != s.second.dvars.size())
            std::cerr << ',';
        }
        std::cerr << "},";

        std::cerr << "\n";



        // std::cerr << "var: " << s.second.identifierName << " defined on lines: ";
        // for(auto ln : s.second.def) {
        //   std::cerr << ln << ", ";
        // }
        // std::cerr << "\n";

        // std::cerr << "var: " << s.second.identifierName << " used on lines: ";
        // for(auto ln : s.second.use) {
        //   std::cerr << ln << ", ";
        // }
        // std::cerr << "\n";
      
        // std::cerr << "var: " << s.second.identifierName << " has some effect on: ";
        // for(auto dvar : s.second.dvars) {
        //   std::cerr << dvar << ", ";
        // } 
        // std::cerr << "\n\n\n\n";
      }
    }

    /*


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
    */





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

  //FunctionSignaturePolicy::SignatureData sigdata;
  //std::vector<FunctionSignaturePolicy::SignatureData> signaturedata;

  SrcSlicePolicy::SliceProfileSet sliceprofile;
  std::vector<SrcSlicePolicy::SliceProfileSet> sliceprofiles;

  /*
  FunctionSliceProfilePolicy::FunctionSliceProfileMap FSPpolicyData;
  std::vector<FunctionSliceProfilePolicy::FunctionSliceProfile> FSprofiles;
  */
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
  //std::cerr << srcmlstr << std::endl;

  SrcSlice srcslicedata;
  srcSAXController control(srcmlstr);
  //srcSAXEventDispatch::srcSAXEventDispatcher<FunctionSliceProfilePolicy> handler {&srcslicedata};
  srcSAXEventDispatch::srcSAXEventDispatcher<SrcSlicePolicy> handler {&srcslicedata};
  control.parse(&handler); //Start parsing
  srcslicedata.display_csv();

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