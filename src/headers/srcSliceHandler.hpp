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
#include <sstream>


/**
 * srcSliceHandler
 *
 * Base class that provides hooks for SAX processing.
 */

class srcSliceHandler : public srcSAXHandler {

private :
    enum ParserState {decl, expr, param, decl_stmt, expr_stmt, parameter_list, call, ctrlflow, endflow, name, function, nonterminal, empty};
    typedef std::pair<ParserState, std::string> StateFunctionPair;
    struct SliceMetaData{
        SliceMetaData(unsigned int line, ParserState current, ParserState prev): lineNumber(line), currentState(current), previousState(prev){};
        unsigned int lineNumber;
        ParserState currentState;
        ParserState previousState;
    };
    int fileNumber;
    int functionNumber;

    std::vector<SliceMetaData> SliceVarMetaDataStack;
    std::vector<StateFunctionPair> ParserContext;

    std::unordered_map<int, std::string> FunctionMap;
    std::unordered_map<int, std::string> FileMap;
    bool collectFunctionData;
public :
    srcSliceHandler(){
        collectFunctionData = false;
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
        unsigned int lineNum;
        if(num_attributes){
            lineNum = strtoul(attributes[0].value, NULL, 0);
        }
        if(std::string(localname) == "function"){
            collectFunctionData = true;
        }
        push_element(localname, prefix, lineNum);
    }
    void UpdateSliceProfile(std::string varName){
        //std::cerr<<SliceVarMetaDataStack.back().currentState<<std::endl;
        if(SliceVarMetaDataStack.front().currentState == decl_stmt){   
            SliceMetaData data = SliceVarMetaDataStack.back();
            if(data.currentState == name && data.previousState == decl){
                std::cerr<<"VAR: "<<varName<<" "<<data.lineNumber<<std::endl;
            }
        }else if(SliceVarMetaDataStack.front().currentState == parameter_list){
            SliceMetaData data = SliceVarMetaDataStack.back();
            if(data.currentState == name && data.previousState == decl){
                std::cerr<<"VAR: "<<varName<<" "<<data.lineNumber<<std::endl;
            }
        }else if(SliceVarMetaDataStack.front().currentState == expr_stmt){
            SliceMetaData data = SliceVarMetaDataStack.back();
            if(data.currentState == name && data.previousState == expr){
                std::cerr<<"VAR: "<<varName<<" "<<data.lineNumber<<std::endl;
            }
        }
        /*
                            if(data.previousState == function);
                        //std::cerr<<varName<<" "<<data.lineNumber<<std::endl;
                    if(data.previousState == name);*/
    }
    void UpdateFunctionContext(std::string functionName){
        if(ParserContext.back().first == name){
            ParserContext.back().second = functionName;
        }else if (ParserContext.back().first == parameter_list){
            ParserContext.pop_back();
            FunctionMap.insert(std::make_pair(functionNumber, ParserContext.back().second));
            ++functionNumber;
            std::cerr<<"FUNCTION: "<<ParserContext.back().second<<std::endl;
            collectFunctionData = false;
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
        std::string content = "";
        content.append(ch, len);
        
        if(!SliceVarMetaDataStack.empty())
            UpdateSliceProfile(content);
        if(collectFunctionData)
            UpdateFunctionContext(content);

        //std::cerr<<elementIter->currentState<<" "<<(++elementIter)->currentState<<elementIter->previousState<<std::endl;

        /*Names are equivalent to terminal nodes. There has to be some kind of state when I see one.
         *So the idea is that maybe I can keep track of what state I'm in by recording a small amount
         *of information while I parse the doc. Enums or something.*/
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
        
        FileMap.insert(std::make_pair(fileNumber, attributes[1].value));
        ++fileNumber;
        std::cerr<<attributes[1].value<<std::endl;
        //Take filename attribute and hash it.      
        //push_element(localname, prefix);
    }
    void push_element(const char * localname, const char * prefix, unsigned int line = 0) {

        std::string full_name = "";

        if(prefix) {

            full_name = prefix;
            full_name += ":";

        }

        full_name += localname;

        ParserState prev = SliceVarMetaDataStack.empty() ? empty : SliceVarMetaDataStack.back().currentState;
        
        if(full_name == "decl_stmt"){
            SliceVarMetaDataStack.push_back(SliceMetaData(line, decl_stmt, prev));
        }else if (full_name == "expr_stmt"){
            SliceVarMetaDataStack.push_back(SliceMetaData(line, expr_stmt, prev));
        }else if (full_name == "parameter_list"){
            ParserContext.push_back(StateFunctionPair(parameter_list, ""));
            SliceVarMetaDataStack.push_back(SliceMetaData(line, parameter_list, prev));
        }else if (full_name == "param"){
            SliceVarMetaDataStack.push_back(SliceMetaData(line, param, prev));
        }else if(full_name == "decl"){
            SliceVarMetaDataStack.push_back(SliceMetaData(line, decl, prev));
        }else if (full_name == "expr"){
            SliceVarMetaDataStack.push_back(SliceMetaData(line, expr, prev));
        }else if (full_name == "call"){
            SliceVarMetaDataStack.push_back(SliceMetaData(line, call, prev));
        }else if (full_name == "name"){
            if(collectFunctionData){
               ParserContext.push_back(StateFunctionPair(name, ""));
            }
            SliceVarMetaDataStack.push_back(SliceMetaData(line, name, prev));
        }else if (full_name == "function"){
            ParserContext.push_back(StateFunctionPair(function, ""));
        }else if(!SliceVarMetaDataStack.empty()){
            if(!collectFunctionData){
                ParserContext.push_back(StateFunctionPair(nonterminal, ""));
            }
            SliceVarMetaDataStack.push_back(SliceMetaData(line, nonterminal, prev));
        }

    }
    

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {
         //if(!SliceVarMetaDataStack.empty()) SliceVarMetaDataStack.pop_back();
    }
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {
         //if(!SliceVarMetaDataStack.empty()) SliceVarMetaDataStack.pop_back();
    }
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
         if(!SliceVarMetaDataStack.empty()) SliceVarMetaDataStack.pop_back();
    }
    /*
    virtual void comment(const char * value) {}
    virtual void cdataBlock(const char * value, int len) {}
    virtual void processingInstruction(const char * target, const char * data) {}
    */

#pragma GCC diagnostic pop

};

#endif
