#ifndef SRCSLICEPOLICY
#define SRCSLICEPOLICY

#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>
#include <InitPolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <FunctionCallPolicy.hpp>

class SliceProfile{
    public:
        SliceProfile():index(0),containsDeclaration(false),potentialAlias(false),dereferenced(false),isGlobal(false){}
        SliceProfile(
            std::string name, int line, bool alias = 0, bool global = 0, 
            std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {}, 
            std::vector<std::pair<std::string, std::string>> cFunc = {}, 
            std::set<std::string> dv = {}, bool containsDecl = false):
                variableName(name), lineNumber(line), potentialAlias(alias), 
                isGlobal(global), definitions(aDef), uses(aUse), cfunctions(cFunc), 
                dvars(dv), containsDeclaration(containsDecl){
            
            dereferenced = false;
        }

        void PrintProfile(){
            std::cout<<"=========================================================================="<<std::endl;
            std::cout<<"Name and type: "<<variableName<<" "<<variableType<<std::endl;
            std::cout<<"Dvars: {";
            for(auto dvar : dvars){
                std::cout<<dvar<<",";
            }
            std::cout<<"}"<<std::endl;
            std::cout<<"Aliases: {";
            for(auto alias : aliases){
                std::cout<<alias<<",";
            }
            std::cout<<"}"<<std::endl;
            std::cout<<"Cfunctions: {";
            for(auto cfunc : cfunctions){
                std::cout<<cfunc.first<<" "<<cfunc.second<<",";
            }
            std::cout<<"}"<<std::endl;
            std::cout<<"Use: {";
            for(auto use : uses){
                std::cout<<use<<",";
            }
            std::cout<<"}"<<std::endl;
            std::cout<<"Def: {";
            for(auto def : definitions){
                std::cout<<def<<",";
            }
            std::cout<<"}"<<std::endl;
            std::cout<<"=========================================================================="<<std::endl;
        }

        unsigned int index;
        int lineNumber;
        std::string file;
        std::string function;
        
        bool potentialAlias;
        bool dereferenced;

        bool isGlobal;
        bool containsDeclaration;

        std::string variableName;
        std::string variableType;
        std::unordered_set<std::string> memberVariables;

        std::set<unsigned int> definitions;
        std::set<unsigned int> uses;
        
        std::set<std::string> dvars;
        std::set<std::string> aliases;

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
        
            declPolicy.AddListener(this);
            exprPolicy.AddListener(this);
            callPolicy.AddListener(this);
            initPolicy.AddListener(this);
            paramPolicy.AddListener(this);

