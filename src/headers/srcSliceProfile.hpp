#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>

//Problem with variables with same name in different scopes (Use a stack, every time change of scope push onto stack)
//Possibly add line numbers to names

class srcSliceProfilePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        struct SrcProfile{
            SrcProfile(){}
            SrcProfile(std::string name, std::string type, std::set<unsigned int> lines){
                identifierName = name;
                identifierType = type;
                lineNumbers = lines;
            }

            void clear(){
                identifierName.clear();
                identifierType.clear();
                lineNumbers.clear();
            }

            std::vector<std::string> namespaces;
            int linenumber;
            bool isConst;
            bool isReference;
            bool isPointer;
            bool isStatic;

            std::string identifierName;
            std::string identifierType;
            std::set<unsigned int> lineNumbers;
        };
        SrcProfile data;
        ~srcSliceProfilePolicy(){}
        srcSliceProfilePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
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
                data.identifierName = declData.nameofidentifier;
                data.identifierType = declData.nameoftype;
                data.isConst = declData.isConst;
                data.isReference = declData.isReference;
                data.isPointer = declData.isPointer;
                data.isStatic = declData.isStatic;
            }else if (ctx.IsOpen(ParserState::exprstmt) && ctx.IsClosed(ParserState::declstmt)){
                std::cerr<<"Call expr"<<std::endl;
                exprData = *policy->Data<ExprPolicy::ExprDataSet>();
            }
        }

    protected:
        void * DataInner() const override {
            //export profile to listeners
            return new SrcProfile(data);
        }
    private:
        ExprPolicy exprPolicy;
        ExprPolicy::ExprDataSet exprData;
        DeclTypePolicy declTypePolicy;
        DeclTypePolicy::DeclTypeData declData;

        //This correct?
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
