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

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stack>

class srcSliceHandler : public srcSAXHandler {
private:
    /*ParserState is a set of enums corresponding to srcML tags. Primarily, they're for addressing into the 
     *triggerField vector and figuring out which tags have been seen. It keeps a count of how many of each
     *tag is currently open. Increments at a start tag and decrements at an end tag*/
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, 
        argument_list, call, ctrlflow, endflow, name, function, 
        argument, index, block, type, init, op, 
        literal, modifier, member_list, classn, preproc,
        whileloop, forloop, ifcond, nonterminal, empty, 
        MAXENUMVALUE = empty};

    unsigned int fileNumber;
    unsigned int numArgs;
    unsigned int declIndex;

    int constructorNum;

    SliceProfile* lhs;
    std::string lhsName;
    unsigned int lhsLine;

    /*Hashing function/file names. This will accomplish that.*/
    std::hash<std::string> functionNameHash;

    /*Holds data for functions as we parse. Useful for going back to figuring out which function we're in*/
    FunctionData functionTmplt;

    /*keeps track of which functioni has been called. Useful for when argument slice profiles need to be updated*/
    std::stack<std::string> nameOfCurrentClldFcn;
    std::stack<unsigned int> controlFlowLineNum;
    /*These two iterators keep track of where we are inside of the system dictionary. They're primarily so that
     *there's no need to do any nasty map.finds on the dictionary (since it's a nested map of maps). These must
     *be updated as the file is parsed*/
    FileFunctionVarMap::iterator FileIt;
    FunctionVarMap::iterator FunctionIt;
    VarMap::iterator varIt;

    /*Their names basically say what they do. Nested calls are an issue so the first one is a flag to figure out
     *if I need to do special processing on a name in a call. Might be a better way to do it. Investigate later. 
     *The second bool checks to see if the function that was just seen is a constructor. Do this because
     *functions and constructors are treated basically the same (and I mark them as the same thing in triggerfield)
     *but constructors need to be counted so a number can be appended to them to differentiate*/
    bool isConstructor;

    /*bool to tell us when we're not in a function*/
    bool inGlobalScope;
    bool isACallName;
    bool opassign;
    bool skipMember;

    bool dirtyAlias;
    bool potentialAlias;

    bool dereferenced;
    /*These along with triggerfield make up the meat of this slicer.Check the triggerfield for context (E.g., triggerField[init])
     *and then once you know the right tags are open, check the correct line/string pair to see what the name is
     *at that position and its line number to be stored in the slice profile*/ 
    std::vector<unsigned short int> triggerField;
    std::string calledFunctionName;
    std::stack<NameLineNumberPair> callArgData;
    NameLineNumberPair currentCallArgData;
    NameLineNumberPair currentParam;
    FunctionData currentFunctionBody;
    NameLineNumberPair currentDeclStmt;
    NameLineNumberPair currentExprStmt;
    NameLineNumberPair currentDeclArg;
    /*function headers*/
    void GetCallData();
    void ProcessDeclStmt();
    void GetFunctionData();
    void GetDeclStmtData();
    void ProcessExprStmt();
    void ProcessConstructorDecl();
    SliceProfile* Find(const std::string&);
    
    
    
    SliceProfile ArgumentProfile(std::string, unsigned int);

