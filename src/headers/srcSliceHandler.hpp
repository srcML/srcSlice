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
#include <Utility.hpp>
#include <srcSlice.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <stack>
class srcSliceHandler : public srcSAXHandler {
private:
    /*ParserState is a set of enums corresponding to srcML tags. Primarily, they're for addressing into the 
     *triggerField vector and figuring out which tags have been seen. It keeps a count of how many of each
     *tag is currently open. Increments at a start tag and decrements at an end tag*/
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, 
        argument_list, argument_list_template, call, templates, ctrlflow, endflow, 
        name, function, functiondecl, constructor, constructordecl, destructordecl, destructor,
        argument, index, block, type, init, op, literal, modifier, member_list, classn,
        preproc, whileloop, forloop, ifcond, nonterminal, macro, classblock, functionblock,
        specifier, empty, MAXENUMVALUE = empty};

    std::unordered_map<std::string, std::function<void()>> process_map;
    std::unordered_map<std::string, std::function<void()>> process_map3;

    std::string fileName;
    unsigned int numArgs;
    unsigned int declIndex;

    int constructorNum;

    SliceProfile currentSliceProfile;

    std::string lhsName;
    unsigned int lhsLine;

    /*Hashing param types*/
    std::hash<std::string> paramTypeHash;

    /*Holds data for functions as we parse. Useful for going back to figuring out which function we're in*/
    FunctionData functionTmplt;

    /*keeps track of which functioni has been called. Useful for when argument slice profiles need to be updated*/
    std::stack<std::string> nameOfCurrentClldFcn;
    std::stack<unsigned int> controlFlowLineNum;
    
    /*These two iterators keep track of where we are inside of the system dictionary. They're primarily so that
     *there's no need to do any nasty map.finds on the dictionary (since it's a nested map of maps). These must
     *be updated as the file is parsed*/
    std::unordered_map<std::string, ClassProfile>::iterator classIt;
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

    //for decl_stmts
    bool opassign;
    bool sawnew;
    bool sawinit;
    bool sawgeneric;

    //for expr_stmts    
    bool exprassign;
    bool exprop;
    bool foundexprlhs;

    std::list<NameLineNumberPair> useExprStack;

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
    NameLineNumberPair currentParamType;
    
    NameLineNumberPair currentFunctionReturnType;

    NameLineNumberPair currentDecl;
    NameLineNumberPair currentDeclType;
    NameLineNumberPair currentDeclInit;
    NameLineNumberPair currentDeclCtor;

    NameLineNumberPair currentExprStmt;
    NameLineNumberPair lhsExprStmt;

    NameLineNumberPair useExprStmt;

    NameLineNumberPair currentDeclArg;
    NameLineNumberPair currentClassName;
    
    NameLineNumberPair currentFunctionBody;
    NameLineNumberPair currentFunctionDecl;
    /*function headers*/
    void GetCallData();
    void GetParamName();
    void GetParamType();
    void ProcessDeclStmt();
    void GetFunctionData();
    void GetDeclStmtData();
    void ProcessDeclCtor();
    
    void ProcessExprStmtPreAssign();
    void ProcessExprStmtPostAssign();
    void ProcessExprStmtNoAssign();

    void ProcessConstructorDecl();
    void GetFunctionDeclData();

    SliceProfile ArgumentProfile(std::string, unsigned int, VarMap::iterator);
    SliceProfile* Find(const std::string& varName);
