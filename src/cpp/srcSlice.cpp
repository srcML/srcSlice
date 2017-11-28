#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <unordered_map>
#include <unordered_set>
#include "../headers/SrcSlicePolicy.hpp"
#include <SrcSlicePolicy.hpp>
#include <FunctionSliceProfilePolicy.hpp>
#include <cassert>
#include <fstream>
#include <streambuf>

int main(int argc, char** argv){

  std::ifstream codestream(argv[1]);
  std::string srcmlstr;

  codestream.seekg(0, std::ios::end);   
  srcmlstr.reserve(codestream.tellg());
  codestream.seekg(0, std::ios::beg);
  
  srcmlstr.assign((std::istreambuf_iterator<char>(codestream)), std::istreambuf_iterator<char>());
  
  std::cerr << srcmlstr << std::endl;
  SrcSlicePolicy srcslicedata;    
  srcSAXController control(srcmlstr);
  srcSAXEventDispatch::srcSAXEventDispatcher<FunctionSliceProfilePolicy> handler {&srcslicedata};
  control.parse(&handler); //Start parsing
  for(auto i : srcslicedata.data.varNameProf) {
    std::cout << i.second.identifierName << "\n";
    std::cout << "def loc: " << i.second.def.size() << "\n";
    std::cout << "use loc: " << i.second.use.size() << "\n";
    std::cout << "test def: " << srcslicedata.functionSliceData->dataset[i.second.identifierName].def.size() << "\n";
    std::cout << "test use: " << srcslicedata.functionSliceData->dataset[i.second.identifierName].use.size() << "\n";
  }

  for(auto i : srcslicedata.functionSliceData->dataset) {

    std::cout << " . " << "\n";
  }
  // std::cout << srcslicedata.data.varNameProf.begin()->second.linenumber << "\n";
  // srcslicedata.Test();
}