            profileMap = pm;
        }
        void MergeProfiles(SliceProfile currentSliceProfile, std::vector<SliceProfile>* sliceProfiles){
            for(std::vector<SliceProfile>::iterator it = sliceProfiles->begin(); it != sliceProfiles->end(); ++it){
                //if current profile and another profile have the same containing class and/or function name
                //and if of one them is the actual declaration
                //marge the profiles. Otherwise, just add the new profile
            }

        }
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
            using namespace srcSAXEventDispatch;
            if(typeid(DeclTypePolicy) == typeid(*policy)){
                decldata = *policy->Data<DeclData>();
                auto sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);
                
                //Just add new slice profile if name already exists. Otherwise, add new entry in map.
                if(sliceProfileItr != profileMap->end()){
                    sliceProfileItr->second.push_back(SliceProfile(decldata.nameOfIdentifier,decldata.lineNumber, (decldata.isPointer || decldata.isReference), true, std::set<unsigned int>{decldata.lineNumber}));
                    sliceProfileItr->second.back().containsDeclaration = true;
                }else{
                    auto sliceProf = SliceProfile(decldata.nameOfIdentifier,decldata.lineNumber,
                     (decldata.isPointer || decldata.isReference), true, std::set<unsigned int>{decldata.lineNumber});
                    sliceProf.containsDeclaration = true;
                    profileMap->insert(std::make_pair(decldata.nameOfIdentifier, 
                        std::vector<SliceProfile>{
                           std::move(sliceProf)
                        }));
                }
                sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);
                //look at the dvars and add this current variable to their dvar's lists. If we haven't seen this name before, add its slice profile
                for(std::string dvar : declDvars){
                    auto updateDvarAtThisLocation = profileMap->find(dvar);
                    if(updateDvarAtThisLocation != profileMap->end()){
                        updateDvarAtThisLocation->second.back().dvars.insert(decldata.nameOfIdentifier);
                        if(sliceProfileItr != profileMap->end() && sliceProfileItr->second.back().potentialAlias){
                            updateDvarAtThisLocation->second.back().aliases.insert(decldata.nameOfIdentifier);
                        }
                    }else{
                        auto sliceProf = SliceProfile(dvar, decldata.lineNumber, true, true, std::set<unsigned int>{}, std::set<unsigned int>{decldata.lineNumber});
                        auto newSliceProfileFromDeclDvars = profileMap->insert(std::make_pair(dvar, 
                            std::vector<SliceProfile>{
                                std::move(sliceProf)
                            }));
                        newSliceProfileFromDeclDvars.first->second.back().dvars.insert(decldata.nameOfIdentifier);
                        if(sliceProfileItr != profileMap->end() && sliceProfileItr->second.back().potentialAlias){
                            newSliceProfileFromDeclDvars.first->second.back().aliases.insert(decldata.nameOfIdentifier);
                        }
                    }
                }
                declDvars.clear();
            }else if(typeid(ExprPolicy) == typeid(*policy)){
                exprDataSet = *policy->Data<ExprPolicy::ExprDataSet>();
                //iterate through every token found in the expression statement
                for(auto exprdata : exprDataSet.dataSet){
                    auto sliceProfileExprItr = profileMap->find(exprdata.second.nameOfIdentifier);
                    auto sliceProfileLHSItr = profileMap->find(exprDataSet.lhsName);
                    //Just update definitions and uses if name already exists. Otherwise, add new name.
                    if(sliceProfileExprItr != profileMap->end()){
                        sliceProfileExprItr->second.back().uses.insert(exprdata.second.uses.begin(), exprdata.second.uses.end());
                        sliceProfileExprItr->second.back().definitions.insert(exprdata.second.definitions.begin(), exprdata.second.definitions.end());
                        if(!currentName.empty() && (exprdata.second.lhs || currentName!=exprdata.second.nameOfIdentifier)) sliceProfileExprItr->second.back().dvars.insert(currentName);
                        if(sliceProfileLHSItr!= profileMap->end() && sliceProfileLHSItr->second.back().potentialAlias) sliceProfileExprItr->second.back().aliases.insert(exprDataSet.lhsName);
                    }else{
                        auto sliceProfileExprItr2 = profileMap->insert(std::make_pair(exprdata.second.nameOfIdentifier, 
                            std::vector<SliceProfile>{
                                SliceProfile(exprdata.second.nameOfIdentifier, ctx.currentLineNumber, true, true, 
                                    exprdata.second.definitions, exprdata.second.uses)
                            }));
                        if(sliceProfileLHSItr!= profileMap->end() && sliceProfileLHSItr->second.back().potentialAlias) sliceProfileExprItr2.first->second.back().aliases.insert(exprDataSet.lhsName);
                        //Only ever record a variable as being a dvar of itself if it was seen on both sides of =
                        if(!currentName.empty() && (exprdata.second.lhs || currentName!=exprdata.second.nameOfIdentifier)) sliceProfileExprItr2.first->second.back().dvars.insert(currentName);
                    }
                }
            }else if(typeid(InitPolicy) == typeid(*policy)){
                initDataSet = *policy->Data<InitPolicy::InitDataSet>();
                //iterate through every token found in the initialization of a decl_stmt
                for(auto initdata : initDataSet.dataSet){
                    declDvars.push_back(initdata.second.nameOfIdentifier);
                    auto sliceProfileItr = profileMap->find(initdata.second.nameOfIdentifier);
                    //Just update definitions and uses if name already exists. Otherwise, add new name.
                    if(sliceProfileItr != profileMap->end()){
                        sliceProfileItr->second.back().uses.insert(initdata.second.uses.begin(), initdata.second.uses.end());
                    }else{
                        profileMap->insert(std::make_pair(initdata.second.nameOfIdentifier, 
                            std::vector<SliceProfile>{
                                SliceProfile(initdata.second.nameOfIdentifier, ctx.currentLineNumber, true, true, 
                                    std::set<unsigned int>{}, initdata.second.uses)
                            }));
                    }   
                }
            }else if(typeid(CallPolicy) == typeid(*policy)){
                calldata = *policy->Data<CallPolicy::CallData>();
                bool isFuncNameNext = false;
                std::vector<std::pair<std::string, unsigned int>> funcNameAndCurrArgumentPos;
                //Go through each token found in a function call
                for(auto currentCallToken : calldata.callargumentlist){
                    //Check to see if we are entering a function call or exiting-- 
                    //if entering, we know the next token is the name of the call
                    //otherwise, we're exiting and need to pop the current function call off the stack
                    switch(currentCallToken[0]){
                        case '(':{ 
                            isFuncNameNext = true;
                            continue;
                        }
                        case ')':{
                            if(!funcNameAndCurrArgumentPos.empty()) funcNameAndCurrArgumentPos.pop_back();
                            continue;
                        }
                    }
                    //If we noted that a function name was coming in that switch above, record it here.
                    //Otherwise, the next token is an argument in the function call
                    if(isFuncNameNext){
                        funcNameAndCurrArgumentPos.push_back(std::make_pair(currentCallToken, 1));
                        isFuncNameNext = false;
                    }else{
                        auto sliceProfileItr = profileMap->find(currentCallToken);
                        
                        std::string callOrder, argumentOrder;
                        for(auto name : funcNameAndCurrArgumentPos){
                            callOrder+=name.first+'-';
                            argumentOrder+=std::to_string(name.second)+'-';
                        }
                        if(!callOrder.empty())callOrder.erase(callOrder.size()-1); ///need to implement join
                        if(!argumentOrder.empty()) argumentOrder.erase(argumentOrder.size()-1); ///need to implement join
                        
                        //Just update cfunctions if name already exists. Otherwise, add new name.
                        if(sliceProfileItr != profileMap->end()){
                            sliceProfileItr->second.back().cfunctions.push_back(std::make_pair(callOrder, argumentOrder));
                        }else{  
                            profileMap->insert(std::make_pair(currentCallToken, 
                                std::vector<SliceProfile>{
                                    SliceProfile(currentCallToken, ctx.currentLineNumber, true, true, 
                                        std::set<unsigned int>{}, std::set<unsigned int>{ctx.currentLineNumber}, 
                                        std::vector<std::pair<std::string, std::string>>{std::make_pair(callOrder, argumentOrder)})
                                }));
                        }
                        if(!funcNameAndCurrArgumentPos.empty()) ++funcNameAndCurrArgumentPos.back().second;
                    }
                }
            }else if(typeid(ParamTypePolicy) == typeid(*policy)){
                paramdata = *policy->Data<DeclData>();
                //record parameter data-- this is done exact as it is done for decl_stmts except there's no initializer
                auto sliceProfileItr = profileMap->find(paramdata.nameOfIdentifier);
                //Just add new slice profile if name already exists. Otherwise, add new entry in map.
                if(sliceProfileItr != profileMap->end()){
                    sliceProfileItr->second.push_back(SliceProfile(paramdata.nameOfIdentifier,paramdata.lineNumber, (paramdata.isPointer || paramdata.isReference), true, std::set<unsigned int>{paramdata.lineNumber}));
                }else{
                    profileMap->insert(std::make_pair(paramdata.nameOfIdentifier, 
                        std::vector<SliceProfile>{
                            SliceProfile(paramdata.nameOfIdentifier,paramdata.lineNumber, (paramdata.isPointer || paramdata.isReference), true, std::set<unsigned int>{paramdata.lineNumber})
                        }));
                }
            }
        }
        void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx){}
    
    protected:
        void *DataInner() const override {
            return (void*)0; // export profile to listeners
        }
        
    private:
        DeclTypePolicy declPolicy;
        DeclData decldata;

        ParamTypePolicy paramPolicy;
        DeclData paramdata;

        InitPolicy initPolicy;
        InitPolicy::InitDataSet initDataSet;
        
        ExprPolicy::ExprDataSet exprDataSet;
        ExprPolicy exprPolicy;  
        
        CallPolicy callPolicy;
        CallPolicy::CallData calldata;

        FunctionSignaturePolicy functionpolicy;
        std::string currentExprName;
        std::vector<std::string> declDvars;

        std::string currentName;
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            closeEventMap[ParserState::op] = [this](srcSAXEventContext& ctx){
                if(ctx.currentToken == "="){
                    currentName = currentExprName;
                }
            };
            openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->AddListenerDispatch(&declPolicy);
            };
            openEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&paramPolicy);
            };
            openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->AddListenerDispatch(&exprPolicy);
            };
            openEventMap[ParserState::call] = [this](srcSAXEventContext& ctx){
                //don't want multiple callPolicy parsers running
                if(ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                    ctx.dispatcher->AddListenerDispatch(&callPolicy);
                }
            };
            openEventMap[ParserState::init] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->AddListenerDispatch(&initPolicy);
            };
            closeEventMap[ParserState::call] = [this](srcSAXEventContext& ctx){
                if(ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                    ctx.dispatcher->RemoveListenerDispatch(&callPolicy);
                }
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&declPolicy);
                currentName.clear();
            };
            closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&exprPolicy);
                currentName.clear();
            };
            closeEventMap[ParserState::init] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&initPolicy);
            };
            closeEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->RemoveListenerDispatch(&paramPolicy);
            };
            closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext& ctx){
                //TODO: possibly, this if-statement is suppressing more than just unmarked whitespace. Investigate.
                if(!(ctx.currentToken.empty() || ctx.currentToken == " ")){
                    if(ctx.And({ParserState::name, ParserState::expr, ParserState::exprstmt}) && ctx.Nor({ParserState::specifier, ParserState::modifier, ParserState::op})){
                        currentExprName = ctx.currentToken;
                    }
                }
            };
        }
};
#endif