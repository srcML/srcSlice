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
private :

    void ProcessConstructorDecl();
    void ProcessDeclStmt();
    void GetCallData();
    void GetFunctionData();
    void GetDeclStmtData();
    void ProcessExprStmt();
    SliceProfile* Find(const std::string&, const std::string&);
    /*ParserState is a set of enums corresponding to srcML tags. Primarily, they're for addressing into the 
     *triggerField vector and figuring out which tags have been seen. It keeps a count of how many of each
     *tag is currently open. Increments at a start tag and decrements at an end tag*/
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, 
        argument_list, call, ctrlflow, endflow, name, function, 
        argument, index, block, type, init, op, 
        literal, modifier, member_list, classn, preproc,
        nonterminal, empty, MAXENUMVALUE = empty};

    unsigned int fileNumber;
    unsigned int numArgs;
    
    int constructorNum;
    
    /*Hashing function/file names. This will accomplish that.*/
    std::hash<std::string> functionAndFileNameHash;

    /*Holds data for functions as we parse. Useful for going back to figuring out which function we're in*/
    FunctionData functionTmplt;

    /*keeps track of which functioni has been called. Useful for when argument slice profiles need to be updated*/
    std::stack<std::string> nameOfCurrentClldFcn;

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

    bool potentialAlias;

    /*These along with triggerfield make up the meat of this slicer.Check the triggerfield for context (E.g., triggerField[init])
     *and then once you know the right tags are open, check the correct line/string pair to see what the name is
     *at that position and its line number to be stored in the slice profile*/ 
    std::vector<unsigned short int> triggerField;
    std::string calledFunctionName;
    NameLineNumberPair currentCallArgData;
    NameLineNumberPair currentParam;
    FunctionData currentFunctionBody;
    NameLineNumberPair currentDeclStmt;
    NameLineNumberPair currentExprStmt;
public:
    SystemDictionary sysDict;
    srcSliceHandler(){
        fileNumber = 0;
        numArgs = 0;
        constructorNum = 0;

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
        fileNumber = functionAndFileNameHash(attributes[1].value);
        sysDict.fileTable.insert(std::make_pair(fileNumber, attributes[1].value));
        FileIt = sysDict.dictionary.insert(std::make_pair(fileNumber, FunctionVarMap())).first; //insert and keep track of most recent.         
        FunctionIt = FileIt->second.insert(std::make_pair(functionAndFileNameHash(std::string(attributes[1].value).append("0")), VarMap())).first; //for globals. Makes a bad assumption about where globals are. Fix.
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
        std::string lnspace;
        if(prefix){
            lnspace.append(prefix);
        }
        if(lnspace == "cpp"){
            ++triggerField[preproc];
        }

        if(lname == "decl_stmt"){
            currentCallArgData.first.clear();
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
            currentCallArgData.first.clear();
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
        }
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] || 
            triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            if (lname == "param"){
                    ++triggerField[param];
            }else if(lname == "member_list"){
                    ++triggerField[member_list];
            }else if(lname == "class"){
                    ++triggerField[classn];
            }else if(lname == "index"){
                    ++triggerField[index];
            }else if(lname == "operator"){
                    ++triggerField[op];
            }else if (lname == "block"){ //So we can discriminate against things in or outside of blocks
                    //currentFunctionBody.functionName.clear();
                    ++triggerField[block];
            }else if (lname == "init"){//so that we can get more stuff after the decl's name 
                    currentDeclStmt.first.clear();
                    ++triggerField[init];
            }else if (lname == "argument"){
                    ++numArgs;
                    calledFunctionName.clear();
                    currentCallArgData.first.append(":"); //denote arguments so that I can parse them out later.
                    currentDeclStmt.first.append(":"); //same as above.
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

        if((triggerField[decl_stmt] || triggerField[expr_stmt]) && (triggerField[name] || triggerField[op]) && !(triggerField[index] || triggerField[preproc])){
            currentCallArgData.first.append(ch, len);
            //std::cerr<<currentCallArgData.first<<std::endl;
        }
        if((triggerField[function] && triggerField[name]) && !(triggerField[block] || triggerField[argument_list] 
            || triggerField[type] || triggerField[parameter_list] || triggerField[index] || triggerField[preproc])){
            currentFunctionBody.functionName.append(ch, len);
            //std::cerr<<"This is content:"<<currentFunctionBody.functionName<<std::endl;
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
            currentDeclStmt.first.clear();
            potentialAlias = false;
            --triggerField[decl_stmt];
        }else if (lname == "expr_stmt"){
            ProcessExprStmt();
            --triggerField[expr_stmt];
            currentCallArgData.first.clear();
            currentExprStmt.first.clear();
            
        }else if(lname == "function" || lname == "constructor" || lname == "destructor"){
            sysDict.functionTable.insert(std::make_pair(functionTmplt.functionNumber, functionTmplt));
            FunctionIt = FileIt->second.insert(std::make_pair(functionTmplt.functionNumber, VarMap())).first;
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
                }else if(triggerField[decl_stmt] && triggerField[decl] && triggerField[argument_list] && triggerField[argument] && triggerField[expr] &&
                        !triggerField[type]){ //For the case where we need to get a constructor decl
                    ProcessConstructorDecl();
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
                //Get function arguments
                if(triggerField[call]){
                    GetCallData();
                }
                --triggerField[name];

            }
        }
    }
#pragma GCC diagnostic pop

};

#endif
