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
#include <locale>
#include <algorithm>
/**
 * srcSliceHandler
 *
 * Base class that provides hooks for SAX processing.
 */
inline std::string RemoveWhiteSpace(std::string str){
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return str;
}
typedef std::pair<std::string, unsigned int> NameLineNumberPair;
typedef std::pair<std::string, std::string> TypeNamePair;

class srcSliceHandler : public srcSAXHandler {

private :
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, argument_list, call, ctrlflow, endflow, name, function, argument, block, type, init, op, literal, modifier, nonterminal, empty, MAXENUMVALUE = empty};

    unsigned int fileNumber;
    unsigned int NumOfMostRecentlySeenFunction;
    unsigned int numArgs;

    FunctionData functionTmplt;

    std::string previousDeclVarName;
    std::string nameOfCurrentClldFcn;
    std::string currentOp;

    std::vector<NameLineNumberPair> SliceVarMetaDataStack;
    std::vector<unsigned short int> triggerField;

    SystemDictionary sysDict;
    std::unordered_map<std::string, SliceProfile> openSliceProfiles;
    bool flag;

public :
    srcSliceHandler(){
        fileNumber = 0;
        numArgs = 0;
        flag = false;

        NumOfMostRecentlySeenFunction = 0;
        nameOfCurrentClldFcn = "global";

        triggerField = std::vector<unsigned short int>(MAXENUMVALUE, 0);
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
            SliceVarMetaDataStack.back().first = "";
            ++triggerField[decl_stmt];
        }else if(std::string(localname) == "function" || std::string(localname) == "constructor" || std::string(localname) == "destructor"){
            SliceVarMetaDataStack.back().first = "";
            ++triggerField[function];
        }else if (std::string(localname) == "expr_stmt"){
            SliceVarMetaDataStack.back().first = "";
            ++triggerField[expr_stmt];
        }else if (std::string(localname) == "parameter_list"){
            ++triggerField[parameter_list];
        }else if (std::string(localname) == "argument_list"){
            ++triggerField[argument_list];
        }else if (std::string(localname) == "call"){
            SliceVarMetaDataStack.back().first = "";
            ++triggerField[call];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] || 
            triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (std::string(localname) == "param"){
                    if(!SliceVarMetaDataStack.empty())
                    SliceVarMetaDataStack.back().first = "";
                    ++triggerField[param];
            }else if(std::string(localname) == "operator"){
                    currentOp = "";
                    //functionTmplt.exprstmt.op = "";SliceMetaData(lineNum, op, "")
                    //SliceVarMetaDataStack.push_back(std::make_pair("", lineNum));
                    ++triggerField[op];
            }else if (std::string(localname) == "block"){ //So I can discriminate against things in or outside of blocks
                    SliceVarMetaDataStack.back().first = "";
                    ++triggerField[block];
            }else if (std::string(localname) == "init"){ //So I can discriminate against things in or outside of blocks
                    SliceVarMetaDataStack.back().first = ""; //so that I can get more stuff after the decl's name
                    ++triggerField[init];
            }else if (std::string(localname) == "argument"){
                    ++numArgs;
                    if(triggerField[init]){
                        //Only if we're in init because templates can have arguments too
                        SliceVarMetaDataStack.back().first = "";
                    }
                    ++triggerField[argument];
            }else if (std::string(localname) == "literal"){
                    ++triggerField[literal];
            }else if (std::string(localname) == "modifier"){
                    ++triggerField[modifier];
            }else if(std::string(localname) == "decl"){
                    ++triggerField[decl]; 
            }else if(std::string(localname) == "type"){
                    ++triggerField[type]; 
            }else if (std::string(localname) == "expr"){
                    ++triggerField[expr];
            }else if (std::string(localname) == "name"){
                ++triggerField[name];
                if(triggerField[function] && triggerField[type] && !triggerField[block]){
                    if(num_attributes){
                        functionTmplt.functionLineNumber = strtoul(attributes[0].value, NULL, 0);
                    }
                }else if(triggerField[expr_stmt] && triggerField[expr]){
                    if(num_attributes){
                        functionTmplt.exprstmt.ln = strtoul(attributes[0].value, NULL, 0);
                    }
                }
                if(SliceVarMetaDataStack.size() < 1){
                    SliceVarMetaDataStack.push_back(std::make_pair("", lineNum));    
                }
            }else{
                    ++triggerField[nonterminal]; 
            }   
        }
    }
        //functionTmplt.functionLineNumber = strtoul(attributes[0].value, NULL, 0);
        //push_element(localname, prefix, lineNum);

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
        /*everything below the first nested if-statement is to handle expr_stmts because they're currently a huge corner case.
        The first else if deals with collecting the left and right hand side data from an expr_stmt
        The second else if deals with collecting operator information like . and = and ->
        I'm certain there's a better way to accompish this, but it's a pain in the ass so I'm not dealing with it
        just yet.*/ 
        if(!SliceVarMetaDataStack.empty()){
            //auto currentStack = SliceVarMetaDataStack.back();
            if((triggerField[name] || triggerField[op]) && content != "="){
                SliceVarMetaDataStack.back().first += content;
                //std::cerr<<SliceVarMetaDataStack.back().first<<std::endl;
            }
            if(triggerField[op]){
                currentOp = content;
            }
            if(content == "="){
                functionTmplt.exprstmt.opeq = true;
                functionTmplt.exprstmt.op = "=";
            }
            if(triggerField[literal]){
                functionTmplt.declstmt.clear();
                functionTmplt.exprstmt.clear();
            }
        }
    }

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
        if(std::string(localname) == "decl_stmt"){
            //std::cerr<<functionTmplt.declstmt.type<<" "<<functionTmplt.declstmt.name<<std::endl;
            auto dat = SliceVarMetaDataStack.back();
            
            if(functionTmplt.declstmt.potentialAlias){
                //std::cerr<<"decl: "<<functionTmplt.functionName<<" "<<functionTmplt.declstmt.name<<std::endl;

            }

            SliceVarMetaDataStack.back().first = "";
            functionTmplt.declstmt.clear();
            --triggerField[decl_stmt];
        }else if (std::string(localname) == "expr_stmt"){
            //std::cerr<<"expr: "<<functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.op<<" "<<functionTmplt.exprstmt.rhs<<std::endl;
            if(!(functionTmplt.exprstmt.lhs.empty() || functionTmplt.exprstmt.rhs.empty() || functionTmplt.exprstmt.op.empty())){
                ///TODO: This won't work for all expressions
                //std::cerr<<functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.lhs<<std::endl;
                functionTmplt.exprstmt.lhs = RemoveWhiteSpace(std::move(functionTmplt.exprstmt.lhs));
                functionTmplt.exprstmt.rhs = RemoveWhiteSpace(std::move(functionTmplt.exprstmt.rhs));                        
                auto posOfDotRhs = std::find_if(functionTmplt.exprstmt.rhs.begin(), 
                    functionTmplt.exprstmt.rhs.end(), [](char x){
                        return !std::isalnum(x);
                    });
                if(posOfDotRhs != functionTmplt.exprstmt.rhs.end()){
                    functionTmplt.exprstmt.rhs.erase(posOfDotRhs, functionTmplt.exprstmt.rhs.end());
                }
                auto posOfDotLhs = std::find_if(functionTmplt.exprstmt.lhs.begin(), 
                    functionTmplt.exprstmt.lhs.end(), [](char x){
                        return !std::isalnum(x);
                    });
                if(posOfDotLhs != functionTmplt.exprstmt.rhs.end()){
                    functionTmplt.exprstmt.lhs.erase(posOfDotLhs, functionTmplt.exprstmt.lhs.end());
                }
                //std::cerr<<functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.rhs<<std::endl;
                if(functionTmplt.exprstmt.lhs != functionTmplt.exprstmt.rhs){
                    auto spIt = openSliceProfiles.find(functionTmplt.functionName+":"+functionTmplt.exprstmt.lhs);
                    if(spIt != openSliceProfiles.end()){
                        //std::cerr<<functionTmplt.functionName+":"+functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.rhs<<std::endl;
                        spIt->second.dvars.push_back(functionTmplt.exprstmt.rhs);
                        spIt->second.slines.push_back(functionTmplt.exprstmt.ln);
                    }
                }//TODO: Implement the rest of this for aliases.
            }
            --triggerField[expr_stmt];

            SliceVarMetaDataStack.back().first = "";
            
            functionTmplt.exprstmt.clear();
            functionTmplt.exprstmt.opeq = false;
            
        }else if(std::string(localname) == "function" || std::string(localname) == "constructor" || std::string(localname) == "destructor"){
            
            sysDict.functionTable.insert(std::make_pair(functionTmplt.functionName, functionTmplt));

            //std::cerr<<functionTmplt.returnType<<" "<<functionTmplt.functionName<<std::endl;
/*            
            for(auto argn : functionTmplt.arguments){
                if(argn.potentialAlias == true)
                std::cerr<<"Arg: "<<argn.type<<" "<<argn.name<<std::endl;
            }
            std::cerr<<functionTmplt.functionLineNumber<<std::endl;
            std::cerr<<functionTmplt.functionNumber<<std::endl;

            std::cerr<<"----------------------------"<<std::endl;
            /**/
            functionTmplt.clear();
            SliceVarMetaDataStack.back().first = "";
            --triggerField[function];
        }else if (std::string(localname) == "parameter_list"){
            --triggerField[parameter_list];
        }else if (std::string(localname) == "argument_list"){
            numArgs = 0;
            --triggerField[argument_list];
        }else if (std::string(localname) == "call"){
            //std::cerr<<nameOfCurrentClldFcn<<std::endl;
            flag = false; //to tell me if I'm in a nested name because if I am I have to do special stuff. Only for calls.
            nameOfCurrentClldFcn = "";
            --triggerField[call];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] 
            || triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (std::string(localname) == "param"){
                --triggerField[param];
            }else if (std::string(localname) == "literal"){
                --triggerField[literal];
            }else if (std::string(localname) == "modifier"){
                if(triggerField[decl_stmt] && triggerField[decl]){ //only care about modifiers in decls
                    functionTmplt.declstmt.potentialAlias = true;
                }else if(triggerField[function] && triggerField[parameter_list] && triggerField[param] && triggerField[decl]){
                    functionTmplt.arg.potentialAlias = true;
                }
                --triggerField[modifier];
            }else if (std::string(localname) == "argument"){
                --triggerField[argument];
            }else if (std::string(localname) == "block"){
                --triggerField[block];
            }else if(std::string(localname) == "decl"){
                //std::cerr<<functionTmplt.returnType<<std::endl;
                --triggerField[decl];                 
            }else if(std::string(localname) == "init"){
                --triggerField[init];                 
            }else if (std::string(localname) == "expr"){    
                --triggerField[expr]; 
            }else if (std::string(localname) == "type"){
                if(triggerField[decl_stmt] || (triggerField[function] && ! triggerField[block])) {
                    SliceVarMetaDataStack.back().first.clear();
                }
                --triggerField[type]; 
            }else if (std::string(localname) == "operator"){
                if(currentOp == "=")
                    SliceVarMetaDataStack.back().first = "";
                
                --triggerField[op];
            }
            else if (std::string(localname) == "name"){
                //Get Function names and arguments
                if(triggerField[function]){
                    GetFunctionData();
                }
                //Get variable decls
                if(triggerField[decl_stmt]){
                    GetDeclStmtData();
                }
                if(triggerField[expr_stmt]){
                    GetExprStmtParts();
                }
                
                //Name of function being called
                if(triggerField[call] && triggerField[name] && !(triggerField[argument_list])){
                    if(triggerField[name] > 1){
                        flag = true;
                    }
                    auto dat = SliceVarMetaDataStack.back();
                    if(triggerField[name] == 1 && flag == false){
                        nameOfCurrentClldFcn = dat.first;
                    }else if(triggerField[name] == 2){
                        nameOfCurrentClldFcn = dat.first;
                    }
                    
                    //SliceVarMetaDataStack.back().first = "";
                    //std::cerr<<dat.first<<std::endl;
                }
                //Get function arguments
                if(triggerField[call]){
                    GetCallData();
                }
                --triggerField[name];

            }
        }
    }
    void GetCallData(){
        //std::cerr<<nameOfCurrentClldFcn<<std::endl;
        //Get function arguments
        if(triggerField[argument_list] && triggerField[argument] && 
            triggerField[expr] && triggerField[name]){
           
            
            NameLineNumberPair dat = SliceVarMetaDataStack.back();
            
            dat.first.erase(
                std::find_if(dat.first.begin(), dat.first.end(), 
                    [](char x){
                        return !std::isalnum(x);
                    }), dat.first.end());

            auto sp = openSliceProfiles.find(functionTmplt.functionName+":"+dat.first);
            if(sp != openSliceProfiles.end()){
                sp->second.slines.push_back(dat.second);
                sp->second.index = dat.second - functionTmplt.functionLineNumber;
                sp->second.cfunctions.push_back(std::make_pair(nameOfCurrentClldFcn, numArgs));
                //dat.second - functionTmplt.functionLineNumber
            }
            
            //std::cerr<<nameOfCurrentClldFcn<<" "<<dat.first<<std::endl;       
        }
    }
    void GetFunctionData(){
        //Get function type
        if(triggerField[type] && !(triggerField[parameter_list] || triggerField[block])){
            NameLineNumberPair dat = SliceVarMetaDataStack.back();
            functionTmplt.returnType = dat.first;
        }
        //Get function name
        if(triggerField[name] == 1 && !(triggerField[argument_list] || 
            triggerField[block] || triggerField[type] || triggerField[parameter_list])){
            NameLineNumberPair dat = SliceVarMetaDataStack.back();
            
            ++NumOfMostRecentlySeenFunction;
            
            std::size_t pos = dat.first.find("::");
            if(pos != std::string::npos){
                dat.first.erase(0, pos+2);
            }
            
            functionTmplt.functionName = dat.first;
            functionTmplt.functionNumber = NumOfMostRecentlySeenFunction;

            //std::cerr<<dat.first<<std::endl;
            
        }
        //Get param types
        if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type] && !triggerField[block]){
            functionTmplt.arg.type = SliceVarMetaDataStack.back().first;
            //functionTmplt.arguments.push_back(std::make_pair(SliceVarMetaDataStack.back().first, ""));
        }
        //Get Param names
        if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[type] || triggerField[block])){
            NameLineNumberPair dat = SliceVarMetaDataStack.back();

            functionTmplt.arg.name = dat.first;
            functionTmplt.arguments.push_back(functionTmplt.arg);
            
            //std::cerr<<"Param: "<<functionTmplt.declstmt.type <<" "<<functionTmplt.functionName+":"+functionTmplt.arg.name<<std::endl;
            
            openSliceProfiles.insert(std::make_pair(functionTmplt.functionName+":"+functionTmplt.arg.name, 
                SliceProfile(functionTmplt.arg.ln - functionTmplt.functionLineNumber, fileNumber, 
                    NumOfMostRecentlySeenFunction, functionTmplt.arg.ln, functionTmplt.arg.name)));
        }

        
    }
    void GetDeclStmtData(){
        if(triggerField[decl] && triggerField[type] && !(triggerField[init])){
            NameLineNumberPair dat = SliceVarMetaDataStack.back();
            //std::cerr<<dat.first<<std::endl;
            functionTmplt.declstmt.type = dat.first;
            //agglomerate string into type. Clear when we exit the decl_stmt
        }
        //Get name of decl stmt
        if(triggerField[decl] && !(triggerField[type] || triggerField[init])){
            NameLineNumberPair dat = SliceVarMetaDataStack.back();
            functionTmplt.declstmt.name = dat.first;
            functionTmplt.declstmt.ln = dat.second;

            openSliceProfiles.insert(std::make_pair(functionTmplt.functionName+":"+functionTmplt.declstmt.name, 
                SliceProfile(functionTmplt.declstmt.ln - functionTmplt.functionLineNumber, fileNumber, 
                    functionTmplt.functionNumber, functionTmplt.declstmt.ln, functionTmplt.declstmt.name)));

            //std::cerr<<"Decl: "<<functionTmplt.declstmt.type <<" "<<functionTmplt.functionName+":"+functionTmplt.declstmt.name<<std::endl;
            //functionTmplt.declstmt.potentialAlias = false;
        }
        //Deal with decl's that have an init in them.
        if(triggerField[decl] && triggerField[init] && (triggerField[call] || triggerField[expr])){
            NameLineNumberPair dat = SliceVarMetaDataStack.back();
            dat.first.erase(
                std::find_if(dat.first.begin(), dat.first.end(), 
                    [](char x){
                        return !std::isalnum(x);
                    }), dat.first.end());
            //std::cerr<<"Tried: "<<functionTmplt.functionName+":"+dat.first<<std::endl;
            auto sp = openSliceProfiles.find(functionTmplt.functionName+":"+dat.first);
            if(sp != openSliceProfiles.end()){
                //Either has to be a variable local to this function or a global variable (function == 0)
                //std::cerr<<"Found: "<<functionTmplt.declstmt.name<<" "<<functionTmplt.functionName+":"+dat.first<<" "<<sp->second.function<<" "<<functionTmplt.functionNumber<<" "<<functionTmplt.functionName+":"+previousDeclVarName<<std::endl;
                if(sp->second.function == functionTmplt.functionNumber || sp->second.function == 0){
                    auto lastSp = openSliceProfiles.find(functionTmplt.functionName+":"+functionTmplt.declstmt.name);
                    if(lastSp != openSliceProfiles.end()){                                    
                        //std::cerr<<"dat: "<<dat.first<<" "<<functionTmplt.declstmt.name<<" "<<sp->second.function<<" "<<functionTmplt.functionNumber<<std::endl;
                        lastSp->second.dvars.push_back(dat.first);
                        //check for aliases
                    }
                }
            }
            //std::cerr<<"Init: "<<dat.first<<" "<<functionTmplt.functionName<<std::endl;
            //Split on -> and . and then process
        }
        //Get Init of decl stmt
    }
    void GetExprStmtParts(){
        //std::cerr<<"op: "<<currentOp<<std::endl;
        if(triggerField[expr_stmt] && !triggerField[call]){
            auto dat = SliceVarMetaDataStack.back();
            if(functionTmplt.exprstmt.opeq == false){
                functionTmplt.exprstmt.lhs = dat.first;
            }else{
                functionTmplt.exprstmt.rhs = dat.first;
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