/**
 * @file Utility.hpp
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
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <set>

#include <SliceProfile.hpp>

static std::set<std::string> errorset;
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
    VarMap globalMap;
    std::unordered_map<std::string, ClassProfile> classTable;
    std::unordered_map<std::string, FunctionData> fileFunctionTable;
    std::vector<std::pair<unsigned int, unsigned int>> controledges;
    Context currentContext;
    FileFunctionVarMap ffvMap;
};
template <typename T> 
inline void SetUnion(std::unordered_set<T>& set1, std::unordered_set<T> set2){
    for(typename std::unordered_set<T>::iterator itr = set2.begin(); itr != set2.end(); ++itr){
        set1.insert(*itr);
    }
}

template <typename T> 
inline void SetUnion(std::set<T>& set1, std::set<T> set2){
    for(typename std::set<T>::iterator itr = set2.begin(); itr != set2.end(); ++itr){
        set1.insert(*itr);
    }
}

template <typename T, typename U>
inline void SetUnion(std::unordered_set<T, U>& set1, std::unordered_set<T, U>& set2){
    for(typename std::unordered_set<T, U>::iterator itr = set2.begin(); itr != set2.end(); ++itr){
        set1.insert(*itr);
    }
}

inline std::vector<std::string> SplitLhsRhs(const std::string& str){
    std::vector<std::string> expr;
    expr.push_back(std::string());
    for(int i = 0; i<str.size(); ++i){
        if(str[i] == '='){
            expr.push_back(str.substr(i+1, str.size()-1));
            break;
        }else{
            expr[0]+=str[i];
        }
    }
    return expr;
}

/* Split function for splitting strings by tokens. Works on sets of tokens or just one token*/
inline std::vector<std::string> SplitOnTok(const std::string& str, const char* tok){
    std::size_t tokPos = str.find_first_of(tok);
    std::vector<std::string> result;
    std::size_t nextPos = 0, curPos = 0;
    while(curPos != std::string::npos){
        result.push_back(str.substr(nextPos, tokPos - nextPos));
        nextPos = tokPos+1;
        curPos = tokPos;
        tokPos = str.find_first_of(tok, nextPos);
    }
    return result;
}
#endif