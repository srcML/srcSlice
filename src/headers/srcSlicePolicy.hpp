#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>

//Problem with variables with same name in different scopes (Use a stack, every time change of scope push onto stack)
//Possibly add line numbers to names

class SrcSlicePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        struct SrcSliceData{
            SrcSliceData(){}
            SrcSliceData(std::string name, std::string type, std::set<unsigned int> lines){
                identifierName = name;
                identifierType = type;
                lineNumbers = lines;
            }

            void clear(){
                identifierName.clear();
                identifierType.clear();
                lineNumbers.clear(); //Should I clear this???
            }

            std::string identifierName;
            std::string identifierType;
            std::set<unsigned int> lineNumbers;
        };

        struct SrcSliceSet{
            SrcSliceSet() = default;
            SrcSliceSet(std::map<std::string, SrcSliceData> dat){
            srcSliceMap = dat;
           }
           void clear(){
            srcSliceMap.clear();
           }
           std::map<std::string, SrcSliceData> srcSliceMap;
        };

        std::map<std::string, SrcSliceData> srcSliceMap;
        SrcSliceSet data;
        ~SrcSlicePolicy(){}
        SrcSlicePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            exprPolicy.AddListener(this);
            declTypePolicy.AddListener(this);
            InitializeEventHandlers();
        }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            using namespace srcSAXEventDispatch;
            

            if(ctx.IsOpen(ParserState::declstmt) && ctx.IsClosed(ParserState::exprstmt)){
                declData = *policy->Data<DeclTypePolicy::DeclTypeData>();


                auto it = srcSliceMap.find(declData.nameofidentifier);
                if(it == srcSliceMap.end()){
                    std::set<unsigned int> lines;
                    lines.insert(declData.linenumber);
                    srcSliceMap.insert(std::make_pair(declData.nameofidentifier, SrcSliceData(declData.nameofidentifier, declData.nameoftype, lines)));
                }else{
                    //What to do about multiple decl of same name
                }


            }else if (ctx.IsOpen(ParserState::exprstmt) && ctx.IsClosed(ParserState::declstmt)){
                exprData = *policy->Data<ExprPolicy::ExprDataSet>();

                //Needed?
                for(auto deal : exprData.dataset){
                    auto it = srcSliceMap.find(deal.second.nameofidentifier);
                    if(it != srcSliceMap.end()){


                        std::set<unsigned int> temp1 = deal.second.def;
                        std::set<unsigned int> temp2 = deal.second.use;
                        std::set<unsigned int> final = temp2;
                        bool match = false;


                        //Union
                        /*
                        for(std::set<unsigned int>::iterator it1 = temp1.begin(); it1!= temp1.end(); ++it1){
                            for(std::set<unsigned int>::iterator it2 = temp2.begin(); it2!= temp2.end(); ++it2){
                                if((*it1) == (*it2))
                                    match = true;
                            }
                            if(!match)
                                final.insert(*it1);

                            match = false;

                        }

                        temp1 = it->second.lineNumbers;
                        temp2 = final;

                        //Union with existing
                        for(std::set<unsigned int>::iterator it1 = temp1.begin(); it1!= temp1.end(); ++it1){
                            for(std::set<unsigned int>::iterator it2 = temp2.begin(); it2!= temp2.end(); ++it2){
                                if((*it1) == (*it2))
                                    match = true;
                            }
                            if(!match)
                                final.insert(*it1);

                            match = false;

                        }

                        */

                        SrcSliceData sliceData = SrcSliceData(deal.second.nameofidentifier, it->second.identifierType, final);
                        data.srcSliceMap.insert(std::make_pair(deal.second.nameofidentifier, sliceData));
                    }
                }

            }




        }


    protected:
        void * DataInner() const override {
            return new SrcSliceSet(data);
        }
    private:
        ExprPolicy exprPolicy;
        ExprPolicy::ExprDataSet exprData;
        DeclTypePolicy declTypePolicy;
        DeclTypePolicy::DeclTypeData declData;

        //This correct?
        void InitializeEventHandlers(){
            std::cerr<<"Enter"<<std::endl;
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
                NotifyAll(ctx);
                //data.clear();
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&declTypePolicy);
                NotifyAll(ctx);
                ///data.clear(); //Do we clear?
            };

            //Is this Needed??
            //closeEventMap[ParserState::archive] = [this](srcSAXEventContext& ctx){
            //    NotifyAll(ctx);
            //};
            std::cerr<<"Exit"<<std::endl;
        }

};
