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
bool TestPrimitiveTypes(){
    std::string srcmlStr = StringToSrcML(FlatSlicePrograms::FlatSliceOne());
    try{
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*Test i's slice*/
        {
            std::cerr<< "TESTING i's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main",10));
            auto iSlice = sslice.Find("i");

            const std::set<unsigned int> defanswer = {11};
            const std::set<unsigned int> useanswer = {1,2,5,7,12,13,15,16,17};
            const std::unordered_set<std::string> dvarsanswer = {"sum"};
            std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 2));
            cfuncanswer.insert(std::make_pair("fun", 1));

            assert(iSlice.second.def == defanswer);
            assert(iSlice.second.use == useanswer);
            assert(iSlice.second.cfunctions == cfuncanswer);
            assert(iSlice.second.aliases.empty());
            assert(iSlice.second.dvars == dvarsanswer);
            std::cerr<< "COMPLETE"<<std::endl;
        }
        /*test sum's slice*/
        {
            std::cerr<< "TESTING sum's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main",10));
            auto sumSlice = sslice.Find("sum");
            
            std::set<unsigned int> defanswer = {10,17};
            std::set<unsigned int> useanswer = {1,2,5,6,13,15};
            std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> cfuncanswer;
            cfuncanswer.insert(std::make_pair("fun", 1));
            cfuncanswer.insert(std::make_pair("foo", 1));
    
            assert(sumSlice.second.def == defanswer);
            assert(sumSlice.second.use == useanswer);
            assert(sumSlice.second.cfunctions == cfuncanswer);
            assert(sumSlice.second.aliases.empty());
            assert(sumSlice.second.dvars.empty());
            std::cerr<< "COMPLETE"<<std::endl;  
        }
        {
            std::cerr<< "TESTING z's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "fun",1));
            auto zSlice = sslice.Find("z");
            
            std::set<unsigned int> defanswer = {1};
            std::set<unsigned int> useanswer = {2};

            assert(zSlice.second.def == defanswer);
            assert(zSlice.second.use == useanswer);
            assert(zSlice.second.cfunctions.empty());
            assert(zSlice.second.aliases.empty());
            assert(zSlice.second.dvars.empty());
            std::cerr<< "COMPLETE"<<std::endl;   
        }
        {
            std::cerr<< "TESTING y's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo",4));
            auto ySlice = sslice.Find("y");
            
            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {7};
            std::unordered_set<std::string> aliasanswer = {"i"};
            assert(ySlice.second.def == defanswer);
            assert(ySlice.second.use == useanswer);
            assert(ySlice.second.aliases == aliasanswer);
            assert(ySlice.second.cfunctions.empty());
            assert(ySlice.second.dvars.empty());
            std::cerr<< "COMPLETE"<<std::endl;
        }
        {
            std::cerr<< "TESTING x's SLICE"<<std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo",4));
            auto xSlice = sslice.Find("x");
            
            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {1,2,6};
            
            std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> cfuncanswer;
            cfuncanswer.insert(std::make_pair("fun", 1));

            std::unordered_set<std::string> aliasanswer = {"sum"};

            assert(xSlice.second.def == defanswer);
            assert(xSlice.second.use == useanswer);
            assert(xSlice.second.cfunctions == cfuncanswer);
            assert(xSlice.second.aliases == aliasanswer);
            assert(xSlice.second.dvars.empty());   
        }
    }catch(SAXError e){
        std::cerr<<"ERROR: "<<e.message;
    }
    return true;
}
int main(int argc, char** argv){
    TestPrimitiveTypes();
    //srcTypeNS::srcType typeDict;
    //typeDict.ReadArchiveFile(argv[1]);
    //typeDict.SerializeMap(SerializeToCppUMap);
    //std::cerr<<typeDict.size();
}