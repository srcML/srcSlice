/**
 * @file srcSliceHandler.hpp
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

#ifndef INCLUDED_SRCSLICEHANDLER_HPP
#define INCLUDED_SRCSLICEHANDLER_HPP

#include <srcSAXHandler.hpp>
#include <SliceProfile.hpp>

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <bitset>
#include <list>

/**
 * srcSliceHandler
 *
 * Base class that provides hooks for SAX processing.
 */

class srcSliceHandler : public srcSAXHandler {

private :
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, argument_list, call, ctrlflow, endflow, name, function, argument, block, type, init, op, nonterminal, empty};
    typedef std::pair<ParserState, std::string> StateFunctionPair;
    struct SliceMetaData{
        SliceMetaData(unsigned int line, ParserState current, std::string dat): lineNumber(line), currentState(current), data(dat){};
        unsigned int lineNumber;
        ParserState currentState;
        std::string data;
    };
    struct ExprStmt{
        ExprStmt(){
            numNames = 0;
        }
        std::string lhs;
        std::string op;
        std::string rhs;
        int numNames; //So I know if I'm looking at lhs or rhs.
    };
    unsigned int fileNumber;
    unsigned int functionNumber;
    unsigned int functionLineNumber;
    
    std::string currentFunction;
    
    ExprStmt exprstmt;
    ExprStmt compoundName;

    std::list<std::vector<SliceMetaData>> SliceVarMetaDataStack;
    std::vector<unsigned short int> triggerField;

    SystemDictionary sysDict;
    std::unordered_map<unsigned int, SliceProfile> openSliceProfiles;


