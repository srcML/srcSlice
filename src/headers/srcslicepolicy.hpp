#ifndef SRCSLICEPOLICY
#define SRCSLICEPOLICY

#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <FunctionCallPolicy.hpp>

class SliceProfile{
    public:
        SliceProfile():index(0),visited(false),potentialAlias(false),dereferenced(false),isGlobal(false){}
        SliceProfile(unsigned int idx, std::string fle, std::string fcn, unsigned int sline, std::string name, bool alias = 0, bool global = 0):
        index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name),isGlobal(global) {
            dereferenced = false;
            visited = false;
        }
        SliceProfile(std::string name, int line, bool alias = 0, bool global = 0, 
            std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {}, std::vector<std::pair<std::string, std::string>> cFunc = {}):
         variableName(name), linenumber(line), isGlobal(global), def(aDef), use(aUse), cfunctions(cFunc) {
            dereferenced = false;
            visited = false;
        }

        unsigned int index;
        int linenumber;
        std::string file;
        std::string function;
        
        bool potentialAlias;
        bool dereferenced;

        bool isGlobal;
        bool visited;

        std::string variableName;
        std::string variableType;
        std::unordered_set<std::string> memberVariables;

        std::set<unsigned int> def;
        std::set<unsigned int> use;
        
        std::unordered_set<std::string> dvars;
        std::unordered_set<std::string> aliases;

        std::vector<std::pair<std::string, std::string>> cfunctions;
};

class SrcSlicePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    public:
        ~SrcSlicePolicy(){};
        std::unordered_map<std::string, std::vector<SliceProfile>>* profileMap;
        SrcSlicePolicy(std::unordered_map<std::string, std::vector<SliceProfile>>* pm, std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){
            // making SSP a listener for FSPP
            InitializeEventHandlers();
            
            declpolicy.AddListener(this);
            exprpolicy.AddListener(this);
            callpolicy.AddListener(this);

            profileMap = pm;
        }
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
            using namespace srcSAXEventDispatch;
            if(typeid(DeclTypePolicy) == typeid(*policy)){
                //ctx.currentFunctionName
                decldata = *policy->Data<DeclData>();
                auto sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);
                if(sliceProfileItr != profileMap->end()){
                    sliceProfileItr->second.push_back(SliceProfile(decldata.nameOfIdentifier,decldata.linenumber, true, true, std::set<unsigned int>{decldata.linenumber}));
                }else{
                    profileMap->insert(std::make_pair(decldata.nameOfIdentifier, 
                        std::vector<SliceProfile>{
                            SliceProfile(decldata.nameOfIdentifier,decldata.linenumber, true, true, std::set<unsigned int>{decldata.linenumber})
                        }));
                }
            }else if(typeid(ExprPolicy) == typeid(*policy)){
                exprdataset = *policy->Data<ExprPolicy::ExprDataSet>();
                for(auto exprdata : exprdataset.dataset){
                    auto sliceProfileItr = profileMap->find(exprdata.second.nameofidentifier);
                    if(sliceProfileItr != profileMap->end()){
                        sliceProfileItr->second.back().use.insert(exprdata.second.use.begin(), exprdata.second.use.end());
                        sliceProfileItr->second.back().def.insert(exprdata.second.def.begin(), exprdata.second.def.end());
                    }else{
                        profileMap->insert(std::make_pair(exprdata.second.nameofidentifier, 
                            std::vector<SliceProfile>{
                                SliceProfile(exprdata.second.nameofidentifier, ctx.currentLineNumber, true, true, 
                                    exprdata.second.def, exprdata.second.use)
                            }));
                    }   
                }
            }else if(typeid(CallPolicy) == typeid(*policy)){
                calldata = *policy->Data<CallPolicy::CallData>();
                int depthCounter = 0, iterationCounter = 0;
                bool funcNameIsNext = false;
                std::vector<std::pair<std::string, unsigned int>> funcName;
                for(auto functString : calldata.callargumentlist){
                    switch(functString[0]){
                        case '(':{ 
                            ++depthCounter;
                            funcNameIsNext = true;
                            continue;
                        }
                        case ')':{
                            --depthCounter;
                            funcName.pop_back();
                            continue;
                        }
                    }
                    if(funcNameIsNext){
                        funcName.push_back(std::make_pair(functString, 1));
                        funcNameIsNext = false;
                    }else{
                        auto sliceProfileItr = profileMap->find(functString);
                        
                        std::string callOrder, argumentOrder;
                        for(auto name : funcName){
                            callOrder+=name.first+'-';
                            argumentOrder+=std::to_string(name.second)+'-';
                        }
                        callOrder.erase(callOrder.size()-1); ///need to implement join
                        argumentOrder.erase(argumentOrder.size()-1); ///need to implement join
                        
                        if(sliceProfileItr != profileMap->end()){
                            sliceProfileItr->second.back().cfunctions.push_back(std::make_pair(callOrder, argumentOrder));
                        }else{  
                            profileMap->insert(std::make_pair(functString, 
                                std::vector<SliceProfile>{
                                    SliceProfile(functString, ctx.currentLineNumber, true, true, 
                                        std::set<unsigned int>{}, std::set<unsigned int>{ctx.currentLineNumber}, 
                                        std::vector<std::pair<std::string, std::string>>{std::make_pair(callOrder, argumentOrder)})
                                }));
                        }

                        ++funcName.back().second;
                    }
                    ++iterationCounter;
                }
            }
        }
        void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx){

        }
    
    protected:
        void *DataInner() const override {
            return (void*)0; // export profile to listeners
        }
        
    private:
        DeclTypePolicy declpolicy;
        DeclData decldata;
        
        ExprPolicy::ExprDataSet exprdataset;
        ExprPolicy exprpolicy;
        
        
        CallPolicy callpolicy;
        CallPolicy::CallData calldata;

        FunctionSignaturePolicy functionpolicy;

        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
                openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                    ctx.dispatcher->AddListenerDispatch(&declpolicy);
                };
                openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                    ctx.dispatcher->AddListenerDispatch(&exprpolicy);
                };
                openEventMap[ParserState::call] = [this](srcSAXEventContext& ctx){
                    //don't want multiple callpolicy parsers running
                    if(ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                        ctx.dispatcher->AddListenerDispatch(&callpolicy);
                        ctx.dispatcher->RemoveListenerDispatch(&exprpolicy); //remove expr policy so it doesn't interfere
                    }
                };
                closeEventMap[ParserState::call] = [this](srcSAXEventContext& ctx){
                    if(ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                        ctx.dispatcher->RemoveListenerDispatch(&callpolicy);
                        ctx.dispatcher->AddListenerDispatch(&exprpolicy); //re-add expr policy once done with call.
                    }
                };
                closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                    ctx.dispatcher->RemoveListenerDispatch(&declpolicy);
                };
                closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                    ctx.dispatcher->RemoveListenerDispatch(&exprpolicy);
                };
        }
};
#endif