public:
    void ComputeInterprocedural(const std::string&);
    SliceDictionary* sysDict;
    unsigned int lineNum;
    srcSliceHandler(SliceDictionary* dict){
        sysDict = dict;
        numArgs = 0;
        declIndex = 0;

        constructorNum = 0;
        lineNum = 0;
        potentialAlias = false;

        dereferenced = false;
        opassign = false;
        sawnew = false;
        sawinit = false;
        sawgeneric = false;

        exprassign = false;
        exprop = false;
        foundexprlhs = false;

        isACallName = false;
        isConstructor = false;
        inGlobalScope = true;
        triggerField = std::vector<unsigned short int>(MAXENUMVALUE, 0);
        process_map = {
            {"decl_stmt", [this](){
                ++declIndex; //to keep track of index of declarations
                ++triggerField[decl_stmt];
            } }, 

            { "expr_stmt", [this](){
                ++triggerField[expr_stmt];
            } },
            { "parameter_list", [this](){
                if((triggerField[function] || triggerField[functiondecl] || triggerField[constructor]) && !(triggerField[functionblock] || triggerField[parameter_list])){
                    GetFunctionData();
                }
                ++triggerField[parameter_list];
                if(triggerField[function] && (!triggerField[functionblock] || triggerField[type] || triggerField[parameter_list])){
                    FunctionIt = FileIt->second.insert(std::make_pair(functionTmplt.functionName, VarMap())).first;
                }
                if(triggerField[constructordecl]){ //For the case where we need to get a constructor decl
                    ProcessConstructorDecl();
                    currentDeclArg.first.clear();
                }
                if(triggerField[functiondecl]){
                    functionTmplt.functionLineNumber = currentFunctionDecl.second;
                    functionTmplt.functionName = currentFunctionDecl.first;
                    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type]){
                        GetFunctionDeclData();
                    }
                }
            } },

            { "if", [this](){
                ++triggerField[ifcond];
                //controlFlowLineNum.push(lineNum);
            } },

            { "for", [this](){
                ++triggerField[forloop];
                //controlFlowLineNum.push(lineNum);
            } },

            { "while", [this](){
                ++triggerField[whileloop];
                //controlFlowLineNum.push(lineNum);
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
                ++triggerField[function];
            } },
            { "constructor", [this](){
                ++constructorNum;//constructors have numbers appended to them since they all have the same name.
                
                isConstructor = true;
                inGlobalScope = false;

                ++triggerField[function];
            } },
            { "function_decl", [this](){
                currentFunctionDecl.first.clear();
                ++triggerField[functiondecl];
            } },
            { "destructor_decl", [this](){
                currentFunctionDecl.first.clear();
                ++triggerField[destructordecl];
            } },
            { "constructor_decl", [this](){
                currentFunctionDecl.first.clear();
                ++triggerField[constructordecl];
            } },
            { "template", [this](){
                ++triggerField[templates];
            } },
            { "class", [this](){
                ++triggerField[classn];
            } },

            { "destructor", [this](){
                inGlobalScope = false;
                ++triggerField[function];
            } },
            { "parameter", [this](){
                    ++triggerField[param];
                    ++declIndex;
            } },    
            { "member_list", [this](){
                ++triggerField[member_list];
            } },
            { "index", [this](){
                ++triggerField[index];
            } },    
            { "operator", [this](){
                ++triggerField[op];
                if(triggerField[expr_stmt]){
                    exprop = true; //assume we're not seeing =
                }
                //Don't want the operators. But do make a caveat for ->
                if(triggerField[call]){
                    currentCallArgData.first.clear();
                }

            } },    
            { "block", [this](){     
                if((triggerField[function] || triggerField[constructor])){
                    ++triggerField[functionblock];
                }
                if(triggerField[classn]){
                    ++triggerField[classblock];
                }
                ++triggerField[block];
            } },
            { "init", [this](){
                //This one is only called if we see init. If there's no init, it's safely ignored.
                if(triggerField[decl_stmt] && (triggerField[constructor] || triggerField[function])){
                    GetDeclStmtData();
                    sawinit = true;
                }
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

                useExprStmt.second = lhsExprStmt.second = currentCallArgData.second = currentParam.second = currentParamType.second = 
                currentFunctionBody.second = currentDecl.second =  
                currentExprStmt.second = currentFunctionDecl.second = currentDeclInit.second = lineNum;
            } },
            { "macro", [this](){
                ++triggerField[macro];
            } },
            { "specifier", [this](){
                ++triggerField[specifier];
            } }
        };
        process_map3 = {

            {"decl_stmt", [this](){
                currentCallArgData.first.clear();
                currentDeclArg.first.clear();
                currentDeclCtor.first.clear();
                potentialAlias = false;
                sawinit = false;
                --triggerField[decl_stmt];
            } }, 

            { "expr_stmt", [this](){
                --triggerField[expr_stmt];
                
                //for decl_stmts
                opassign = false;
                dereferenced = false;
                
                //for expr_stmts
                foundexprlhs = false;
                exprop = false;
                exprassign = false;

                ProcessExprStmtNoAssign(); //collect data about things that were not in assignment expr_stmts
                useExprStack.clear(); //clear data

                //uncategorized
                lhsLine = 0;
                lhsName.clear();
                currentExprStmt.first.clear();
                lhsExprStmt.first.clear();

                useExprStmt.first.clear();

                currentCallArgData.first.clear();
            } },

            { "parameter_list", [this](){
                --triggerField[parameter_list];
            } },

            { "if", [this](){
                //sysDict->controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
                //controlFlowLineNum.pop();
                --triggerField[ifcond];
            } },

            { "for", [this](){
                //sysDict->controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
                //controlFlowLineNum.pop();
                --triggerField[forloop];
            } },

            { "while", [this](){
                //sysDict->controledges.push_back(std::make_pair(controlFlowLineNum.top()+1, lineNum)); //save line number for beginning and end of control structure
                //controlFlowLineNum.pop();
                --triggerField[whileloop];
            } },

            { "argument_list", [this](){
                numArgs = 0;
                sawgeneric = false;
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
                //std::cerr<<"inserting: "<<functionTmplt.functionName<<std::endl;
                //classIt->second.memberFunctions.insert(functionTmplt);
                
                functionTmplt.clear();
                --triggerField[function];
            } },
            { "constructor", [this](){
                isConstructor = false;
                declIndex = 0;

                //classIt->second.memberFunctions.insert(functionTmplt);
                
                inGlobalScope = true;
                functionTmplt.clear();
                --triggerField[function];
            } },

            { "destructor", [this](){
                declIndex = 0;

                //classIt->second.memberFunctions.insert(functionTmplt);

                inGlobalScope = true;
                functionTmplt.clear();
                --triggerField[function];
            } },
            { "function_decl", [this](){
                if(triggerField[classn]){
                    //std::cerr<<"inserting: "<<functionTmplt.functionName<<std::endl;;
                    //classIt->second.memberFunctions.insert(functionTmplt);
                }
                --triggerField[functiondecl];
            } },
            { "constructor_decl", [this](){
                if(triggerField[classn]){
                    //std::cerr<<"inserting: "<<functionTmplt.functionName<<std::endl;;
                    //classIt->second.memberFunctions.insert(functionTmplt);
                }
                --triggerField[functiondecl];
            } },
            { "destructor_decl", [this](){
                if(triggerField[classn]){
                    //std::cerr<<"inserting: "<<functionTmplt.functionName<<std::endl;;
                    //classIt->second.memberFunctions.insert(functionTmplt);
                }
                --triggerField[functiondecl];
            } },            
            { "class", [this](){
                currentClassName.first.clear();
                //classIt = sysDict->classTable.find("GLOBAL");
                /*
                std::cerr<<"Class mfs: "<<sysDict->classTable.find("CLASSBRO")->second.memberFunctions.size()<<std::endl;
                std::cerr<<"Class mvs: "<<sysDict->classTable.find("CLASSBRO")->second.memberVariables.size()<<std::endl;

                std::cerr<<"Class mfs: "<<sysDict->classTable.find("GLOBAL")->second.memberFunctions.size()<<std::endl;
                std::cerr<<"Class mvs: "<<sysDict->classTable.find("GLOBAL")->second.memberVariables.size()<<std::endl;
                */
                --triggerField[classn];
            } },
            { "parameter", [this](){
                    if(triggerField[parameter_list] && triggerField[param] && !(triggerField[type] || triggerField[functionblock] || triggerField[templates])){
                        GetParamName();
                    }
                    potentialAlias = false;
                    --triggerField[param];
            } },    
            { "member_list", [this](){
                --triggerField[member_list];
            } },    
            { "index", [this](){
                --triggerField[index];
            } },    
            { "operator", [this](){
                if(currentDeclInit.first == "new"){
                     //separate new operator because we kinda need to know when we see it.
                    sawnew = true;
                }
                //The logic for exprassign and op is basically that we want to know when we've hit the left and right hand side of expr stmt
                //expr assign is set when we see =. Everything read up to that point is lhs. exprop is any other operator. When I see that
                //I know that we're probably in a member call chain a->b->c etc. I don't care about b and c, so expr op helps skip those.
                if(triggerField[expr_stmt]){

                    useExprStmt.first.clear();

                    if(exprassign){
                        ProcessExprStmtPreAssign();
                        if(!lhsExprStmt.first.empty()){
                            lhsName = lhsExprStmt.first;
                        }
                        lhsExprStmt.first.clear();
                        exprop = false;
                        //foundexprlhs = true;
                    }
                }
                if(!(sawgeneric || currentDeclCtor.first.empty()) && triggerField[decl_stmt] && triggerField[argument] && (!triggerField[init])){
                    GetDeclStmtData();
                    ProcessDeclCtor(); //-- to process decl_stmts that use constructor syntax
                    sawinit = true;
                    currentDeclCtor.first.clear();
                }
                

                currentDeclInit.first.clear(); //this doesn't need to keep track of operators other than new

                if(triggerField[function] && !(triggerField[functionblock] || triggerField[templates] || triggerField[parameter_list] || triggerField[type] || triggerField[argument_list])){
                    currentFunctionBody.first.clear();
                }
                --triggerField[op];
            } },
            { "block", [this](){ 
                if((triggerField[function] || triggerField[constructor])){
                    --triggerField[functionblock];
                }
                if(triggerField[classn]){
                    --triggerField[classblock];
                }
                --triggerField[block];
            } },
            { "init", [this](){//so that we can get more stuff after the decl's name 
                --triggerField[init];
            } },    
            { "argument", [this](){
                currentDeclArg.first.clear(); //get rid of the name of the var that came before it for ctor calls like: object(InitVarable)
                currentCallArgData.first.clear();
                calledFunctionName.clear();
                if(!(sawgeneric || currentDeclCtor.first.empty()) && triggerField[decl_stmt] && triggerField[argument] && !triggerField[init]){
                    GetDeclStmtData();
                    ProcessDeclCtor(); //-- to process decl_stmts that use constructor syntax
                    sawinit = true;
                    currentDeclCtor.first.clear();
                }
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
            { "template", [this](){
                --triggerField[templates];
            } },    
            { "decl", [this](){
                if(!sawinit && triggerField[decl_stmt] && (triggerField[constructor] || triggerField[function])){
                    //only run if we didn't run it during init
                    GetDeclStmtData();
                }
                currentDecl.first.clear();
                --triggerField[decl]; 
            } },    
            { "type", [this](){
                if((triggerField[type] && triggerField[decl_stmt] && (triggerField[function] || triggerField[constructor]) && !(triggerField[modifier] || triggerField[argument_list]))){
                    //Get the type -- news
                    currentSliceProfile.variableType = currentDeclType.first;
                    currentDeclType.first.clear();
                }
                if(triggerField[type] && triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[functionblock] || triggerField[templates])){
                    GetParamType();
                }
                if(triggerField[function] && triggerField[type] && !(triggerField[functionblock] || triggerField[argument_list] || triggerField[templates] || triggerField[parameter_list])){
                    //get functionr ret type  -- news
                    functionTmplt.returnType = currentFunctionReturnType.first;
                    currentFunctionReturnType.first.clear();
                }
                if(triggerField[functiondecl] && triggerField[type] && !(triggerField[parameter_list] || triggerField[member_list])){
                    //get function decl return type
                    functionTmplt.returnType = currentFunctionDecl.first;
                }
                --triggerField[type];
            } },    
            { "expr", [this](){
                --triggerField[expr];
            } },    
            { "name", [this](){
                if(triggerField[call] && triggerField[argument]){
                    callArgData.push(currentCallArgData);
                }
                //Get function arguments
                if(triggerField[call] || (triggerField[decl_stmt] && triggerField[argument_list])){
                    GetCallData();//issue with statements like object(var)
                    while(!callArgData.empty())
                        callArgData.pop();
                }
                if(triggerField[expr] && triggerField[expr_stmt]){
                    if(exprassign){
                        ProcessExprStmtPostAssign();//problems with exprs like blotttom->next = expr
                        useExprStack.clear(); //found an assignment so throw everything off of the other stack TODO: Potential better way?
                        currentExprStmt.first.clear();
                    }else{
                        if(!useExprStmt.first.empty()){
                            if(Find(useExprStmt.first)){
                                useExprStack.push_back(useExprStmt);
                            }
                            useExprStmt.first.clear();                            
                        }
                    }
                }
                if(triggerField[init] && triggerField[decl] && triggerField[decl_stmt] && 
                !(triggerField[type] || triggerField[argument_list] || triggerField[call])){
                    ProcessDeclStmt();
                }
                exprop = false; //reset expr after each name so that the next name will be read unless there's another op in front of it
                --triggerField[name];
            } },
            { "macro", [this](){
                currentDeclType.first.clear();
                currentDecl.first.clear();
                --triggerField[macro];
            } },
            { "specifier", [this](){
                --triggerField[specifier];
            } },
        };

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
        FileIt = sysDict->ffvMap.insert(std::make_pair(std::string(attributes[2].value), FunctionVarMap())).first; //insert and keep track of most recent.         
        //std::cerr<<"val: "<<attributes[1].value<<std::endl;exit(1);
        //classIt = sysDict->classTable.insert(std::make_pair("GLOBAL", ClassProfile())).first;
        FunctionIt = FileIt->second.insert(std::make_pair("GLOBAL", VarMap())).first; //for globals. Makes a bad assumption about where globals are. Fix.
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
        std::string name;
        if(num_attributes){
            lineNum = strtoul(attributes[0].value, NULL, 0);
            name = attributes[0].value;
        }
        std::string lname(localname);
        std::string lnspace;
        if(prefix){
            lnspace.append(prefix);
        }
        if(lnspace == "cpp"){
            ++triggerField[preproc];
        }
        if(name == "generic"){
            sawgeneric = true;
        }
        
        std::unordered_map<std::string, std::function<void()>>::const_iterator process = process_map.find(lname);
        if (process != process_map.end()) {
            process->second();
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
        if(((triggerField[function] || triggerField[constructor] || triggerField[destructor]) && triggerField[name]) 
        && !(triggerField[functionblock] || triggerField[parameter_list]|| triggerField[argument_list] || triggerField[argument_list_template] || triggerField[type]
         || triggerField[index] || triggerField[preproc] || triggerField[op]|| triggerField[macro])){                
            currentFunctionBody.first.append(ch, len);
        }
        if(triggerField[type] && triggerField[function]  
            && !(triggerField[functionblock] || triggerField[op] || triggerField[argument_list] || triggerField[argument_list_template] || triggerField[templates] || triggerField[parameter_list]|| triggerField[macro] || triggerField[preproc])){

            if(!triggerField[modifier]){
                currentFunctionReturnType.first = std::string(ch, len);
            }else{
                currentFunctionReturnType.first.append(ch, len);
            }                
        }
        if((triggerField[functiondecl] || triggerField[constructordecl]) && !(triggerField[type] || triggerField[parameter_list])){
            currentFunctionDecl.first.append(ch,len);
        }
        if((triggerField[param] && (triggerField[function] || triggerField[functiondecl] || triggerField[constructor]) && triggerField[name])
            && !(triggerField[type] || triggerField[argument_list] || triggerField[templates]|| triggerField[macro] || triggerField[preproc])){
            currentParam.first.append(ch, len);
        }
        if((triggerField[param] && (triggerField[function] || triggerField[functiondecl] || triggerField[constructor])) && triggerField[name] &&  triggerField[type]
        && !(triggerField[argument_list] || triggerField[templates] || triggerField[op] || triggerField[macro] || triggerField[preproc])){
            currentParamType.first = std::string(ch, len);
        }
        if((triggerField[type] && triggerField[decl_stmt] && triggerField[name] && !(triggerField[argument_list] || triggerField[modifier] || triggerField[op]|| triggerField[macro] || triggerField[preproc]))){
            currentDeclType.first = std::string(ch,len);
        }
        //this is to handle lhs of decl stmts and rhs of decl stmts
        if((triggerField[name] || triggerField[op]) && triggerField[decl_stmt] && triggerField[decl] && !(triggerField[argument_list] || triggerField[index] || triggerField[preproc] || triggerField[type] || triggerField[macro])) {
            if(triggerField[init]){
                if(!triggerField[call]){//if it's not in a call then we can do like normal
                    currentDeclInit.first.append(ch,len);
                }else{
                    if(triggerField[argument]){//if it's in a call, ignore until we hit an argument
                        currentDeclInit.first.append(ch,len);
                    }
                }
            }else{
                currentDecl.first.append(ch,len);
            }
        }
        //this is to handle lhs of decl stmts and rhs of decl stmts
        if(!sawgeneric && (triggerField[name]) && triggerField[decl_stmt] && triggerField[argument_list] && triggerField[decl] &&  !(triggerField[op] || triggerField[index] || triggerField[preproc] || triggerField[type] || triggerField[macro])) {
                currentDeclCtor.first.append(ch,len);
        }
        //This only handles expr statments of the form a = b. Anything without = in it is skipped here -- Not entirely true anymore
        if((triggerField[name] || triggerField[op]) && triggerField[expr] && triggerField[expr_stmt] && !(triggerField[index] || triggerField[preproc])){
            std::string str = std::string(ch, len);
            if(str.back() == '='){
                exprassign = true;
                exprop = false; //assumed above in "operator" that I wouldn't see =. This takes care of when I assume wrong.
                str.clear(); //don't read the =, just want to know it was there.
            }
            if(triggerField[op]){
                if(str == "*"){
                    dereferenced = true; //most recent word was dereferenced
                    exprop = false; //slight hack. Need to be able to tell when * is used as dereferenced because I don't wanna skip
                }
                str.clear();
            }
            if(exprassign){
                if(!triggerField[call]){//if it's not in a call then we can do like normal
                    currentExprStmt.first.append(str);
                }else{
                    if(triggerField[argument]){//if it's in a call, ignore until we hit an argument
                        currentExprStmt.first.append(str);
                    }
                }
            }else{
                if(!exprop){ //haven't seen any operator (including =)
                    lhsExprStmt.first.append(str);
                }
                useExprStmt.first.append(str); //catch expr_stmts like cout<<identifier;
            }
            
        }
        if(triggerField[call]){
            calledFunctionName.append(ch, len);
        }
        if(triggerField[decl_stmt] && triggerField[decl] && triggerField[argument_list] && triggerField[argument] && triggerField[expr] &&
            !triggerField[type]){
            currentDeclArg.first.append(ch,len);
        }
        if(!triggerField[classblock] && (triggerField[name] && triggerField[classn])){
            currentClassName.first.append(ch,len);
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
        lineNum = 0;
        if(prefix){
            lnspace.append(prefix);
        }
        if(lnspace == "cpp"){
            currentDeclType.first.clear();
            currentDecl.first.clear();
            --triggerField[preproc];
        }
        std::unordered_map<std::string, std::function<void()>>::const_iterator process3 = process_map3.find(lname);
        if (process3 != process_map3.end()) {
            process3->second();
        }
#pragma GCC diagnostic pop
    }
};
inline void DoComputation(srcSliceHandler& h, const FileFunctionVarMap& mp){
    for(FileFunctionVarMap::const_iterator ffvmIt = mp.begin(); ffvmIt != mp.end(); ++ffvmIt){
        h.ComputeInterprocedural(ffvmIt->first);
    }
}
#endif