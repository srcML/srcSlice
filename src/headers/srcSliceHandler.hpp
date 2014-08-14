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
inline std::vector<std::string> Split(const std::string& str, const char* tok){
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
inline std::string StripModifier(std::string str){
    if(str[0] == '&' || str[0] == '*'){
        auto posOfDotRhs = std::find_if(str.begin(), 
                str.end(), [](const char x){
                    return (x == '&' || x=='*');
                });
        if(posOfDotRhs != str.end()){
            str.erase(posOfDotRhs, posOfDotRhs+1);
        }
    }else{
        auto posOfDotRhs = std::find_if(str.begin(), 
            str.end(), [](char x){
                return !std::isalnum(x);
            });
        if(posOfDotRhs != str.end()){
            str.erase(posOfDotRhs, str.end());
        }
    }

    return str;
}

typedef std::pair<std::string, unsigned int> NameLineNumberPair;
typedef std::pair<std::string, std::string> TypeNamePair;

class srcSliceHandler : public srcSAXHandler {

private :
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, argument_list, call, ctrlflow, endflow, name, function, argument, block, type, init, op, literal, modifier, nonterminal, empty, MAXENUMVALUE = empty};

    unsigned int fileNumber;
    unsigned int numArgs;
    
    char constructorNum;
    
    std::hash<std::string> hash_fn;

    FunctionData functionTmplt;

    std::string nameOfCurrentClldFcn;
    std::string currentOp;

    NameLineNumberPair currentNameAndLine;
    std::vector<unsigned short int> triggerField;

    
    
    FileFunctionVarMap::iterator FileIt;
    FunctionVarMap::iterator FunctionIt;


    bool flag;
    bool isConstructor;
public :
    SystemDictionary sysDict;
    srcSliceHandler(){
        fileNumber = 0;
        numArgs = 0;
        constructorNum = '0';

        flag = false;
        isConstructor = false;

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
        fileNumber = hash_fn(attributes[1].value);
        FileIt = sysDict.dictionary.insert(std::make_pair(fileNumber, FunctionVarMap())).first; //insert and keep track of most recent. 
        
        FunctionIt = FileIt->second.insert(std::make_pair(hash_fn(std::string(attributes[1].value).append("0")), VarMap())).first; //for globals. Makes a bad assumption about where globals are. Fix.
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
        std::string lname(localname);
        if(lname == "decl_stmt"){
            currentNameAndLine.first.clear();
            ++triggerField[decl_stmt];
        }else if(lname == "function" || lname == "constructor" || lname == "destructor"){
            if(lname == "constructor"){
                ++constructorNum;
                isConstructor = true;
            }
            currentNameAndLine.first.clear();
            ++triggerField[function];
        }else if (lname == "expr_stmt"){
            currentNameAndLine.first.clear();
            ++triggerField[expr_stmt];
        }else if (lname == "parameter_list"){
            ++triggerField[parameter_list];
        }else if (lname == "argument_list"){
            ++triggerField[argument_list];
        }else if (lname == "call"){

            ++triggerField[call];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] || 
            triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (lname == "param"){
                    currentNameAndLine.first.clear();
                    ++triggerField[param];
            }else if(lname == "operator"){
                    currentOp = "";
                    //functionTmplt.exprstmt.op = "";SliceMetaData(lineNum, op, "")
                    //currentNameAndLine.push_back(std::make_pair("", lineNum));
                    ++triggerField[op];
            }else if (lname == "block"){ //So I can discriminate against things in or outside of blocks
                    currentNameAndLine.first.clear();
                    ++triggerField[block];
            }else if (lname == "init"){ //So I can discriminate against things in or outside of blocks
                    currentNameAndLine.first.clear(); //so that I can get more stuff after the decl's name
                    ++triggerField[init];
            }else if (lname == "argument"){
                    ++numArgs;
                    if(triggerField[init] || triggerField[call]){
                        //Only if we're in init because templates can have arguments too
                        currentNameAndLine.first.clear();
                    }
                    ++triggerField[argument];
            }else if (lname == "literal"){
                    ++triggerField[literal];
            }else if (lname == "modifier"){
                    ++triggerField[modifier];
            }else if(lname == "decl"){
                    ++triggerField[decl]; 
            }else if(lname == "type"){
                    ++triggerField[type]; 
            }else if (lname == "expr"){
                    ++triggerField[expr];
            }else if (lname == "name"){
                ++triggerField[name];
                currentNameAndLine.second = lineNum;
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
        //auto currentStack = currentNameAndLine.first;
        if((triggerField[name] || triggerField[op] || triggerField[literal]) && content != "=" && content != "new"){
            currentNameAndLine.first.append(content);
            //std::cerr<<currentNameAndLine.first<<std::endl;
        }
        if(triggerField[op]){
            currentOp.append(content);
        }
        if(content == "="){
            functionTmplt.exprstmt.opeq = true;
            functionTmplt.exprstmt.op = "=";
        }
        if(triggerField[literal]){
            //functionTmplt.declstmt.clear();
            //functionTmplt.exprstmt.clear();
        }        
    }

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {
        //std::cerr<<sysDict.dictionary.size()<<std::endl;
    }
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
        std::string lname(localname);
        if(lname == "decl_stmt"){
            //std::cerr<<functionTmplt.declstmt.type<<" "<<functionTmplt.declstmt.name<<std::endl;
            auto dat = currentNameAndLine;
            ProcessDeclStmtInit();
            if(functionTmplt.declstmt.potentialAlias){
                //std::cerr<<"decl: "<<functionTmplt.functionName<<" "<<functionTmplt.declstmt.name<<std::endl;

            }

            currentNameAndLine.first.clear();
            functionTmplt.declstmt.clear();
            --triggerField[decl_stmt];
        }else if (lname == "expr_stmt"){
            //std::cerr<<"expr: "<<functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.op<<" "<<functionTmplt.exprstmt.rhs<<std::endl;
            ProcessExprStmtRhs();
            //TODO:check to see if lhs is an alias of rhs. Then done.
            
            --triggerField[expr_stmt];

            currentNameAndLine.first.clear();
            
            functionTmplt.exprstmt.clear();
            functionTmplt.exprstmt.opeq = false;
            
        }else if(lname == "function" || lname == "constructor" || lname == "destructor"){
            
            sysDict.functionTable.insert(std::make_pair(functionTmplt.functionNumber, functionTmplt));
            FunctionIt = FileIt->second.insert(std::make_pair(functionTmplt.functionNumber, VarMap())).first;

            //std::cerr<<functionTmplt.returnType<<" "<<functionTmplt.functionName<<std::endl;
            /*
            for(auto argn : functionTmplt.arguments){
                if(argn.potentialAlias == true)
                std::cerr<<"Arg: "<<argn.type<<" "<<argn.name<<std::endl;
            }
            std::cerr<<functionTmplt.functionLineNumber<<std::endl;
            std::cerr<<functionTmplt.functionNumber<<std::endl;

            std::cerr<<"----------------------------"<<std::endl;
            */
            if(lname == "constructor"){
                isConstructor = false;
            }
            functionTmplt.clear();
            currentNameAndLine.first.clear();
            --triggerField[function];
        }else if (lname == "parameter_list"){
            --triggerField[parameter_list];
        }else if (lname == "argument_list"){
            numArgs = 0;
            --triggerField[argument_list];
        }else if (lname == "call"){
            //std::cerr<<nameOfCurrentClldFcn<<std::endl;
            flag = false; //to tell me if I'm in a nested name because if I am I have to do special stuff. Only for calls.
            nameOfCurrentClldFcn = "";
            --triggerField[call];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] 
            || triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (lname == "param"){
                --triggerField[param];
            }else if (lname == "literal"){
                if(triggerField[expr_stmt]){
                    functionTmplt.exprstmt.rhs = currentNameAndLine.first;
                }
                --triggerField[literal];
            }else if (lname == "modifier"){
                if(triggerField[decl_stmt] && triggerField[decl]){ //only care about modifiers in decls
                    functionTmplt.declstmt.potentialAlias = true;
                }else if(triggerField[function] && triggerField[parameter_list] && triggerField[param] && triggerField[decl]){
                    functionTmplt.arg.potentialAlias = true;
                }
                --triggerField[modifier];
            }else if (lname == "argument"){
                --triggerField[argument];
            }else if (lname == "block"){
                --triggerField[block];
            }else if(lname == "decl"){
                //std::cerr<<functionTmplt.returnType<<std::endl;
                --triggerField[decl];                 
            }else if(lname == "init"){
                --triggerField[init];                 
            }else if (lname == "expr"){    
                --triggerField[expr]; 
            }else if (lname == "type"){
                if(triggerField[decl_stmt] || (triggerField[function] && ! triggerField[block])) {
                    currentNameAndLine.first.clear();
                }
                --triggerField[type]; 
            }else if (lname == "operator"){
                if(currentOp == "=")
                    currentNameAndLine.first.clear();
                
                --triggerField[op];
            }
            else if (lname == "name"){
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
                    auto dat = currentNameAndLine;
                    if(triggerField[name] == 1 && flag == false){
                        nameOfCurrentClldFcn = dat.first;
                    }else if(triggerField[name] == 2){
                        nameOfCurrentClldFcn = dat.first;
                    }
                    
                    //currentNameAndLine.first.clear();
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
           
            
            NameLineNumberPair dat = currentNameAndLine;
            //std::cerr<<"CALLED: "<<functionTmplt.functionName+":"+dat.first<<std::endl;
            auto sp = FunctionIt->second.find(functionTmplt.functionName+":"+dat.first);
            if(sp != FunctionIt->second.end()){
                sp->second.slines.insert(dat.second);
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
            NameLineNumberPair dat = currentNameAndLine;
            functionTmplt.returnType = dat.first;
        }
        //Get function name
        if(triggerField[name] == 1 && !(triggerField[argument_list] || 
            triggerField[block] || triggerField[type] || triggerField[parameter_list])){
            NameLineNumberPair dat = currentNameAndLine;

            
            std::size_t pos = dat.first.find("::");
            if(pos != std::string::npos){
                dat.first.erase(0, pos+2);
            }
            if(isConstructor){
                dat.first+=constructorNum;
                //std::cerr<<"NAME: "<<dat.first<<std::endl;
            }
            functionTmplt.functionName = dat.first;
            functionTmplt.functionLineNumber = dat.second;
            functionTmplt.functionNumber = hash_fn(dat.first); //give me the hash num for this name.

            //std::cerr<<dat.first<<std::endl;
            
        }
        //Get param types
        if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type] && !triggerField[block]){
            functionTmplt.arg.type = currentNameAndLine.first;
            //functionTmplt.arguments.push_back(std::make_pair(currentNameAndLine.first, ""));
        }
        //Get Param names
        if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[type] || triggerField[block])){
            NameLineNumberPair dat = currentNameAndLine;

            functionTmplt.arg.name = dat.first;
            functionTmplt.arg.ln = dat.second;
            functionTmplt.arguments.push_back(functionTmplt.arg);
            
            //std::cerr<<"Param: "<<functionTmplt.arg.type <<" "<<functionTmplt.functionName+":"+functionTmplt.arg.name<<std::endl;
            
            FunctionIt->second.insert(std::make_pair(functionTmplt.functionName+":"+functionTmplt.arg.name, 
                SliceProfile(functionTmplt.arg.ln - functionTmplt.functionLineNumber, fileNumber, 
                    functionTmplt.functionNumber, functionTmplt.arg.ln, functionTmplt.arg.name, functionTmplt.arg.potentialAlias)));
        }

        
    }
    void GetDeclStmtData(){
        if(triggerField[decl] && triggerField[type] && !(triggerField[init])){
            NameLineNumberPair dat = currentNameAndLine;
            //std::cerr<<dat.first<<std::endl;
            functionTmplt.declstmt.type = dat.first;
            //agglomerate string into type. Clear when we exit the decl_stmt
        }
        //Get name of decl stmt
        if(triggerField[decl] && !(triggerField[type] || triggerField[init])){
            NameLineNumberPair dat = currentNameAndLine;
            functionTmplt.declstmt.name = dat.first;
            functionTmplt.declstmt.ln = dat.second;

            FunctionIt->second.insert(std::make_pair(functionTmplt.functionName+":"+functionTmplt.declstmt.name, 
                SliceProfile(functionTmplt.declstmt.ln - functionTmplt.functionLineNumber, fileNumber, 
                    functionTmplt.functionNumber, functionTmplt.declstmt.ln, 
                    functionTmplt.declstmt.name, functionTmplt.declstmt.potentialAlias)));

            //std::cerr<<"Decl: "<<functionTmplt.declstmt.type<<" "<<functionTmplt.declstmt.ln<<" "<<functionTmplt.functionName+":"+functionTmplt.declstmt.name<<std::endl;
            //functionTmplt.declstmt.potentialAlias = false;
        }
        //Get Init of decl stmt
    }
    void GetExprStmtParts(){
        //std::cerr<<"op: "<<currentOp<<std::endl;
        if(triggerField[expr_stmt]){
            auto dat = currentNameAndLine;
            functionTmplt.exprstmt.ln = dat.second;
            
            if(functionTmplt.exprstmt.opeq == false){
                //std::cerr<<"Got lhs: "<<dat.first<<std::endl;
                functionTmplt.exprstmt.lhs = dat.first;
                //functionTmplt.exprstmt.lhs = StripModifier(std::move(functionTmplt.exprstmt.lhs));
                //std::cerr<<"Got lhs: "<<functionTmplt.exprstmt.lhs<<std::endl;
            }else{
                functionTmplt.exprstmt.rhs = dat.first;
                //std::cerr<<"Got rhs: "<<functionTmplt.exprstmt.rhs<<std::endl;
            }

        }
        //std::cerr<<functionTmplt.exprstmt.lhs<<" "<<currentOp<<functionTmplt.exprstmt.rhs<<std::endl;
    }
    void ProcessDeclStmtInit(){
        auto resultVec = Split(currentNameAndLine.first, "+");
        for(auto rVecIt = resultVec.begin(); rVecIt != resultVec.end(); ++rVecIt){
            //std::cerr<<"Tried: "<<functionTmplt.declstmt.name<<" "<<functionTmplt.functionName+":"+*rVecIt<<std::endl;
            *rVecIt = StripModifier(std::move(*rVecIt));
            if(functionTmplt.declstmt.name != *rVecIt){ //lhs != rhs
                auto sp = FunctionIt->second.find(functionTmplt.functionName+":"+ *rVecIt);
                if(sp != FunctionIt->second.end()){
                    //Either has to be a variable local to this function or a global variable (function == 0)
                    //std::cerr<<"Found: "<<functionTmplt.declstmt.name<<" "<<functionTmplt.functionName+":"+*rVecIt<<" "<<sp->second.function<<" "<<functionTmplt.functionNumber<<" "<<functionTmplt.functionName+":"+<<std::endl;
                    if(sp->second.function == functionTmplt.functionNumber || sp->second.function == 0){
                        std::string fdname(functionTmplt.functionName+":"+functionTmplt.declstmt.name);
                        auto lastSp = FunctionIt->second.find(fdname);
                        //std::cerr<<"MMM: "<<fdname<<std::endl;
                        sp->second.slines.insert(currentNameAndLine.second);
                        if(lastSp != FunctionIt->second.end() && !lastSp->second.potentialAlias){ 
                            //std::cerr<<"dat: "<<*rVecIt<<" "<<functionTmplt.declstmt.name<<" "<<sp->second.function<<" "<<functionTmplt.functionNumber<<std::endl;
                            sp->second.dvars.insert(functionTmplt.functionName+":"+functionTmplt.declstmt.name);
                            
                            //check for aliases
                        }else if(lastSp != FunctionIt->second.end() && lastSp->second.potentialAlias){
                            //it's an alias for the rhs.
                            sp->second.isAlias = true;
                            sp->second.lastInsertedAlias = sp->second.aliases.insert(fdname).first;                            
                        }/*
                        if(!triggerField[call] && sp->second.potentialAlias){
                            //it's an alias for the rhs.
                            sp->second.isAlias = true;
                            sp->second.lastInsertedAlias = sp->second.aliases.insert(fdname).first;
                        }*/
                    }
                }
            }
        }
    }
    void ProcessExprStmtRhs(){
        
        ///TODO: This won't work for all expressions
        auto resultVecl = Split(functionTmplt.exprstmt.lhs, "+<.*->");
        //std::cerr<<"lhs: "<<functionTmplt.exprstmt.lhs<<std::endl;
        VarMap::iterator splIt;
        for(auto rVecIt = resultVecl.begin(); rVecIt!= resultVecl.end(); ++rVecIt){
            splIt = FunctionIt->second.find(functionTmplt.functionName+":"+*rVecIt);
            std::cerr<<"lhs: "<<functionTmplt.functionName+":"+*rVecIt<<std::endl;
            if(splIt != FunctionIt->second.end()){ //Found it so add statement line.
                splIt->second.slines.insert(functionTmplt.exprstmt.ln);
                //std::cerr<<"This: "<<splIt->second.variableName<<std::endl;
            }            
        }
        
        
        //std::cerr<<"DIS: "<<functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.rhs<<std::endl;
        if(!(splIt == FunctionIt->second.end() || functionTmplt.exprstmt.rhs.empty() || functionTmplt.exprstmt.op.empty())){
            auto resultVecr = Split(functionTmplt.exprstmt.rhs, "+<.*->");
            for(auto rVecIt = resultVecr.begin(); rVecIt != resultVecr.end(); ++rVecIt){
                //std::cerr<<functionTmplt.exprstmt.lhs<<" "<<functionTmplt.exprstmt.rhs<<std::endl;                    
                *rVecIt = StripModifier(std::move(*rVecIt));
                if(splIt->first != *rVecIt){//lhs !+ rhs    
                    auto sprIt = FunctionIt->second.find(functionTmplt.functionName+":"+*rVecIt);
                    std::cerr<<"That: "<<*rVecIt<<std::endl;
                    if(sprIt != FunctionIt->second.end()){ //lvalue depends on this rvalue
                        //std::cerr<<"ERP: "<<*rVecIt<<" "<<splIt->first<<" "<<functionTmplt.exprstmt.ln<<std::endl;
                        if(!(splIt == FunctionIt->second.end() && splIt->second.potentialAlias)){
                            sprIt->second.dvars.insert(splIt->first);
                            
                        }else{
                            sprIt->second.lastInsertedAlias = sprIt->second.aliases.insert(splIt->first).first;
                        }
                        sprIt->second.slines.insert(functionTmplt.exprstmt.ln);                            
                        if(sprIt->second.isAlias){
                            //std::cerr<<*rVecIt<<std::endl;
                            auto spaIt = FunctionIt->second.find(*sprIt->second.lastInsertedAlias);
                            if(spaIt != FunctionIt->second.end()){
                                spaIt->second.dvars.insert(splIt->first);
                                spaIt->second.slines.insert(functionTmplt.exprstmt.ln);
                            }
                        }
                        if(splIt != FunctionIt->second.end() && splIt->second.potentialAlias){
                           
                        }
                    }
                }
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
                }*/