public:
    void ComputeInterprocedural(const std::string&);
    SystemDictionary sysDict;
    unsigned int lineNum;
    srcSliceHandler(){
        fileNumber = 0;
        numArgs = 0;
        declIndex = 0;

        constructorNum = 0;
        lineNum = 0;
        
        lhs = nullptr;
        skipMember = false;

        dereferenced = false;
        opassign = false;
        dirtyAlias = false;
        isACallName = false;
        isConstructor = false;
        inGlobalScope = true;
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
        fileNumber = functionNameHash(attributes[1].value);
        FileIt = sysDict.dictionary.insert(std::make_pair(attributes[1].value, FunctionVarMap())).first; //insert and keep track of most recent.         
        //std::cerr<<"val: "<<attributes[1].value<<std::endl;exit(1);
        FunctionIt = FileIt->second.insert(std::make_pair(std::string("GLOBALS"), VarMap())).first; //for globals. Makes a bad assumption about where globals are. Fix.
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
          
        
        if(num_attributes){
            lineNum = strtoul(attributes[0].value, NULL, 0);
        }
        std::string lname(localname);
        std::string lnspace;
        if(prefix){
            lnspace.append(prefix);
        }
        if(lnspace == "cpp"){
            ++triggerField[preproc];
        }
        if(lname == "decl_stmt"){
            ++declIndex; //to keep track of index of declarations
            ++triggerField[decl_stmt];
        }else if(lname == "function" || lname == "constructor" || lname == "destructor"){
            if(lname == "constructor"){
                ++constructorNum;//constructors have numbers appended to them since they all have the same name.
                isConstructor = true;
            }
            inGlobalScope = false;
            currentFunctionBody.functionName.clear();
            ++triggerField[function];
        }else if (lname == "expr_stmt"){
            currentExprStmt.first.clear();
            ++triggerField[expr_stmt];
        }else if (lname == "parameter_list"){
            ++triggerField[parameter_list];
        }else if (lname == "argument_list"){
            ++triggerField[argument_list];
            if(triggerField[call]){
                if(isACallName){
                    isACallName = false;
                    nameOfCurrentClldFcn.push(calledFunctionName);
                    calledFunctionName.clear();
                }
            }
        }else if (lname == "call"){
            if(triggerField[call]){//for nested calls
                --numArgs; //already in some sort of a call. Decrement counter to make up for the argument slot the function call took up.
            }
            isACallName = true;
            ++triggerField[call];
        }else if (lname == "while"){
            ++triggerField[whileloop];
            controlFlowLineNum.push(lineNum);
        }else if (lname == "for"){
            ++triggerField[forloop];
            controlFlowLineNum.push(lineNum);
        }else if (lname == "if"){
            ++triggerField[ifcond];
            controlFlowLineNum.push(lineNum);
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] || 
            triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (lname == "param"){
                    ++triggerField[param];
                    ++declIndex;
            }else if(lname == "member_list"){
                    ++triggerField[member_list];
            }else if(lname == "class"){
                    ++triggerField[classn];
            }else if(lname == "index"){
                    ++triggerField[index];
            }else if(lname == "operator"){
                    ++triggerField[op];
                    if(triggerField[call]){
                        currentCallArgData.first.clear(); //Don't want the operators. But do make a caveat for ->
                    }
                    if(triggerField[expr_stmt]){

                        currentExprStmt.first.clear(); //Don't want the operators. But do make a caveat for ->
                    }
            }else if (lname == "block"){ //So we can discriminate against things in or outside of blocks
                    //currentFunctionBody.functionName.clear();
                    ++triggerField[block];
            }else if (lname == "init"){//so that we can get more stuff after the decl's name 
                    currentDeclStmt.first.clear();
                    ++triggerField[init];
            }else if (lname == "argument"){
                    ++numArgs;
                    currentCallArgData.first.clear();
                    calledFunctionName.clear();
                    //currentCallArgData.first.append(":"); //denote arguments so that I can parse them out later.
                    //currentDeclStmt.first.append(":"); //same as above.
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
                currentCallArgData.second = currentParam.second = 
                currentFunctionBody.functionLineNumber = currentDeclStmt.second =  
                currentExprStmt.second = lineNum;
            }else{
                ++triggerField[nonterminal]; 
            }   
        }
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

        if((triggerField[decl_stmt] || triggerField[expr_stmt]) && triggerField[call] && 
            (triggerField[name]) && triggerField[argument_list] &&
            !(triggerField[index] || triggerField[op] || triggerField[preproc])){
            currentCallArgData.first.append(ch, len);
        }
        if((triggerField[function] && triggerField[name]) && !(triggerField[block] || triggerField[argument_list] 
            || triggerField[type] || triggerField[parameter_list] || triggerField[index] || triggerField[preproc])){
            currentFunctionBody.functionName.append(ch, len);
        }
        if((triggerField[function] && triggerField[name]  && triggerField[parameter_list] && triggerField[param])){
            currentParam.first.append(ch, len);            
        }
        if(triggerField[decl_stmt] && (triggerField[name] || triggerField[op]) && triggerField[decl] && !(triggerField[index] || triggerField[preproc])) {
            currentDeclStmt.first.append(ch, len);
            
        }else if(triggerField[expr_stmt] && (triggerField[name] || triggerField[op]) && triggerField[expr] && !(triggerField[index] || triggerField[preproc])){
            currentExprStmt.first.append(ch, len);
        }
        if(triggerField[call]){
            calledFunctionName.append(ch, len);
        }
        if(triggerField[decl_stmt] && triggerField[decl] && triggerField[argument_list] && triggerField[argument] && triggerField[expr] &&
            !triggerField[type]){
            currentDeclArg.first.append(ch,len);
        }
    }

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {

    }
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
        std::string lname(localname);
        std::string lnspace;
        if(prefix){
            lnspace.append(prefix);
        }
        if(lnspace == "cpp"){
            --triggerField[preproc];
        }

        if(lname == "decl_stmt"){
            currentCallArgData.first.clear();
            currentDeclArg.first.clear();
            currentDeclStmt.first.clear();
            potentialAlias = false;
            --triggerField[decl_stmt];
        }else if (lname == "expr_stmt"){
            --triggerField[expr_stmt];
            if(!opassign && lhs){//Don't know if an lhs is a def or use until I see '='. If I don't see it (expr_stmt closes before I see it) then it's definitely use.
                lhs->slines.insert(lhsLine);
                lhs->use.insert(lhsLine);
            }
            lhs = nullptr;
            opassign = false;
            dereferenced = false;
            lhsLine = 0;
            lhsName.clear();
            currentCallArgData.first.clear();
            currentExprStmt.first.clear();
            
        }else if(lname == "function" || lname == "constructor" || lname == "destructor"){
            //std::cerr<<functionTmplt.functionName<<std::endl;
            
            dirtyAlias = false;
            declIndex = 0;
            if(lname == "constructor"){
                isConstructor = false;
            }
            inGlobalScope = true;
            functionTmplt.clear();
            --triggerField[function];
        }else if (lname == "parameter_list"){
            --triggerField[parameter_list];
        }else if (lname == "argument_list"){
            numArgs = 0;
            calledFunctionName.clear();
            --triggerField[argument_list];
        }else if (lname == "call"){
            if(!nameOfCurrentClldFcn.empty()){
                nameOfCurrentClldFcn.pop();
            }
            --triggerField[call];
            if(triggerField[call]){
                ++numArgs; //we exited a call but we're still in another call. Increment to make up for decrementing when we entered the second call.
            }
        }else if (lname == "while"){
            sysDict.controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
            controlFlowLineNum.pop();
            --triggerField[whileloop];
        }else if (lname == "for"){
            sysDict.controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
            controlFlowLineNum.pop();
            --triggerField[forloop];
        }else if (lname == "if"){
            sysDict.controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
            controlFlowLineNum.pop();
            --triggerField[ifcond];
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] 
            || triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (lname == "param"){
                currentParam.first.clear();
                potentialAlias = false;
                --triggerField[param];
            }else if (lname == "literal"){
                --triggerField[literal];
            }else if(lname == "class"){
                    --triggerField[classn];
            }else if(lname == "member_list"){
                    --triggerField[member_list];
            }else if(lname == "index"){
                    --triggerField[index];
            }else if (lname == "modifier"){
                if(triggerField[decl_stmt] && triggerField[decl]){ //only care about modifiers in decls
                    potentialAlias = true;
                }else if(triggerField[function] && triggerField[parameter_list] && triggerField[param] && triggerField[decl]){
                    potentialAlias = true;
                }
                --triggerField[modifier];
            }else if (lname == "argument"){
                currentDeclArg.first.clear(); //get rid of the name of the var that came before it for ctor calls like: object(InitVarable)
                currentCallArgData.first.clear();
                calledFunctionName.clear();
                --triggerField[argument];
            }else if (lname == "block"){
                --triggerField[block];
            }else if(lname == "decl"){
                --triggerField[decl];                 
            }else if(lname == "init"){
                --triggerField[init];                 
            }else if (lname == "expr"){
                if(triggerField[decl_stmt] && triggerField[decl] && triggerField[init] && 
                !(triggerField[type] || triggerField[argument_list] || triggerField[call])){
                    ProcessDeclStmt();
                }
                --triggerField[expr]; 
            }else if (lname == "type"){
                if(triggerField[parameter_list] && triggerField[param]){
                    currentParam.first.clear();
                }
                if(triggerField[decl_stmt] || (triggerField[function] && ! triggerField[block])) {
                    currentDeclStmt.first.clear();
                }
                --triggerField[type]; 
            }else if (lname == "operator"){
                if(triggerField[expr_stmt] && triggerField[expr]){
                    if(currentExprStmt.first == "="){
                        opassign = true;
                        if(lhs){//Don't know if an lhs is a def or use until I see '='. Once '=' is found, it's definitely a def. Otherwise, it's a use (taken care of at end tag of expr_stmt).
                            lhs->slines.insert(lhsLine);
                            lhs->def.insert(lhsLine);
                        }
                    }
                    if(currentExprStmt.first == "*"){
                        dereferenced = true;
                    }
                    if(currentExprStmt.first == "->"){
                        skipMember = true;
                    }
                    if(currentExprStmt.first == "."){
                        skipMember = true;
                    }
                    currentExprStmt.first.clear();
                }

                if(currentDeclStmt.first == "new"){
                    currentDeclStmt.first.append("-"); //separate new operator because we kinda need to know when we see it.
                }
                --triggerField[op];
            }
            else if (lname == "name"){
                if(triggerField[function] && (!triggerField[block] || triggerField[type] || triggerField[parameter_list])){
                    FunctionIt = FileIt->second.insert(std::make_pair(functionTmplt.functionName, VarMap())).first;
                }
                if(triggerField[decl_stmt] && triggerField[decl] && triggerField[argument_list] && triggerField[argument] && triggerField[expr] &&
                        !triggerField[type]){ //For the case where we need to get a constructor decl
                    ProcessConstructorDecl();
                    currentDeclArg.first.clear();
                }
                if(triggerField[call] && triggerField[argument]){
                    //std::cerr<<"Name: "<<currentCallArgData.first<<std::endl;
                    callArgData.push(currentCallArgData);
                }
                //Get Function names and arguments
                if(triggerField[function]){
                    GetFunctionData();
                }
                //Get variable decls
                if(triggerField[decl_stmt]){
                    GetDeclStmtData();
                    //currentDeclStmt.first.clear();
                }                
                //Get function arguments
                if(triggerField[call] || (triggerField[decl_stmt] && triggerField[argument_list])){
                    GetCallData();//issue with statements like object(var)
                    while(!callArgData.empty())
                        callArgData.pop();
                }
                if(triggerField[expr_stmt] && triggerField[expr]){
                    ProcessExprStmt();//problems with exprs like blotttom->next = expr
                    //std::cerr<<"Thing: "<<currentExprStmt.first<<std::endl;
                }
                --triggerField[name];

            }
        }
    }
#pragma GCC diagnostic pop

};

#endif
