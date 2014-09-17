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
void TestSlice(const FileFunctionVarMap& mp){
	for(FileFunctionVarMap::const_iterator ffvmIt = mp.begin(); ffvmIt != mp.end(); ++ffvmIt){
		for(FunctionVarMap::const_iterator fvmIt = ffvmIt->second.begin(); fvmIt != ffvmIt->second.end(); ++fvmIt){
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
					std::cerr<<cfunc.first<<" "<<cfunc.second<<",";
				}
				std::cerr<<"}"<<std::endl;
				std::cerr<<"-------------------------"<<std::endl;
			}
		}
	}
}

void srcSliceToCsv(const SystemDictionary& sd){
	for(FileFunctionVarMap::const_iterator ffvmIt = sd.dictionary.begin(); ffvmIt != sd.dictionary.end(); ++ffvmIt){
		auto fileNameIt = sd.fileTable.find(ffvmIt->first);
		if(fileNameIt != sd.fileTable.end())
		for(FunctionVarMap::const_iterator fvmIt = ffvmIt->second.begin(); fvmIt != ffvmIt->second.end(); ++fvmIt){
			auto functionNameIt = sd.functionTable.find(fvmIt->first);
			if(functionNameIt!=sd.functionTable.end())
			for(VarMap::const_iterator vmIt = fvmIt->second.begin(); vmIt != fvmIt->second.end(); ++vmIt){
				std::cerr<<fileNameIt->second<<","<<functionNameIt->second.functionName;
				std::cerr<<",sl{";
				for(unsigned int sl : vmIt->second.slines){
					std::cerr<<sl<<",";
				}
				std::cerr<<"},";
				std::cerr<<"dv{";
				for(std::string dv : vmIt->second.dvars){
					std::cerr<<dv<<",";
				}
				std::cerr<<"},";
				std::cerr<<"al{";
				for(std::string al : vmIt->second.aliases){
					std::cerr<<al<<",";
				}
				std::cerr<<"},";
				std::cerr<<"cfunc{";
				for(auto cfunc : vmIt->second.cfunctions){
					std::cerr<<cfunc.first<<" "<<cfunc.second<<",";
				}
				std::cerr<<"}";
				std::cerr<<std::endl;
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
int main(int argc, char * argv[]) {

  if(argc < 2) {

    std::cerr << "Useage: element_count input_file.xml\n";
    exit(1);

  }

  srcSAXController control(argv[1]);
  srcSliceHandler handler;
  control.parse(&handler);
  TestSlice(handler.sysDict.dictionary);
  TestSlice2(handler.sysDict.globalMap);
  //srcSliceToCsv(handler.sysDict);
  return 0;
}
