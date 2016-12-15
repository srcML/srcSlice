#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>

//Problem with variables with same name in different scopes (Use a stack, every time change of scope push onto stack)
//Possibly add line numbers to names

class FunctionSliceProfilePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        struct FunctionSliceProfile{
            FunctionSliceProfile(){}
            FunctionSliceProfile(std::string name, std::string type, std::set<unsigned int> lines){
                identifierName = name;
                identifierType = type;
            }

            void clear(){
                identifierName.clear();
                identifierType.clear();
            }

            std::vector<std::string> namespaces;
            int linenumber;
            bool isConst;
            bool isReference;
            bool isPointer;
            bool isStatic;

            std::string identifierName;
            std::string identifierType;

            std::set<unsigned int> def;
            std::set<unsigned int> use;
        };
        struct FunctionSliceProfileMap{
           FunctionSliceProfileMap() = default;
           FunctionSliceProfileMap(std::map<std::string, FunctionSliceProfile> dat){
            dataset = dat;
           }
           void clear(){
            dataset.clear();
           }
           std::map<std::string, FunctionSliceProfile> dataset;
        };
        FunctionSliceProfileMap profileset;
        FunctionSliceProfile data;
        ~FunctionProfilePolicy(){}
        FunctionProfilePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            exprPolicy.AddListener(this);
            declTypePolicy.AddListener(this);
            InitializeEventHandlers();
        }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            using namespace srcSAXEventDispatch;
            if(ctx.IsOpen(ParserState::declstmt) && ctx.IsClosed(ParserState::exprstmt)){
                std::cerr<<"Call decl"<<std::endl;
                declData = *policy->Data<DeclTypePolicy::DeclTypeData>();

                //Generate profile
                auto it = profileset.dataset.find(declData.nameofidentifier);
                if(it != profileset.dataset.end()){
                    it->second.identifierName = declData.nameofidentifier;
                    it->second.identifierType = declData.nameoftype;
                    it->second.isConst = declData.isConst;
                    it->second.isReference = declData.isReference;
                    it->second.isPointer = declData.isPointer;
                    it->second.isStatic = declData.isStatic;
                }else{
                    data.identifierName = declData.nameofidentifier;
                    data.identifierType = declData.nameoftype;
                    data.isConst = declData.isConst;
                    data.isReference = declData.isReference;
                    data.isPointer = declData.isPointer;
                    data.isStatic = declData.isStatic;
                    profileset.dataset.insert(std::make_pair(declData.nameofidentifier, data));
                }
            }else if (ctx.IsOpen(ParserState::exprstmt) && ctx.IsClosed(ParserState::declstmt)){
                exprData = *policy->Data<ExprPolicy::ExprDataSet>();
                for(auto var : exprData.dataset){
                    auto it = profileset.dataset.find(var.first);
                    if(it != profileset.dataset.end()){
                        it->second.def = var.second.def;
                        it->second.use = var.second.use;
                    }else{
                        std::cerr<<"couldn't find identifier named: "<<var.first<<std::endl;
                    }
                }
                //data = exprData;
            }
        }

    protected:
        void * DataInner() const override {
            //export profile to listeners
            return new FunctionSliceProfile(data);
        }
    private:
        ExprPolicy exprPolicy;
        ExprPolicy::ExprDataSet exprData;
        DeclTypePolicy declTypePolicy;
        DeclTypePolicy::DeclTypeData declData;

        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;

            openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&declTypePolicy);
            };
            openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&exprPolicy);
            };
            //Closing EventMap
            closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&exprPolicy);
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&declTypePolicy);
            };
            closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx){
                NotifyAll(ctx);
            };
        }

};
