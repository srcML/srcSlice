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

class srcSliceHandler : public srcSAXHandler {

private :
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, argument_list, call, ctrlflow, endflow, name, function, argument, block, type, init, op, literal, modifier, nonterminal, empty, MAXENUMVALUE = empty};
    struct ExprStmt{
        ExprStmt(){
            ln = 0;
        }
        std::string lhs;
        std::string op;
        std::string rhs;
        int ln;
    };
    struct DeclStmt{
        DeclStmt(){
            ln = 0;
        }
        std::string type;
        std::string name;
        int ln;
    };
    struct FunctionData{
        FunctionData(){
            functionNumber = 0;
            functionLineNumber = 0;
        }
        std::string returnType;
        std::string functionName;
        
        std::vector<std::string> argumentTypes;
        
        ExprStmt exprstmt;
        DeclStmt declstmt;

        unsigned int functionNumber;
        unsigned int functionLineNumber;
    };
    unsigned int fileNumber;
    
    unsigned int NumOfMostRecentlySeenFunction;
    unsigned int ctorNumber;

    FunctionData functionTmplt;

    std::string previousDeclVarName;

    std::string nameOfCurrentClldFcn;
    
    bool opeq; //flag to tell me when I've seen an assignment op
    bool potentialAlias; //flag to tell me when the last name could be used as an alias.

    int numArguments;

    std::vector<NameLineNumberPair> SliceVarMetaDataStack;
    std::vector<unsigned short int> triggerField;

    SystemDictionary sysDict;
    std::unordered_map<std::string, SliceProfile> openSliceProfiles;