public :
    srcSliceHandler(){
        fileNumber = 0;
        functionNumber = 0;
        functionLineNumber = 0;
        currentFunction = "";
        triggerField = std::vector<unsigned short int>(19, 0);
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


    /*
    virtual void startDocument() {}
    virtual void endDocument() {}
    */

    /**
     * startRoot
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param nb_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param nb_attributes the number of attributes on the tag
     * @param nb_defaulted the number of defaulted attributes
     * @param attributes list of attribute name value pairs (localname/prefix/URI/value/end)
     *
     * SAX handler function for start of the root element.
     * Counts the root unit (if an archive, to avoid double count with startUnit).
     * Overide for desired behaviour.
     */
    virtual void startRoot(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {
        
        //push_element(localname, prefix);
    }
    /**
     * startUnit
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param nb_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param nb_attributes the number of attributes on the tag
     * @param nb_defaulted the number of defaulted attributes
     * @param attributes list of attribute name value pairs (localname/prefix/URI/value/end)
     *
     * SAX handler function for start of an unit.
     * Counts each unit tag (= filecount non-archive, = filecount + 1 if archive).
     * Overide for desired behaviour.
     */
    virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {
        
        //sysDict.fileTable.insert(std::make_pair(fileNumber, attributes[1].value));
        //std::cerr<<attributes[1].value<<std::endl;
        //Take filename attribute and hash it.
        //push_element(localname, prefix);
    }
    /**
     * startElementNs
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param nb_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param nb_attributes the number of attributes on the tag
     * @param nb_defaulted the number of defaulted attributes
     * @param attributes list of attribute name value pairs (localname/prefix/URI/value/end)
     *
     * SAX handler function for start of an element.
     * Overide for desired behaviour.
    */
    virtual void startElement(const char * localname, const char * prefix, const char * URI,
                                int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                                const struct srcsax_attribute * attributes) {
          
        unsigned int lineNum = 0;
        if(num_attributes){
            lineNum = strtoul(attributes[0].value, NULL, 0);
        }

        if(std::string(localname) == "decl_stmt"){
            ++triggerField[decl_stmt];
            SliceVarMetaDataStack.push_back(std::vector<SliceMetaData>({SliceMetaData(lineNum, decl_stmt, "")}));
        }else if(std::string(localname) == "function"){
            ++triggerField[function];
            SliceVarMetaDataStack.push_back(std::vector<SliceMetaData>({SliceMetaData(lineNum, function, "")}));   
        }else if (std::string(localname) == "expr_stmt"){
            ++triggerField[expr_stmt];
            SliceVarMetaDataStack.push_back(std::vector<SliceMetaData>({SliceMetaData(lineNum, expr_stmt, "")}));
        }else if (std::string(localname) == "parameter_list"){
            ++triggerField[parameter_list];
            SliceVarMetaDataStack.push_back(std::vector<SliceMetaData>({SliceMetaData(lineNum, parameter_list, "")}));   
        }else if (std::string(localname) == "argument_list"){
            ++triggerField[argument_list];
            SliceVarMetaDataStack.push_back(std::vector<SliceMetaData>({SliceMetaData(lineNum, argument_list, "")}));   
        }else if (std::string(localname) == "call"){
            ++triggerField[call];
            SliceVarMetaDataStack.push_back(std::vector<SliceMetaData>({SliceMetaData(lineNum, call, "")}));
        }else if (std::string(localname) == "param"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[param];
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, param, ""));
            }
        }else if(std::string(localname) == "operator"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[op];
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, op, ""));
            }
        }else if (std::string(localname) == "block"){ //So I can discriminate against things in or outside of blocks
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[block];
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, block, ""));
            }
        }else if (std::string(localname) == "init"){ //So I can discriminate against things in or outside of blocks
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[init];
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, init, ""));
            }
        }else if (std::string(localname) == "argument"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[argument];
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, argument, ""));
            }
        }else if(std::string(localname) == "decl"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[decl]; 
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, decl, ""));
            }
        }else if(std::string(localname) == "type"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[type]; 
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, type, ""));
            }
        }else if (std::string(localname) == "expr"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[expr];
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, expr, ""));
            }
        }else if (std::string(localname) == "name"){
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[name];
                if(triggerField[function] && triggerField[type] && !triggerField[block]){
                    if(num_attributes){
                        auto dat = SliceVarMetaDataStack.back().back();
                        functionLineNumber = strtoul(attributes[0].value, NULL, 0);
                    }
                }
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, name, ""));
            }
        }else{
            if(!SliceVarMetaDataStack.empty()){
                ++triggerField[nonterminal]; 
                SliceVarMetaDataStack.back().push_back(SliceMetaData(lineNum, nonterminal, ""));
            }
        }
        //functionLineNumber = strtoul(attributes[0].value, NULL, 0);
        //push_element(localname, prefix, lineNum);
    }

    /**
     * charactersUnit
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling within a unit.
     * Overide for desired behaviour.
     */
    virtual void charactersUnit(const char * ch, int len) {
        std::string content = "";
        content.append(ch, len);
        
        if(!SliceVarMetaDataStack.empty()){
            //auto currentStack = SliceVarMetaDataStack.back();
            if(SliceVarMetaDataStack.back().back().currentState == name){
                SliceVarMetaDataStack.back().back().data = content;
                //std::cerr<<content<<std::endl;
            }else if(SliceVarMetaDataStack.back().back().currentState == op && triggerField[name] == 0){
                SliceVarMetaDataStack.back().back().data = content;
            }
        }
    }

    void push_element(const char * localname, const char * prefix, unsigned int line = 0) {

        std::string full_name = "";

        if(prefix) {

            full_name = prefix;
            full_name += ":";

        }

        full_name += localname;
        

    }
    

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
        if(std::string(localname) == "decl_stmt"){
            --triggerField[decl_stmt];
            SliceVarMetaDataStack.pop_back();
        }else if(std::string(localname) == "function"){
            --triggerField[function];
            SliceVarMetaDataStack.pop_back();
        }else if (std::string(localname) == "expr_stmt"){
            --triggerField[expr_stmt];
            exprstmt.numNames = 0;
            SliceVarMetaDataStack.pop_back();
        }else if (std::string(localname) == "parameter_list"){
            --triggerField[parameter_list];
            SliceVarMetaDataStack.pop_back();
        }else if (std::string(localname) == "argument_list"){
            --triggerField[argument_list];
            SliceVarMetaDataStack.pop_back();
        }else if (std::string(localname) == "call"){
            //currentFunction = "";
            --triggerField[call];
            SliceVarMetaDataStack.pop_back();
        }else if (std::string(localname) == "param"){
            --triggerField[param];
        }else if (std::string(localname) == "argument"){
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[argument];
            }
        }else if (std::string(localname) == "block"){
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[block];
            }
        }else if(std::string(localname) == "decl"){
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[decl];                 
            }
        }else if(std::string(localname) == "init"){
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[init];                 
            }
        }else if (std::string(localname) == "expr"){
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[expr]; 
            }
        }else if (std::string(localname) == "type"){
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[type]; 
            }
        }else if (std::string(localname) == "operator"){
            if(!SliceVarMetaDataStack.empty()){
                //Get Expression Statement op
                if(triggerField[expr_stmt] && triggerField[expr] && triggerField[op]){
                    auto dat = SliceVarMetaDataStack.back().back();
                    //std::cerr<<"op: "<<dat.data<<std::endl;
                    exprstmt.op = dat.data;
                    //std::cerr<<SliceVarMetaDataStack.back().back().data<<std::endl;
                }
                --triggerField[op]; 
            }
        }
        else if (std::string(localname) == "name"){
            if(!SliceVarMetaDataStack.empty()){
                //Get Function names
                if(triggerField[function] && 
                   !(triggerField[argument_list] || triggerField[block] || triggerField[type] || triggerField[parameter_list]) 
                   && triggerField[name] == 1){
                      //PROBLEM: I MIGHT NOT RUN INTO THE FUNCTION BEFORE IT GETS CALLED. Solve.
                      auto dat = SliceVarMetaDataStack.back().back();
                      ++functionNumber;
                      sysDict.reverseFunctionTable.insert(std::make_pair(dat.data, functionNumber));
                      sysDict.functionTable.insert(std::make_pair(functionNumber,dat.data));

                      //std::cerr<<dat.data<<" "<<dat.lineNumber<<" "<<functionNumber<<std::endl;
                }
                //Get variable decls
                if(triggerField[decl_stmt] && triggerField[decl] && !(triggerField[type] || triggerField[init])){
                    auto dat = SliceVarMetaDataStack.back().back();
                    
                    sysDict.nameTable.insert(std::make_pair(dat.lineNumber, dat.data));
                    sysDict.reverseNameTable.insert(std::make_pair(dat.data, dat.lineNumber));
                    
                    openSliceProfiles.insert(std::make_pair(dat.lineNumber, SliceProfile(dat.lineNumber, fileNumber, functionNumber, dat.lineNumber)));
                    //std::cerr<<dat.lineNumber<<" "<<dat.data<<" "<<functionNumber<<std::endl;
                }
                //Get Param names
                if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !triggerField[type]){
                    auto dat = SliceVarMetaDataStack.back().back();
                    
                    sysDict.nameTable.insert(std::make_pair(dat.lineNumber, dat.data));
                    sysDict.reverseNameTable.insert(std::make_pair(dat.data, dat.lineNumber));
                    
                    openSliceProfiles.insert(std::make_pair(dat.lineNumber, SliceProfile(dat.lineNumber, fileNumber, functionNumber, dat.lineNumber)));
                    //std::cerr<<dat.lineNumber<<" "<<dat.data<<" "<<functionNumber<<std::endl;
                }
                //Get Expression Statement lhs
                if(triggerField[expr_stmt] && triggerField[expr] && exprstmt.numNames == 0){
                    auto dat = SliceVarMetaDataStack.back().back();
                    //std::cerr<<"Lhs"<<dat.lineNumber<<" "<<dat.data<<" "<<functionNumber<<std::endl;
                    exprstmt.lhs = dat.data;
                    ++exprstmt.numNames;
                    //std::cerr<<SliceVarMetaDataStack.back().back().data<<std::endl;
                }
                //get rhs
                if(triggerField[expr_stmt] && triggerField[expr] && exprstmt.numNames > 0){
                    auto dat = SliceVarMetaDataStack.back().back();
                    exprstmt.rhs = dat.data;
                    std::cerr<<"expr: "<<exprstmt.lhs<<" "<<exprstmt.op<<" "<<exprstmt.rhs<<std::endl;
                    //std::cerr<<SliceVarMetaDataStack.back().back().data<<std::endl;
                }
                //Name of function being called
                if(triggerField[call] && triggerField[name] == 1 && !(triggerField[argument_list])){
                    auto dat = SliceVarMetaDataStack.back().back();
                    currentFunction = dat.data;
                    //std::cerr<<currentFunction<<std::endl;
                }
                //function args
                if(triggerField[call] && triggerField[argument_list] && triggerField[argument] && triggerField[expr] && triggerField[name] == 1){
                    auto dat = SliceVarMetaDataStack.back().back();
                    auto nameStr = sysDict.reverseNameTable.find(dat.data);
                    //std::cerr<<currentFunction<<std::endl;
                    auto calledFuncIt = sysDict.reverseFunctionTable.find(currentFunction);
                    unsigned int localFuncNum = 0;
                    if(calledFuncIt != sysDict.reverseFunctionTable.end()){
                        localFuncNum = calledFuncIt->second;
                    }else{
                        sysDict.reverseFunctionTable.insert(std::make_pair(currentFunction, functionNumber));
                        sysDict.functionTable.insert(std::make_pair(functionNumber,currentFunction));
                        localFuncNum = functionNumber;
                        ++functionNumber;
                    }
                    if(nameStr != sysDict.reverseNameTable.end()){
                        auto sp = openSliceProfiles.find(nameStr->second);
                        if(sp != openSliceProfiles.end()){
                            sp->second.slines.push_back(dat.lineNumber);
                            sp->second.index = dat.lineNumber -functionLineNumber;
                            sp->second.cfunctions.push_back(std::make_pair(localFuncNum, dat.lineNumber - functionLineNumber));
                        }
                    }
                    //std::cerr<<dat.lineNumber<<" "<<dat.data<<" "<<functionNumber<<std::endl;
                    //std::cerr<<SliceVarMetaDataStack.back().back().data<<std::endl;
                }
                --triggerField[name]; 
            }
        }else{
            if(!SliceVarMetaDataStack.empty()){
                --triggerField[nonterminal];                
            }
        }
    }
    
    /*
    virtual void comment(const char * value) {}
    virtual void cdataBlock(const char * value, int len) {}
    virtual void processingInstruction(const char * target, const char * data) {}
    */

#pragma GCC diagnostic pop

};

#endif