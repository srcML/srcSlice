#ifndef SRCSLICEPOLICY
#define SRCSLICEPOLICY

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <FunctionSliceProfilePolicy.hpp>

class SrcSlicePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        /*This will contain the full map*/
        struct SliceProfileSet{
            SliceProfileSet(){}
        };

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            using namespace srcSAXEventDispatch;
        }

    protected:
        void * DataInner() const override {
            //export profile to listeners
            return new SliceProfileSet(data);
        }
    private:
        SliceProfileSet data;
        FunctionSliceProfilePolicy funcSliceProfilePolicy;
        FunctionSliceProfilePolicy::FunctionSliceProfileMap functionSliceData;
        int currentscopelevel;
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->AddListenerDispatch(&funcSliceProfilePolicy);
            };
            closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&funcSliceProfilePolicy);
            };
        }

};
#endif