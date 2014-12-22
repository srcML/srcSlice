/**
 * @file element_count.hpp
 *
 * @copyright Copyright (C) 2013-2014  SDML (www.srcML.org)
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <srcSliceHandler.hpp>
#include <srcSAXController.hpp>
#include <time.h> 
#include <map>
#include <iostream>

void TestSlice2(const VarMap& mp){
	for(VarMap::const_iterator vmIt = mp.begin(); vmIt != mp.end(); ++vmIt){
		std::cerr<<"-------------------------"<<std::endl;
		std::cerr<<"Variable: "<<vmIt->first<<std::endl;
		std::cerr<<"Slines: {";
		for(unsigned int sl : vmIt->second.slines){
			std::cerr<<sl<<",";
		}
		std::cerr<<"}"<<std::endl;
		std::cerr<<"dvars: {";
		for(std::string dv : vmIt->second.dvars){
			std::cerr<<dv<<",";
		}
		std::cerr<<"}"<<std::endl;
		std::cerr<<"aliases: {";
		for(std::string al : vmIt->second.aliases){
			std::cerr<<al<<",";
		}
		std::cerr<<"}"<<std::endl;
		std::cerr<<"cfuntions: {";
		for(auto cfunc : vmIt->second.cfunctions){
			std::cerr<<cfunc.first<<" "<<cfunc.second<<",";
		}
		std::cerr<<"}"<<std::endl;
		std::cerr<<"-------------------------"<<std::endl;
	}
}
void TestSlice(const FileFunctionVarMap& mp, srcSliceHandler handler){
	for(FileFunctionVarMap::const_iterator ffvmIt = mp.begin(); ffvmIt != mp.end(); ++ffvmIt){
		for(FunctionVarMap::const_iterator fvmIt = ffvmIt->second.begin(); fvmIt != ffvmIt->second.end(); ++fvmIt){
			//std::cerr<<fvmIt->first<<std::endl;
			std::cerr<<handler.sysDict.functionTable.find(fvmIt->first)->second<<std::endl; 
			for(VarMap::const_iterator vmIt = fvmIt->second.begin(); vmIt != fvmIt->second.end(); ++vmIt){
				std::cerr<<"-------------------------"<<std::endl;
				std::cerr<<"Variable: "<<vmIt->first<<std::endl;
				std::cerr<<"Slines: {";
				for(unsigned int sl : vmIt->second.slines){
					std::cerr<<sl<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"dvars: {";
				for(std::string dv : vmIt->second.dvars){
					std::cerr<<dv<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"aliases: {";
				for(std::string al : vmIt->second.aliases){
					std::cerr<<al<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"cfuntions: {";
				for(auto cfunc : vmIt->second.cfunctions){
					auto bla = handler.sysDict.functionTable.find(cfunc.first);
					if(bla != handler.sysDict.functionTable.end())
						std::cerr<<bla->second<<" "<<cfunc.second<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"def: {";
				for(auto defv : vmIt->second.def){
					std::cerr<<defv<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"use: {";
				for(auto usev : vmIt->second.use){
					std::cerr<<usev<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"-------------------------"<<std::endl;
			}
		}
	}
}

void srcSliceToCsv(const srcSliceHandler& handler){
	for(FileFunctionVarMap::const_iterator ffvmIt = handler.sysDict.dictionary.begin(); ffvmIt != handler.sysDict.dictionary.end(); ++ffvmIt){
		//auto fileNameIt = handler.sysDict.fileTable.find(ffvmIt->first);
		//if(fileNameIt != handler.sysDict.fileTable.end())
		for(FunctionVarMap::const_iterator fvmIt = ffvmIt->second.begin(); fvmIt != ffvmIt->second.end(); ++fvmIt){
			//auto functionNameIt = handler.sysDict.functionTable.find();
			for(VarMap::const_iterator vmIt = fvmIt->second.begin(); vmIt != fvmIt->second.end(); ++vmIt){
				std::cout<<ffvmIt->first<<","<<handler.sysDict.functionTable.find(fvmIt->first)->second<<","<<vmIt->first;
				std::cout<<",sl{";
				for(unsigned int sl : vmIt->second.slines){
					std::cout<<sl<<",";
				}
				std::cout<<"},";
				std::cout<<"dv{";
				for(std::string dv : vmIt->second.dvars){
					std::cout<<dv<<",";
				}
				std::cout<<"},";
				std::cout<<"al{";
				for(std::string al : vmIt->second.aliases){
					std::cout<<al<<",";
				}
				std::cout<<"},";
				std::cout<<"cfunc{";
				for(auto cfunc : vmIt->second.cfunctions){
					auto bla = handler.sysDict.functionTable.find(cfunc.first);
					if(bla != handler.sysDict.functionTable.end())
						std::cout<<bla->second<<" "<<cfunc.second<<",";
				}
				std::cout<<"}";
				std::cout<<std::endl;
			}
		}
	}	
}
/**
 * main
 * @param argc number of arguments
 * @param argv the provided arguments (array of C strings)
 * 
 * Invoke srcSAX handler to count element occurences and print out the resulting element counts.
 */
/*
  Type Resolution tool
  Def Use Tool as separate thing (same as type res?)
  methods
  statement #
  Consider output to srcML
  */
int main(int argc, char * argv[]) {

  if(argc < 2) {

    std::cerr << "Useage: element_count input_file.xml\n";
    exit(1);

  }
  //clock_t t;
  //t = clock();
  srcSAXController control(argv[1]);
  srcSliceHandler handler;
  control.parse(&handler);
  //t = clock() - t;
  //std::cerr<<"Time is: "<<((float)t)/CLOCKS_PER_SEC<<std::endl;
  //std::string filename = handler.sysDict.dictionary.find("stack.cpp.xml");
  //handler.ComputeInterprocedural("SlicerTestSample.cpp");
  //TestSlice(handler.sysDict.dictionary, handler);
  
  //TestSlice2(handler.sysDict.globalMap);
  srcSliceToCsv(handler);
  return 0;
}