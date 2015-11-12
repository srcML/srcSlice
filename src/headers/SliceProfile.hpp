/**
 * @file SliceProfile.hpp
 *
 * @copyright Copyright (C) 2013-2014 SDML (www.srcML.org)
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

#ifndef SLICEPROFILE_HPP
#define SLICEPROFILE_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
class SliceProfile;
typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<std::string, VarMap> FunctionVarMap;
typedef std::unordered_map<std::string, FunctionVarMap> FileFunctionVarMap;
typedef std::pair<std::string, unsigned int> NameLineNumberPair;

struct NameLineNumberPairHash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first);
  }
};
struct FunctionData{
    FunctionData(){
        functionLineNumber = 0;
    }
    void clear(){
        returnType.clear();
        functionName.clear();
    }
    std::string returnType;
    std::string functionName;
    std::string fileName;
    
    std::vector<std::string> params; //size of vector is # of arguments. String is type of argument.

    unsigned int functionLineNumber;
};

struct ClassProfile{
    std::string className;
    std::unordered_set<std::string> memberVariables;
    //std::unordered_set<FunctionData, FunctionArgtypeArgnumHash> memberFunctions; //need to handle overloads. Can't be string.
};

class SliceProfile{
	public:
		SliceProfile():index(0),visited(false),potentialAlias(false),dereferenced(false),isGlobal(false){}
		SliceProfile(unsigned int idx, std::string fle, std::string fcn, unsigned int sline, std::string name, bool alias = 0, bool global = 0):
        index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name),isGlobal(global) {
            dereferenced = false;
            visited = false;
		}

		unsigned int index;
		std::string file;
		std::string function;
		
        std::unordered_set<std::string>::iterator lastInsertedAlias;

		bool potentialAlias;
        bool dereferenced;

        bool isGlobal;
        bool visited;

		std::string variableName;
		std::string variableType;
		std::unordered_set<std::string> memberVariables;
		std::unordered_set<unsigned int> slines; //Deprecated
        
        std::set<unsigned int> def;
        std::set<unsigned int> use;
		
        std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> cfunctions;
		std::unordered_set<std::string> dvars;
		std::unordered_set<std::string> aliases;
};
#endif