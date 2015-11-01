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
        std::cerr<<srcmlStr<<std::endl;
        srcSlice sslice;
        sslice.ReadArchiveFile("srcslicetest.cpp.xml");
        sslice.SetContext("home/workbox/srcTools/build/bin/srcslicetest.cpp", "main",10);
        auto sumSlice = sslice.Find("sum");
        //std::cerr<<sumSlice.first;
        for(unsigned int i : sumSlice.second.def){
            std::cerr<<i<<std::endl;
        }
        for(unsigned int i : sumSlice.second.use){
            std::cerr<<i<<std::endl;
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