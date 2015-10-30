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



struct srcSlice{
    struct SliceDictionary{
        //context can be used to keep track of what function you're searching in. Makes searching faster because I assume you're using that function as the context
        struct Context{
            int ln;
            std::string fileName;
            std::string functionName;
            FunctionVarMap::iterator currentFunc;
            FileFunctionVarMap::iterator currentFile;
            Context():fileName(""), functionName(""), ln(-1){}
            bool IsSet() const {return (ln == -1 || functionName == "") ? false : true;}
            Context(std::string file, std::string func, unsigned int line, FileFunctionVarMap::iterator fileIt, FunctionVarMap::iterator funcIt)
            : fileName(file), functionName(func), ln(line), currentFile(fileIt), currentFunc(funcIt){}
        };
        Context currentContext;
        FileFunctionVarMap ffvMap;
    };
	/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
	SliceDictionary dictionary;

    VarMap globalMap;
    std::unordered_map<std::string, ClassProfile> classTable;
    std::unordered_map<unsigned int, std::string> typeTable;
    std::vector<std::pair<unsigned int, unsigned int>> controledges;
    
    srcSlice(){}
    srcSlice(const char*, const char*){}
    srcSlice(std::string, const char*){}
    srcSlice(FILE*, const char*){}
    srcSlice(int, const char*){}
    void ReadArchiveFile(std::string){}
    
    int size()const {return dictionary.ffvMap.size();}
    
    bool SetContext(std::string fle, std::string fn, int linenumber){
        FileFunctionVarMap::iterator fleIt = dictionary.ffvMap.find(fle);
        if(fleIt != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fnIt = fleIt->second.find(fn);
            if(fnIt != fleIt->second.end()){
                dictionary.currentContext.currentFile = fleIt;
                dictionary.currentContext.currentFunc = fnIt;
                dictionary.currentContext.ln = linenumber;
                dictionary.currentContext.functionName = fle;
                dictionary.currentContext.functionName = fn;
                return true;
            }            
        }
        return false;
    }

    bool SetContext(std::string fn, int linenumber){
        if(dictionary.currentContext.currentFile != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fnIt = dictionary.currentContext.currentFile->second.find(fn);
            if(fnIt != dictionary.currentContext.currentFile->second.end()){
                dictionary.currentContext.currentFunc = fnIt;
                dictionary.currentContext.ln = linenumber;
                dictionary.currentContext.functionName = fn;
                return true;
            }            
        }
        return false;
    }
    bool SetContext(int linenumber){ //it enough to just check function? Need to check file?
        if(dictionary.currentContext.currentFunc != dictionary.currentContext.currentFile->second.end()){
            dictionary.currentContext.ln = linenumber;
            return true;
        }
        return false;
    }
    //Definition of find that assumes the user didn't give a context (They should just give a context, though, tbh).
    std::pair<bool, SliceProfile> Find(std::string flename, std::string funcname, std::string varname, int lineNumber)const{
        FileFunctionVarMap::const_iterator ffvmIt = dictionary.ffvMap.find(flename);
        if(ffvmIt != dictionary.ffvMap.end()){
            FunctionVarMap::const_iterator fvmIt = ffvmIt->second.find(funcname);
            if(fvmIt != ffvmIt->second.end()){
                VarMap::const_iterator vtmIt = fvmIt->second.find(varname);
                if(vtmIt != fvmIt->second.end()){
                    return std::make_pair(true, vtmIt->second);
                }
            }            
        }

        return std::make_pair(false, SliceProfile());
    }
    //Definition of find that assumes the user didn't give a context (They should just give a context, though, tbh).
    std::pair<bool, SliceProfile> Find(std::string funcname, std::string varname, int lineNumber)const{
        FunctionVarMap::const_iterator fvmIt = dictionary.currentContext.currentFile->second.find(funcname);
        if(fvmIt != dictionary.currentContext.currentFile->second.end()){
            VarMap::const_iterator vtmIt = fvmIt->second.find(varname);
            if(vtmIt != fvmIt->second.end()){
                return std::make_pair(true, vtmIt->second);
            }
        }
        return std::make_pair(false, SliceProfile());
    }
    //Definition of find that uses the context (so it doesn't need to take a function name as context)
    std::pair<bool, SliceProfile> Find(std::string varname) const{
        if(!dictionary.currentContext.IsSet()){
            throw std::runtime_error("Context not set"); //for now, std exception
        }else{
            VarMap::const_iterator it = dictionary.currentContext.currentFunc->second.find(varname);
            if(it != dictionary.currentContext.currentFunc->second.end()){
                return std::make_pair(true, it->second);
            }
            return std::make_pair(false, SliceProfile());
        }
    }
    bool Insert(std::string flename, std::string funcname, const SliceProfile& np){
        FileFunctionVarMap::iterator ffvmIt = dictionary.ffvMap.find(flename);
        if(ffvmIt != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fvmIt = ffvmIt->second.find(funcname);
            if(fvmIt != ffvmIt->second.end()){
                VarMap::iterator vtmIt = fvmIt->second.find(np.variableName);
                if(vtmIt != fvmIt->second.end()){
                    vtmIt->second = np;
                    return true;
                }else{
                    fvmIt->second.insert(std::make_pair(np.variableName, np));
                    return true;
                }
            }
        }
        return false;
    }

    bool Insert(std::string funcname, const SliceProfile& np){
        if(dictionary.currentContext.currentFile != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fvmIt = dictionary.currentContext.currentFile->second.find(funcname);
            if(fvmIt != dictionary.currentContext.currentFile->second.end()){
                VarMap::iterator vtmIt = fvmIt->second.find(np.variableName);
                if(vtmIt != fvmIt->second.end()){
                    vtmIt->second = np;
                    return true;
                }else{
                    fvmIt->second.insert(std::make_pair(np.variableName, np));
                    return true;
                }
            }            
        }
        return false;
    }

    bool Insert(const SliceProfile& np){
        if(dictionary.currentContext.ln == -1){ //TODO: Make better
            throw std::runtime_error("Context not set"); //for now, std exception
        }else{
            auto it = dictionary.currentContext.currentFunc->second.find(np.variableName);
            if(it != dictionary.currentContext.currentFunc->second.end()){
                it->second = np;
                return true;
            }else{
                dictionary.currentContext.currentFunc->second.insert(std::make_pair(np.variableName, np));
                return true;
            }
        }
        return false;
    }

};
#endif