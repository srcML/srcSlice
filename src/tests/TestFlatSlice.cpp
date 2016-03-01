/**
 * @file srcSlice.cpp
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
#include <srcml.h>
#include <cassert>
#include "TestFlatSlice.hpp"
/// <summary>
/// Utility function that trims from the right of a string. For now it's just solving a weird issue with srcML
/// and garbage text ending up at the end of the cstring it returns.
/// </summary>
inline char* TrimFromEnd(char *s, size_t len){
    for (int i = len - 1; i > 0; --i){
        if (s[i] != '>'){
            s[i] = 0;
        }else{
            return s;
        }
    }
    return nullptr;
}
std::string StringToSrcML(std::string str){
    struct srcml_archive* archive;
    struct srcml_unit* unit;
    size_t size = 0;

    char *ch = new char[str.size()];

    archive = srcml_archive_create();
    srcml_archive_enable_option(archive, SRCML_OPTION_POSITION);
    srcml_archive_write_open_memory(archive, &ch, &size);

    unit = srcml_unit_create(archive);
    srcml_unit_set_language(unit, SRCML_LANGUAGE_CXX);
    srcml_unit_set_filename(unit, "testsrcSlice.cpp");

    srcml_unit_parse_memory(unit, str.c_str(), str.size());
    srcml_write_unit(archive, unit);
    
    srcml_unit_free(unit);
    srcml_archive_close(archive);
    srcml_archive_free(archive);
    TrimFromEnd(ch, size);
    return std::string(ch);
}
typedef std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> cfuncset;

void OutputCompare(const cfuncset& lhsSet, const cfuncset& rhsSet){
    std::cerr<<"cfuncs: {";
    for(auto i : lhsSet){
        std::cerr<<"{"<<i.first<<","<<i.second<<"},";
    }
    std::cerr<<"} == {";
    for(auto i : rhsSet){
        std::cerr<<"{"<<i.first<<","<<i.second<<"},";
    }
    std::cerr<<"}"<<std::endl;
}

template <typename T>
void OutputCompare(const T& lhsSet, const T& rhsSet){
    std::cerr<<"{";
    for(auto i : lhsSet){
        std::cerr<<i<<",";
    }
    std::cerr<<"} == {";
    for(auto i : rhsSet){
        std::cerr<<i<<",";
    }
    std::cerr<<"}"<<std::endl;
}
bool TestPrimitiveTypes(){
    std::string srcmlStr = StringToSrcML(FlatSlicePrograms::FlatSliceOne());
    try{
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*Test i's slice*/
        {
            std::cerr<<std::endl<<"================= TESTING i's SLICE ================="<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main",10));
            auto iSlice = sslice.Find("i");

            const std::set<unsigned int> defanswer = {11};
            const std::set<unsigned int> useanswer = {1,2,3,5,7,12,13,15,16,17};
            const std::unordered_set<std::string> dvarsanswer = {"sum"};
            cfuncset cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 2));
            cfuncanswer.insert(std::make_pair("fun", 1));

            OutputCompare(iSlice.second.def, defanswer);
            OutputCompare(iSlice.second.use, useanswer);
            OutputCompare(iSlice.second.dvars, dvarsanswer);
            OutputCompare(iSlice.second.cfunctions, cfuncanswer);

            assert(iSlice.second.use == useanswer);
            assert(iSlice.second.cfunctions == cfuncanswer);
            assert(iSlice.second.aliases.empty());
            assert(iSlice.second.dvars == dvarsanswer);
            std::cerr<<"================= COMPLETE ================="<<std::endl;
        }
        /*test sum's slice*/
        {
            std::cerr<<std::endl<<"================= TESTING sum's SLICE ================="<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main",10));
            auto sumSlice = sslice.Find("sum");
            
            std::set<unsigned int> defanswer = {10,17};
            std::set<unsigned int> useanswer = {1,2,3,5,6,13,15};
            cfuncset cfuncanswer;
            cfuncanswer.insert(std::make_pair("fun", 1));
            cfuncanswer.insert(std::make_pair("foo", 1));

            OutputCompare(sumSlice.second.def, defanswer);
            OutputCompare(sumSlice.second.use, useanswer);
            OutputCompare(sumSlice.second.cfunctions, cfuncanswer);

            assert(sumSlice.second.def == defanswer);
            assert(sumSlice.second.use == useanswer);
            assert(sumSlice.second.cfunctions == cfuncanswer);
            assert(sumSlice.second.aliases.empty());
            assert(sumSlice.second.dvars.empty());
            std::cerr<<"================= COMPLETE ================="<<std::endl;  
        }
        {
            std::cerr<<std::endl<<"================= TESTING z's SLICE ================="<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "fun",1));
            auto zSlice = sslice.Find("z");
            
            std::set<unsigned int> defanswer = {1};
            std::set<unsigned int> useanswer = {2,3};

            OutputCompare(zSlice.second.def, defanswer);
            OutputCompare(zSlice.second.use, useanswer);

            assert(zSlice.second.def == defanswer);
            assert(zSlice.second.use == useanswer);
            assert(zSlice.second.cfunctions.empty());
            assert(zSlice.second.aliases.empty());
            assert(zSlice.second.dvars.empty());
            std::cerr<<"================= COMPLETE ================="<<std::endl;   
        }
        {
            std::cerr<<std::endl<<"================= TESTING y's SLICE ================="<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo",4));
            auto ySlice = sslice.Find("y");
            
            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {7};
            std::unordered_set<std::string> aliasanswer = {"i"};
            
            OutputCompare(ySlice.second.def, defanswer);
            OutputCompare(ySlice.second.use, useanswer);

            assert(ySlice.second.def == defanswer);
            assert(ySlice.second.use == useanswer);
            assert(ySlice.second.aliases == aliasanswer);
            assert(ySlice.second.cfunctions.empty());
            assert(ySlice.second.dvars.empty());
            std::cerr<<"================= COMPLETE ================="<<std::endl;
        }
        {
            std::cerr<<std::endl<<"================= TESTING x's SLICE ================="<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo",4));
            auto xSlice = sslice.Find("x");
            
            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {1,2,3,6};
            
            cfuncset cfuncanswer;
            cfuncanswer.insert(std::make_pair("fun", 1));

            std::unordered_set<std::string> aliasanswer = {"sum"};

            OutputCompare(xSlice.second.def, defanswer);
            OutputCompare(xSlice.second.use, useanswer);
            OutputCompare<std::unordered_set<std::string>>(xSlice.second.aliases, aliasanswer);
            OutputCompare(xSlice.second.cfunctions, cfuncanswer);
            assert(xSlice.second.def == defanswer);
            assert(xSlice.second.use == useanswer);
            assert(xSlice.second.cfunctions == cfuncanswer);
            assert(xSlice.second.aliases == aliasanswer);
            assert(xSlice.second.dvars.empty());
            std::cerr<<"================= COMPLETE ================="<<std::endl;
        }

    }catch(SAXError e){
        std::cerr<<"ERROR: "<<e.message;
    }
    return true;
}
bool TestDecl(){
    std::string srcmlStr = StringToSrcML(FlatSlicePrograms::DeclSlice());
    try{
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        std::cerr<<std::endl<<"================= TESTING x's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto xSlice = sslice.Find("x");                
            const std::unordered_set<std::string> dvarsanswer = {"y"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {4};

            OutputCompare(xSlice.second.def, defanswer);
            OutputCompare(xSlice.second.use, useanswer);
            OutputCompare(xSlice.second.dvars, dvarsanswer);

            assert(xSlice.second.def == defanswer);
            assert(xSlice.second.use == useanswer);
            assert(xSlice.second.cfunctions.empty());
            assert(xSlice.second.aliases.empty());
            assert(xSlice.second.dvars == dvarsanswer);
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING b's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto bSlice = sslice.Find("b");                
            const std::unordered_set<std::string> dvarsanswer = {"y"};
            std::set<unsigned int> defanswer = {3};
            std::set<unsigned int> useanswer = {4};
            
            OutputCompare(bSlice.second.def, defanswer);
            OutputCompare(bSlice.second.use, useanswer);
            OutputCompare(bSlice.second.dvars, dvarsanswer);

            assert(bSlice.second.def == defanswer);
            assert(bSlice.second.use == useanswer);
            assert(bSlice.second.cfunctions.empty());
            assert(bSlice.second.aliases.empty());
            assert(bSlice.second.dvars == dvarsanswer);
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING y's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto ySlice = sslice.Find("y");
            std::set<unsigned int> defanswer = {4};
            
            OutputCompare(ySlice.second.def, defanswer);

            assert(ySlice.second.def == defanswer);
            assert(ySlice.second.use.empty());
            assert(ySlice.second.cfunctions.empty());
            assert(ySlice.second.aliases.empty());
            assert(ySlice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING str1's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto str1Slice = sslice.Find("str1");
            std::set<unsigned int> defanswer = {5};

            OutputCompare(str1Slice.second.def, defanswer);

            assert(str1Slice.second.def == defanswer);
            assert(str1Slice.second.use.empty());
            assert(str1Slice.second.cfunctions.empty());
            assert(str1Slice.second.aliases.empty());
            assert(str1Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING str2's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto str2Slice = sslice.Find("str2");
            std::set<unsigned int> defanswer = {5};

            OutputCompare(str2Slice.second.def, defanswer);
            
            assert(str2Slice.second.def == defanswer);
            assert(str2Slice.second.use.empty());
            assert(str2Slice.second.cfunctions.empty());
            assert(str2Slice.second.aliases.empty());
            assert(str2Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING str3's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto str3Slice = sslice.Find("str3");
            std::set<unsigned int> defanswer = {5};
            
            OutputCompare(str3Slice.second.def, defanswer);

            assert(str3Slice.second.def == defanswer);
            assert(str3Slice.second.use.empty());
            assert(str3Slice.second.cfunctions.empty());
            assert(str3Slice.second.aliases.empty());
            assert(str3Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING str4's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto str4Slice = sslice.Find("str4");
            std::set<unsigned int> defanswer = {6};
            std::set<unsigned int> useanswer = {8};
            
            OutputCompare(str4Slice.second.def, defanswer);
            OutputCompare(str4Slice.second.use, useanswer);

            assert(str4Slice.second.def == defanswer);
            assert(str4Slice.second.use == useanswer);
            assert(str4Slice.second.cfunctions.empty());
            assert(str4Slice.second.aliases.empty());
            assert(str4Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING str5's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto str5Slice = sslice.Find("str5");
            std::set<unsigned int> defanswer = {8};
            std::unordered_set<std::string> aliasanswer = {"str4"};
            
            OutputCompare(str5Slice.second.def, defanswer);
            OutputCompare<std::unordered_set<std::string>>(str5Slice.second.aliases, aliasanswer);

            assert(str5Slice.second.def == defanswer);
            assert(str5Slice.second.use.empty());
            assert(str5Slice.second.cfunctions.empty());
            assert(str5Slice.second.aliases == aliasanswer);
            assert(str5Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING str6's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto str6Slice = sslice.Find("str6");
            std::set<unsigned int> defanswer = {6};
            
            OutputCompare(str6Slice.second.def, defanswer);

            assert(str6Slice.second.def == defanswer);
            assert(str6Slice.second.use.empty());
            assert(str6Slice.second.cfunctions.empty());
            assert(str6Slice.second.aliases.empty());
            assert(str6Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING mp1's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto mp1Slice = sslice.Find("mp1");
            std::set<unsigned int> defanswer = {7};
            
            OutputCompare(mp1Slice.second.def, defanswer);

            assert(mp1Slice.second.def == defanswer);
            assert(mp1Slice.second.use.empty());
            assert(mp1Slice.second.cfunctions.empty());
            assert(mp1Slice.second.aliases.empty());
            assert(mp1Slice.second.dvars.empty());
        }            
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        
    }catch(SAXError e){
        std::cerr<<"ERROR: "<<e.message;
    }
    return true;
}
bool TestExpr(){
    std::string srcmlStr = StringToSrcML(FlatSlicePrograms::ExprSlice());
    try{
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*test sum's slice*/
        
        std::cerr<<std::endl<<"================= TESTING var's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto varSlice = sslice.Find("var");     
            std::set<unsigned int> defanswer = {9};
            std::set<unsigned int> useanswer = {9,10};
            
            OutputCompare(varSlice.second.def, defanswer);
            OutputCompare(varSlice.second.use, useanswer);

            assert(varSlice.second.def == defanswer);
            assert(varSlice.second.use == useanswer);
            assert(varSlice.second.cfunctions.empty());
            assert(varSlice.second.aliases.empty());
            assert(varSlice.second.dvars.empty());
        }
        std::cerr<<"=================COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING temp2's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto temp2Slice = sslice.Find("temp2");                
            const std::unordered_set<std::string> dvarsanswer = {"var1"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {3,6,10,13};
            cfuncset cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 1));
            
            OutputCompare(temp2Slice.second.def, defanswer);
            OutputCompare(temp2Slice.second.use, useanswer);
            OutputCompare(temp2Slice.second.dvars, dvarsanswer);
            OutputCompare(temp2Slice.second.cfunctions, cfuncanswer);
            assert(temp2Slice.second.def == defanswer);
            assert(temp2Slice.second.use == useanswer);
            assert(temp2Slice.second.cfunctions == cfuncanswer);
            assert(temp2Slice.second.aliases.empty());
            assert(temp2Slice.second.dvars == dvarsanswer);
        }
        std::cerr<<"=================COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING temp's SLICE ================="<<std::endl;            
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto tempSlice = sslice.Find("temp");
            const std::unordered_set<std::string> dvarsanswer = {"var1"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {3,5,6,8,9,13};
            cfuncset cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 1));
            
            OutputCompare(tempSlice.second.def, defanswer);
            OutputCompare(tempSlice.second.use, useanswer);
            OutputCompare(tempSlice.second.dvars, dvarsanswer);
            OutputCompare(tempSlice.second.cfunctions, cfuncanswer);

            assert(tempSlice.second.def == defanswer);
            assert(tempSlice.second.use == useanswer);
            assert(tempSlice.second.cfunctions == cfuncanswer);
            assert(tempSlice.second.aliases.empty());
            assert(tempSlice.second.dvars == dvarsanswer);
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING var1's SLICE ================="<<std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main",1));
            auto var1Slice = sslice.Find("var1");
            std::set<unsigned int> defanswer = {2,3};
            std::set<unsigned int> useanswer = {5,6,8,10};
            cfuncset cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 2));
            
            OutputCompare(var1Slice.second.def, defanswer);
            OutputCompare(var1Slice.second.use, useanswer);
            OutputCompare(var1Slice.second.cfunctions, cfuncanswer);

            assert(var1Slice.second.def == defanswer);
            assert(var1Slice.second.use == useanswer);
            assert(var1Slice.second.cfunctions == cfuncanswer);
            assert(var1Slice.second.aliases.empty());
            assert(var1Slice.second.dvars.empty());
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        
    }catch(SAXError e){
        std::cerr<<"ERROR: "<<e.message;
    }
    return true;
}
bool TestDotAndMemberAccess(){
    std::string srcmlStr = StringToSrcML(FlatSlicePrograms::DotAndMemberAccess());
    try{
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*test sum's slice*/
        std::cerr<<std::endl<<"================= TESTING y's SLICE ================="<<std::endl;
        {
            std::cerr<< "TESTING y's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp","main",1));
            auto sumSlice = sslice.Find("y");

            std::set<unsigned int> defanswer = {7};
            std::set<unsigned int> useanswer = {8,9,10};
            
            assert(sumSlice.second.def == defanswer);
            assert(sumSlice.second.use == useanswer);
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
        std::cerr<<std::endl<<"================= TESTING x's SLICE ================="<<std::endl;
        {
            std::cerr<< "TESTING x's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp","main",10));
            auto sumSlice = sslice.Find("x");

            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {4,5,6};
            assert(sumSlice.second.def == defanswer);
            assert(sumSlice.second.use == useanswer);
        }
        std::cerr<<"================= COMPLETE ================="<<std::endl;
    }catch(SAXError e){
        std::cerr<<"ERROR: "<<e.message;
    }
}
int main(int argc, char** argv){
    TestPrimitiveTypes();
    TestDecl();
    TestExpr();
    TestDotAndMemberAccess();
    //srcTypeNS::srcType typeDict;
    //typeDict.ReadArchiveFile(argv[1]);
    //typeDict.SerializeMap(SerializeToCppUMap);
    //std::cerr<<typeDict.size();
}