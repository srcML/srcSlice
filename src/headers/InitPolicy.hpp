/**
 * @file InitPolicy.hpp
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
#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <set>
#include <vector>
#ifndef INITPOLICY
#define INITPOLICY
class InitPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        struct InitData{
            InitData() {}
            void clear(){
               use.clear();
            }
            std::string nameofidentifier;
            std::set<unsigned int> use; //could be used multiple times in same init
        };
        struct InitDataSet{
           InitDataSet() = default;
           InitDataSet(std::map<std::string, InitData> dat){
            dataset = dat;
           }
           void clear(){
            dataset.clear();
           }
           std::map<std::string, InitData> dataset;
        };
        std::map<std::string, InitData> dataset;
        ~InitPolicy(){}
        InitPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            seenAssignment = false;
            InitializeEventHandlers();
        }
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {} //doesn't use other parsers
        void NotifyWrite(const PolicyDispatcher * policy, srcSAXEventDispatch::srcSAXEventContext & ctx) override {} //doesn't use other parsers
    protected:
        void * DataInner() const override {
            return new InitDataSet(dataset);
        }
    private:
        InitData data;
        std::string currentTypeName, currentInitName, currentModifier, currentSpecifier;
        std::vector<unsigned int> currentLine;
        bool seenAssignment;
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            closeEventMap[ParserState::modifier] = [this](srcSAXEventContext& ctx){
                
            };

            closeEventMap[ParserState::name] = [this](srcSAXEventContext& ctx){
                if(currentLine.empty() || currentLine.back() != ctx.currentLineNumber){
                    currentLine.push_back(ctx.currentLineNumber);
                }
                if(ctx.IsOpen({ParserState::declstmt})){
                    auto it = dataset.find(currentInitName);
                    if(it != dataset.end()){
                        it->second.use.insert(currentLine.back()); //assume it's a use
                    }else{
                        data.nameofidentifier = currentInitName;
                        data.use.insert(currentLine.back());
                        dataset.insert(std::make_pair(currentInitName, data));
                    }
                }
            };

            closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext& ctx){
                //TODO: possibly, this if-statement is suppressing more than just unmarked whitespace. Investigate.
                if(!(ctx.currentToken.empty() || ctx.currentToken == " ")){
                    if(ctx.And({ParserState::name, ParserState::init, ParserState::declstmt}) && ctx.Nor({ParserState::specifier, ParserState::modifier, ParserState::op})){
                        currentInitName = ctx.currentToken;
                    }
                    if(ctx.And({ParserState::specifier, ParserState::init, ParserState::declstmt})){
                        currentSpecifier = ctx.currentToken;
                    }
                    if(ctx.And({ParserState::modifier, ParserState::declstmt})){
                        currentModifier = ctx.currentToken;
                    }
                }
            };
            closeEventMap[ParserState::init] = [this](srcSAXEventContext& ctx){
                NotifyAll(ctx);
                currentLine.pop_back();
                seenAssignment = false;
                currentLine.clear();
                dataset.clear();
                data.clear();
            };

        }
};
#endif