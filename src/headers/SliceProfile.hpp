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
typedef std::pair<unsigned int, unsigned int> HashedNameLineNumberPair;
typedef std::pair<std::string, std::string> TypeNamePair;

struct NameLineNumberPairHash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first);
  }
};
struct FunctionArgtypeArgnumHash {
public:
  template <typename T>
  std::size_t operator()(const T x) const
  {
    return std::hash<unsigned int>()(x.GetFunctionUID());
  }
};
struct FunctionData{
    FunctionData(){
        functionNumber = 0;
        functionLineNumber = 0;
    }
    void clear(){
        returnType.clear();
        functionName.clear();
    }
    bool operator==(const FunctionData& d) const{
        return d.functionName == functionName; //need to define proper op== for the hash
    }
    unsigned int GetFunctionUID() const{
        return functionNumber + params.size()+ functionHash;
    }
    std::string returnType;
    std::string functionName;

    std::vector<unsigned int> params; //size of vector is # of arguments. String is type of argument.
    
    unsigned int functionNumber; //obtained from adding hashes of types of arguments TODO: CHANGE NAME
    unsigned int functionHash; //obtained from hashing name of function TODO: CHANGE NAME
    unsigned int functionLineNumber;
};



struct ClassProfile{
    std::string className;
    std::unordered_set<std::string> memberVariables;
    std::unordered_set<FunctionData, FunctionArgtypeArgnumHash> memberFunctions; //need to handle overloads. Can't be string.
};

class SliceProfile{
	public:
		SliceProfile() = default;
		SliceProfile(unsigned int idx, unsigned int fle, unsigned int fcn, unsigned int sline, std::string name, bool alias = 0, bool global = 0):
        index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name),isGlobal(global) {
			slines.insert(sline);
            dereferenced = false;
		}

        //TODO: Deprecate these; we stopped hasing functions/files
		unsigned int index;
		unsigned int file;
		unsigned int function;
		
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
		std::unordered_set<std::string> dvars;//maybe hash
		std::unordered_set<std::string> aliases;//maybe hash
};
#endif