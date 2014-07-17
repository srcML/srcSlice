/**
 * @file element_count_handler.hpp
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

#ifndef INCLUDED_ELEMENT_COUNT_HANDLER_HPP
#define INCLUDED_ELEMENT_COUNT_HANDLER_HPP

#include <srcSAXHandler.hpp>
#include <SliceProfile.hpp>

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sstream>

/**
 * element_count_handler
 *
 * Base class that provides hooks for SAX processing.
 */

class srcSliceHandler : public srcSAXHandler {

private :
    enum ParserState {decl, expr, call, param, ctrlflow, endflow, name, function, nonterminal, empty};
    struct SliceMetaData{
        SliceMetaData(unsigned int line, ParserState current, ParserState prev): lineNumber(line), currentState(current), previousState(prev){};
        unsigned int lineNumber;
        ParserState currentState;
        ParserState previousState;
    };
    std::vector<SliceMetaData> open_elements;
public :

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
        
        push_element(localname, prefix);
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
        push_element(localname, prefix, lineNum);
    }
    void Dispatch(){
        //std::cerr<<str<<std::endl;
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

        auto elementIter = open_elements.rbegin();
        //std::cerr<<elementIter->currentState<<" "<<(++elementIter)->currentState<<elementIter->previousState<<std::endl;
        if(elementIter->currentState == name && elementIter->previousState == decl){
            std::cerr<<content<<" "<<elementIter->lineNumber<<std::endl;
        }
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
        
        push_element(localname, prefix);
    }
    void push_element(const char * localname, const char * prefix, unsigned int line = 0) {

        std::string full_name = "";

        if(prefix) {

            full_name = prefix;
            full_name += ":";

        }

        full_name += localname;

        ParserState prev = open_elements.empty() ? empty : open_elements.back().currentState;
        
        if(full_name == "decl"){
            open_elements.push_back(SliceMetaData(line, decl, prev));
        }else if (full_name == "name"){
            open_elements.push_back(SliceMetaData(line, name, prev));
        }else{
            open_elements.push_back(SliceMetaData(line, nonterminal, prev));
        }

    }
    

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {
         open_elements.pop_back();
    }
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {
         open_elements.pop_back();
    }
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
         open_elements.pop_back();
    }
    /*
    virtual void comment(const char * value) {}
    virtual void cdataBlock(const char * value, int len) {}
    virtual void processingInstruction(const char * target, const char * data) {}
    */

#pragma GCC diagnostic pop

};

#endif