public :
    srcSliceHandler(){
        fileNumber = 0;
        numArguments = 0;
        ctorNumber = 0;
        NumOfMostRecentlySeenFunction = 0;
        nameOfCurrentClldFcn = "global";

        opeq = false;
        potentialAlias = false;

        nameOfCurrentClldFcn = "";
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
            ++triggerField[decl_stmt];
        }else if(std::string(localname) == "function"){
            ++triggerField[function];
        }else if(std::string(localname) == "constructor"){
            ++ctorNumber;
            ++triggerField[function];
        }else if (std::string(localname) == "expr_stmt"){
            ++triggerField[expr_stmt];
        }else if (std::string(localname) == "parameter_list"){
            ++triggerField[parameter_list];
        }else if (std::string(localname) == "argument_list"){
            ++triggerField[argument_list];
        }else if (std::string(localname) == "call"){
            ++triggerField[call];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] || 
            triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (std::string(localname) == "param"){
                    ++triggerField[param];
            }else if(std::string(localname) == "operator"){
                    //functionTmplt.exprstmt.op = "";SliceMetaData(lineNum, op, "")
                    SliceVarMetaDataStack.push_back(std::make_pair("", lineNum));
                    ++triggerField[op];
            }else if (std::string(localname) == "block"){ //So I can discriminate against things in or outside of blocks
                    ++triggerField[block];
            }else if (std::string(localname) == "init"){ //So I can discriminate against things in or outside of blocks
                    ++triggerField[init];
            }else if (std::string(localname) == "argument"){
                    ++numArguments;
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
                SliceVarMetaDataStack.push_back(std::make_pair("", lineNum));
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
            if((triggerField[name] || triggerField[argument_list]) && !triggerField[expr_stmt]){
                SliceVarMetaDataStack.back().first = content;
                //std::cerr<<content<<std::endl;
            }else if(triggerField[expr_stmt] && triggerField[name] && !triggerField[call]){
                if(!opeq){
                    functionTmplt.exprstmt.lhs += content;
                }else{
                    functionTmplt.exprstmt.rhs += content;
                }
            }else if(triggerField[op]){
                
                //functionTmplt.exprstmt.lhs += content;
                if(content == "=" && triggerField[expr_stmt]){
                    functionTmplt.exprstmt.op = "=";
                    opeq = true;
                }else if (content != "=" && !opeq && triggerField[expr_stmt]){
                    SliceVarMetaDataStack.back().first = content;
                    functionTmplt.exprstmt.lhs += content;
                }else if(content != "=" && opeq && triggerField[expr_stmt]){
                    SliceVarMetaDataStack.back().first = content;
                    functionTmplt.exprstmt.rhs += content;
                }
            }
            if(triggerField[literal]){ //'cause I don't care about literals
                functionTmplt.exprstmt.rhs="";
                functionTmplt.exprstmt.lhs="";
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
            std::cerr<<functionTmplt.declstmt.type<<std::endl;
            functionTmplt.declstmt.type = "";
            --triggerField[decl_stmt];
        }else if(std::string(localname) == "function"){
            --triggerField[function];
        }else if(std::string(localname) == "constructor"){
            --triggerField[function];
        }else if (std::string(localname) == "expr_stmt"){
            --triggerField[expr_stmt];
            
            functionTmplt.exprstmt.lhs = "";
            functionTmplt.exprstmt.rhs = "";
            functionTmplt.exprstmt.op = "";
            functionTmplt.exprstmt.ln = 0;

            opeq = false;
            
        }else if (std::string(localname) == "parameter_list"){
            --triggerField[parameter_list];
        }else if (std::string(localname) == "argument_list"){
            numArguments = 0;
            --triggerField[argument_list];
        }else if (std::string(localname) == "call"){
            //nameOfCurrentClldFcn = "";
            --triggerField[call];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] 
            || triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (std::string(localname) == "param"){
                --triggerField[param];
            }else if (std::string(localname) == "literal"){
                --triggerField[literal];
            }else if (std::string(localname) == "modifier"){
                if(triggerField[decl]){ //only care about modifiers in decls
                    potentialAlias = true;
                }
                --triggerField[modifier];
            }else if (std::string(localname) == "argument"){
                --triggerField[argument];
            }else if (std::string(localname) == "block"){
                --triggerField[block];
            }else if(std::string(localname) == "decl"){
                --triggerField[decl];                 
            }else if(std::string(localname) == "init"){
                --triggerField[init];                 
            }else if (std::string(localname) == "expr"){
                GetExprStmtParts();
                --triggerField[expr]; 
            }else if (std::string(localname) == "type"){
                --triggerField[type]; 
            }else if (std::string(localname) == "operator"){
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
                //Name of function being called
                if(triggerField[call] && triggerField[name] == 1 && !(triggerField[argument_list])){
                    auto dat = SliceVarMetaDataStack.back();
                    nameOfCurrentClldFcn = dat.first;
                    //std::cerr<<nameOfCurrentClldFcn<<std::endl;
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
        //Get function arguments
        if(triggerField[argument_list] && triggerField[argument] && 
            triggerField[expr] && triggerField[name] == 1){
            
            auto dat = SliceVarMetaDataStack.back();
            //std::cerr<<nameOfCurrentClldFcn<<std::endl;
            int localFunc = 0;
            auto calledFuncIt = sysDict.reverseFunctionTable.find(nameOfCurrentClldFcn);
            if(calledFuncIt != sysDict.reverseFunctionTable.end()){
                localFunc = calledFuncIt->second;
            }else{
                sysDict.reverseFunctionTable.insert(std::make_pair(nameOfCurrentClldFcn, NumOfMostRecentlySeenFunction));
                sysDict.functionTable.insert(std::make_pair(NumOfMostRecentlySeenFunction,nameOfCurrentClldFcn));
                localFunc = NumOfMostRecentlySeenFunction;
                ++NumOfMostRecentlySeenFunction;
            }
            auto sp = openSliceProfiles.find(functionTmplt.functionName+":"+dat.first);
            if(sp != openSliceProfiles.end()){
                sp->second.slines.push_back(dat.second);
                sp->second.index = dat.second - functionTmplt.functionLineNumber;
                sp->second.cfunctions.push_back(std::make_pair(localFunc, numArguments));
                //dat.second - functionTmplt.functionLineNumber
            }
        }
    }
    void GetFunctionData(){
        //Get function type
        if(triggerField[type]){
            auto dat = SliceVarMetaDataStack.back();
            //std::cerr<<"type: "<<dat.first<<std::endl;
        }
        //Get function name
        if(triggerField[name] == 1 && !(triggerField[argument_list] || 
            triggerField[block] || triggerField[type] || triggerField[parameter_list])){
            
            auto dat = SliceVarMetaDataStack.back();
            
            ++NumOfMostRecentlySeenFunction;
            
            functionTmplt.functionNumber = NumOfMostRecentlySeenFunction;
            functionTmplt.functionName = dat.first;                      
            sysDict.reverseFunctionTable.insert(std::make_pair(dat.first, NumOfMostRecentlySeenFunction));
            sysDict.functionTable.insert(std::make_pair(NumOfMostRecentlySeenFunction,dat.first));
            //std::cerr<<dat.first<<" "<<dat.second<<" "<<NumOfMostRecentlySeenFunction<<std::endl;
        }
        //Get Param names
        if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !triggerField[type]){
            auto dat = SliceVarMetaDataStack.back();
            openSliceProfiles.insert(std::make_pair(functionTmplt.functionName+":"+dat.first, SliceProfile(dat.second - functionTmplt.functionLineNumber, fileNumber, NumOfMostRecentlySeenFunction, dat.second)));
        }
        
    }
    void GetDeclStmtData(){
        if(triggerField[type]){
            auto dat = SliceVarMetaDataStack.back();
            //std::cerr<<dat.first<<std::endl;
            functionTmplt.declstmt.type += dat.first;
            //agglomerate string into type. Clear when we exit the decl_stmt
        }
        //Get name of decl stmt
        if(triggerField[decl] && !(triggerField[type] || triggerField[init])){
            auto dat = SliceVarMetaDataStack.back();                                        
                    
            previousDeclVarName = dat.first; //keep track of last declared var, need it for init.
            openSliceProfiles.insert(std::make_pair(functionTmplt.functionName+":"+dat.first, SliceProfile(dat.second - functionTmplt.functionLineNumber , fileNumber, functionTmplt.functionNumber, dat.second)));
                    
            //std::cerr<<dat.second<<" "<<functionTmplt.functionName+":"+dat.first<<" "<<functionTmplt.functionNumber<<std::endl;
            potentialAlias = false;
        }
        //Deal with decl's that have an init in them.
        if(triggerField[decl] && triggerField[init]){
            auto dat = SliceVarMetaDataStack.back();
            //std::cerr<<"Tried: "<<functionTmplt.functionName+":"+dat.first<<std::endl;
            auto sp = openSliceProfiles.find(functionTmplt.functionName+":"+dat.first);
            if(sp != openSliceProfiles.end()){
                //Either has to be a variable local to this function or a global variable (function == 0)
                //std::cerr<<"Found: "<<dat.first<<" "<<functionTmplt.functionName+":"+dat.first<<" "<<sp->second.function<<" "<<functionTmplt.functionNumber<<sysDict.functionTable.find(sp->second.function)->second<<std::endl;
                if(sp->second.function == functionTmplt.functionNumber || sp->second.function == 0){
                    auto lastSp = openSliceProfiles.find(functionTmplt.functionName+":"+previousDeclVarName);
                    if(lastSp != openSliceProfiles.end()){                                    
                        //std::cerr<<"dat: "<<dat.first<<" "<<previousDeclVarName<<" "<<sp->second.function<<" "<<functionTmplt.functionNumber<<std::endl;
                        lastSp->second.dvars.push_back(dat.first); 
                    }
                }
            }            
        }
        //Get Init of decl stmt
    }
    void GetExprStmtParts(){
        if(triggerField[expr_stmt] && !triggerField[call]){
            if(!(functionTmplt.exprstmt.lhs.empty() || functionTmplt.exprstmt.rhs.empty() || functionTmplt.exprstmt.op.empty())){
                ///TODO: This won't work for all expressions
                functionTmplt.exprstmt.lhs = RemoveWhiteSpace(std::move(functionTmplt.exprstmt.lhs));
                functionTmplt.exprstmt.rhs = RemoveWhiteSpace(std::move(functionTmplt.exprstmt.rhs));                        
                int posOfDotRhs = functionTmplt.exprstmt.rhs.find('.');
                if(posOfDotRhs != std::string::npos){
                    functionTmplt.exprstmt.rhs.erase(posOfDotRhs, functionTmplt.exprstmt.rhs.size()-1);
                }
                int posOfDotLhs = functionTmplt.exprstmt.lhs.find('.');
                if(posOfDotLhs != std::string::npos){
                    functionTmplt.exprstmt.lhs.erase(posOfDotLhs, functionTmplt.exprstmt.lhs.size()-1);
                }
                if(functionTmplt.exprstmt.lhs != functionTmplt.exprstmt.rhs){
                    auto spIt = openSliceProfiles.find(functionTmplt.functionName+":"+functionTmplt.exprstmt.lhs);
                    if(spIt != openSliceProfiles.end()){
                        spIt->second.dvars.push_back(functionTmplt.exprstmt.rhs);
                        spIt->second.slines.push_back(functionTmplt.exprstmt.ln);
                    }
                }//TODO: Implement the rest of this for aliases.
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
/*

                //get rhs
                if(triggerField[expr_stmt] && triggerField[expr] && exprstmt.numNames > 0){
                    auto dat = SliceVarMetaDataStack.back();
                    exprstmt.rhs = dat.first;
                    std::cerr<<"expr: "<<exprstmt.lhs<<" "<<exprstmt.op<<" "<<exprstmt.rhs<<std::endl;
                    //std::cerr<<SliceVarMetaDataStack.back().data<<std::endl;
                }
                //Name of function being called
                if(triggerField[call] && triggerField[name] == 1 && !(triggerField[argument_list])){
                    auto dat = SliceVarMetaDataStack.back();
                    nameOfCurrentClldFcn = dat.first;
                    //std::cerr<<nameOfCurrentClldFcn<<std::endl;
                }*/
