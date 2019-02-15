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

class SliceProfile{
    public:
        SliceProfile():index(0),visited(false),potentialAlias(false),dereferenced(false),isGlobal(false){}
        SliceProfile(unsigned int idx, std::string fle, std::string fcn, unsigned int sline, std::string name, bool alias = 0, bool global = 0):
        index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name),isGlobal(global) {
            dereferenced = false;
            visited = false;
        }
        SliceProfile(std::string name, bool alias = 0, bool global = 0, std::set<unsigned int> aDef = {}, std::set<unsigned int> aUse = {}):
         variableName(name), isGlobal(global), def(aDef), use(aUse) {
            dereferenced = false;
            visited = false;
        }

        unsigned int index;
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

            profileMap = pm;
        }
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
            using namespace srcSAXEventDispatch;
            if(typeid(FunctionSignaturePolicy) == typeid(*policy)){
                std::cerr<<"Yin"<<std::endl;
            }else if(typeid(DeclTypePolicy) == typeid(*policy)){
                decldata = *policy->Data<DeclData>();
                auto sliceProfileItr = profileMap->find(decldata.nameOfIdentifier);
                if(sliceProfileItr != profileMap->end()){
                    sliceProfileItr->second.push_back(SliceProfile(decldata.nameOfIdentifier, true, true));
                }else{
                    profileMap->insert(std::make_pair(decldata.nameOfIdentifier, std::vector<SliceProfile>{SliceProfile(decldata.nameOfIdentifier, true, true)}));
                }
            }else if(typeid(ExprPolicy) == typeid(*policy)){
                exprdataset = *policy->Data<ExprPolicy::ExprDataSet>();
                for(auto exprdata : exprdataset.dataset){
                    auto sliceProfileItr = profileMap->find(exprdata.second.nameofidentifier);
                    if(sliceProfileItr != profileMap->end()){
                        sliceProfileItr->second.back().use = exprdata.second.use;
                        sliceProfileItr->second.back().def = exprdata.second.def;
                    }else{
                        profileMap->insert(std::make_pair(exprdata.second.nameofidentifier, std::vector<SliceProfile>{SliceProfile(exprdata.second.nameofidentifier, true, true)}));
                    }   
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
        DeclData decldata;
        ExprPolicy::ExprDataSet exprdataset;
        ExprPolicy exprpolicy;
        DeclTypePolicy declpolicy;
        FunctionSignaturePolicy functionpolicy;

        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
                openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                    //std::cerr<<"enter3"<<std::endl;
                    ctx.dispatcher->AddListenerDispatch(&declpolicy);
                    //std::cerr<<"exit"<<std::endl;
                };
                openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                    //std::cerr<<"enter3"<<std::endl;
                    ctx.dispatcher->AddListenerDispatch(&exprpolicy);
                    //std::cerr<<"exit"<<std::endl;
                };
                closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                    //std::cerr<<"enter4"<<std::endl;
                    ctx.dispatcher->RemoveListenerDispatch(&declpolicy);
                    //std::cerr<<"exit"<<std::endl;
                };
                closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                    //std::cerr<<"enter4"<<std::endl;
                    ctx.dispatcher->RemoveListenerDispatch(&exprpolicy);
                    //std::cerr<<"exit"<<std::endl;
                };
        }
};
#endif