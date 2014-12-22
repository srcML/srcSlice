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
    
    
    
    SliceProfile ArgumentProfile(unsigned int, unsigned int);

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
        //fileNumber = functionNameHash(attributes[1].value);
        FileIt = sysDict.dictionary.insert(std::make_pair(std::string(attributes[1].value), FunctionVarMap())).first; //insert and keep track of most recent.         
        //std::cerr<<"val: "<<attributes[1].value<<std::endl;exit(1);
        unsigned int ghash = functionNameHash("GLOBAL");
        sysDict.functionTable.insert(std::make_pair(functionNameHash("GLOBAL"), "GLOBAL"));
        FunctionIt = FileIt->second.insert(std::make_pair(ghash, VarMap())).first; //for globals. Makes a bad assumption about where globals are. Fix.
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
        static std::unordered_map<std::string, std::function<void()>> process_map = {
            {"decl_stmt", [this](){
                currentDeclStmt.first.clear();
                ++declIndex; //to keep track of index of declarations
                ++triggerField[decl_stmt];
            } }, 

            { "expr_stmt", [this](){
                currentExprStmt.first.clear();
                ++triggerField[expr_stmt];
            } },

            { "parameter_list", [this](){
                ++triggerField[parameter_list];
            } },

            { "if", [this](){
                ++triggerField[ifcond];
                controlFlowLineNum.push(lineNum);
            } },

            { "for", [this](){
                ++triggerField[forloop];
                controlFlowLineNum.push(lineNum);
            } },

            { "while", [this](){
                ++triggerField[whileloop];
                controlFlowLineNum.push(lineNum);
            } },

            { "argument_list", [this](){
                ++triggerField[argument_list];
                if(triggerField[call]){
                    if(isACallName){
                        isACallName = false;
                        nameOfCurrentClldFcn.push(calledFunctionName);
                        calledFunctionName.clear();
                    }
                }
            } },

            { "call", [this](){
                if(triggerField[call]){//for nested calls
                    --numArgs; //already in some sort of a call. Decrement counter to make up for the argument slot the function call took up.
                }
                isACallName = true;
                ++triggerField[call];
            } },

            { "function", [this](){
                inGlobalScope = false;
                currentFunctionBody.functionName.clear();
                ++triggerField[function];
            } },

            { "constructor", [this](){
                ++constructorNum;//constructors have numbers appended to them since they all have the same name.
                
                isConstructor = true;
                inGlobalScope = false;

                ++triggerField[function];
            } },

            { "destructor", [this](){
                inGlobalScope = false;
                currentFunctionBody.functionName.clear();
                ++triggerField[function];
            } },
        };
        
        std::unordered_map<std::string, std::function<void()>>::const_iterator process = process_map.find(lname);
        if (process != process_map.end()) {
//            fprintf(stderr, "DEBUG:  %s %s %d\n", __FILE__,  __FUNCTION__, __LINE__);

            process->second();
        }
        
        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] || 
            triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            
            static const std::unordered_map< std::string,  std::function<void()>> process_map2 = {
    
                { "param", [this](){
                    ++triggerField[param];
                    ++declIndex;
                } },
    
                { "member_list", [this](){
                    ++triggerField[member_list];
                } },
    
                { "class", [this](){
                    ++triggerField[classn];
                } },
    
                { "index", [this](){
                    ++triggerField[index];
                } },
    
                { "operator", [this](){
                    ++triggerField[op];
                    //Don't want the operators. But do make a caveat for ->
                    if(triggerField[call]){
                        currentCallArgData.first.clear();
                    }
                    if(triggerField[expr_stmt]){
                        currentExprStmt.first.clear(); 
                    }
                    if(triggerField[decl_stmt]){
                        currentDeclStmt.first.clear();
                    }
                } },
    
                { "block", [this]()
                { //So we can discriminate against things in or outside of blocks
                        //currentFunctionBody.functionName.clear();
                    ++triggerField[block];
                } },
    
                { "init", [this](){//so that we can get more stuff after the decl's name 
                    currentDeclStmt.first.clear();
                    ++triggerField[init];
                } },
    
                { "argument", [this](){
                    ++numArgs;
                    currentCallArgData.first.clear();
                    calledFunctionName.clear();
                    ++triggerField[argument];
                } },
    
                { "literal", [this](){
                    ++triggerField[literal];
                } },
    
                { "modifier", [this](){
                    if(triggerField[decl_stmt]){
                        currentDeclStmt.first.clear();
                    }
                    ++triggerField[modifier];
                } },
    
                { "decl", [this](){
                    ++triggerField[decl]; 
                } },
    
                { "type", [this](){
                    ++triggerField[type]; 
                } },
    
                { "expr", [this](){
                    ++triggerField[expr];
                } },
    
                { "name", [this](){
                    ++triggerField[name];
                    currentCallArgData.second = currentParam.second = 
                    currentFunctionBody.functionLineNumber = currentDeclStmt.second =  
                    currentExprStmt.second = lineNum;
                } },
    
                };   
                std::unordered_map<std::string, std::function<void()>>::const_iterator process2= process_map2.find(lname);
                if (process2 != process_map2.end()) {
        //            fprintf(stderr, "DEBUG:  %s %s %d\n", __FILE__,  __FUNCTION__, __LINE__);
        
                    process2->second();
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
        static std::unordered_map<std::string, std::function<void()>> process_map3 = {

            {"decl_stmt", [this](){
                currentCallArgData.first.clear();
                currentDeclArg.first.clear();
                currentDeclStmt.first.clear();
                potentialAlias = false;
                --triggerField[decl_stmt];
            } }, 

            { "expr_stmt", [this](){
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
            } },

            { "parameter_list", [this](){
                --triggerField[parameter_list];
            } },

            { "if", [this](){
                sysDict.controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
                controlFlowLineNum.pop();
                --triggerField[ifcond];
            } },

            { "for", [this](){
                sysDict.controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
                controlFlowLineNum.pop();
                --triggerField[forloop];
            } },

            { "while", [this](){
                sysDict.controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
                controlFlowLineNum.pop();
                --triggerField[whileloop];
            } },

            { "argument_list", [this](){
                numArgs = 0;
                calledFunctionName.clear();
                --triggerField[argument_list];
            } },

            { "call", [this](){
                if(!nameOfCurrentClldFcn.empty()){
                    nameOfCurrentClldFcn.pop();
                }
                --triggerField[call];
                if(triggerField[call]){
                    ++numArgs; //we exited a call but we're still in another call. Increment to make up for decrementing when we entered the second call.
                }
            } },

            { "function", [this](){
                //std::cerr<<functionTmplt.functionName<<std::endl;
                declIndex = 0;

                inGlobalScope = true;
                functionTmplt.clear();
                --triggerField[function];
            } },

            { "constructor", [this](){
                isConstructor = false;
                declIndex = 0;

                inGlobalScope = true;
                functionTmplt.clear();
                --triggerField[function];
            } },

            { "destructor", [this](){
                declIndex = 0;

                inGlobalScope = true;
                functionTmplt.clear();
                --triggerField[function];
            } },
        };
        
        std::unordered_map<std::string, std::function<void()>>::const_iterator process3 = process_map3.find(lname);
        if (process3 != process_map3.end()) {
//            fprintf(stderr, "DEBUG:  %s %s %d\n", __FILE__,  __FUNCTION__, __LINE__);

            process3->second();
        }
   

        if(triggerField[decl_stmt] || triggerField[function] || triggerField[expr_stmt] 
            || triggerField[parameter_list] || triggerField[argument_list] || triggerField[call]){
            
            static const std::unordered_map< std::string,  std::function<void()>> process_map4 = {
                { "param", [this](){
                    currentParam.first.clear();
                    potentialAlias = false;
                    --triggerField[param];
                } },
    
                { "member_list", [this](){
                    --triggerField[member_list];
                } },
    
                { "class", [this](){
                    --triggerField[classn];
                } },
    
                { "index", [this](){
                    --triggerField[index];
                } },
    
                { "operator", [this](){
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
                    if(triggerField[decl_stmt] && triggerField[decl]){
                        currentDeclStmt.first.clear();
                    }
                    if(currentDeclStmt.first == "new"){
                        currentDeclStmt.first.append("-"); //separate new operator because we kinda need to know when we see it.
                    }
                    --triggerField[op];
                } },
    
                { "block", [this]()
                { //So we can discriminate against things in or outside of blocks
                        //currentFunctionBody.functionName.clear();
                    --triggerField[block];
                } },
    
                { "init", [this](){//so that we can get more stuff after the decl's name 
                    --triggerField[init];
                } },
    
                { "argument", [this](){
                    currentDeclArg.first.clear(); //get rid of the name of the var that came before it for ctor calls like: object(InitVarable)
                    currentCallArgData.first.clear();
                    calledFunctionName.clear();
                    --triggerField[argument];
                } },
    
                { "literal", [this](){
                    --triggerField[literal];
                } },
    
                { "modifier", [this](){
                    if(triggerField[decl_stmt] && triggerField[decl]){ //only care about modifiers in decls
                        potentialAlias = true;
                    }else if(triggerField[function] && triggerField[parameter_list] && triggerField[param] && triggerField[decl]){
                        potentialAlias = true;
                    }
                    --triggerField[modifier];
                } },
    
                { "decl", [this](){
                    --triggerField[decl]; 
                } },
    
                { "type", [this](){
                    if(triggerField[parameter_list] && triggerField[param]){
                        currentParam.first.clear();
                    }
                    if(triggerField[decl_stmt] || (triggerField[function] && ! triggerField[block])) {
                        currentDeclStmt.first.clear();
                    }
                    --triggerField[type];
                } },
    
                { "expr", [this](){
                    --triggerField[expr];
                } },
    
                { "name", [this](){
                    if(triggerField[function] && (!triggerField[block] || triggerField[type] || triggerField[parameter_list])){
                        sysDict.functionTable.insert(std::make_pair(functionNameHash(functionTmplt.functionName), functionTmplt.functionName));
                        FunctionIt = FileIt->second.insert(std::make_pair(functionNameHash(functionTmplt.functionName), VarMap())).first;
                    }
                    if(triggerField[decl_stmt] && triggerField[decl] && triggerField[argument_list] && triggerField[argument] && triggerField[expr] &&
                            !triggerField[type]){ //For the case where we need to get a constructor decl
                        ProcessConstructorDecl();
                        currentDeclArg.first.clear();
                    }
                    if(triggerField[call] && triggerField[argument]){
                        callArgData.push(currentCallArgData);
                    }
                    //Get Function names and arguments
                    if(triggerField[function]){
                        GetFunctionData();
                    }
                    //Get variable decls
                    if(triggerField[decl_stmt]){
                        GetDeclStmtData();
                    }                
                    //Get function arguments
                    if(triggerField[call] || (triggerField[decl_stmt] && triggerField[argument_list])){
                        GetCallData();//issue with statements like object(var)
                        while(!callArgData.empty())
                            callArgData.pop();
                    }
                    if(triggerField[expr_stmt] && triggerField[expr]){
                        ProcessExprStmt();//problems with exprs like blotttom->next = expr
                    }
                    if(triggerField[decl_stmt] && triggerField[decl] && triggerField[init] && 
                    !(triggerField[type] || triggerField[argument_list] || triggerField[call])){
                        ProcessDeclStmt();
                    }
                    --triggerField[name];
                } },
    
                };
                std::unordered_map<std::string, std::function<void()>>::const_iterator process4= process_map4.find(lname);
                if (process4 != process_map4.end()){
        //            fprintf(stderr, "DEBUG:  %s %s %d\n", __FILE__,  __FUNCTION__, __LINE__);        
                    process4->second();
                }else{
                    //--triggerField[nonterminal]; 
                }
            }
        }
#pragma GCC diagnostic pop

};

#endif
