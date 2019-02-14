#ifndef SRCSLICEPOLICY
#define SRCSLICEPOLICY

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <FunctionSliceProfilePolicy.hpp>
#include <unordered_map>
#include <FunctionSignaturePolicy.hpp>

class SrcSlicePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    public:
        ~SrcSlicePolicy(){};

        SrcSlicePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){
            // making SSP a listener for FSPP
 
            InitializeEventHandlers();
        }
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
            using namespace srcSAXEventDispatch;
        }
        void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx) 
    
    protected:
        void *DataInner() const override {
            return new SliceProfileSet(data); // export profile to listeners
        }
        
    private:
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;

            // add policy to listener
/*            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->AddListenerDispatch(&funcSliceProfilePolicy);
                ctx.dispatcher->AddListenerDispatch(&funcSigPol);
            };*/
        }
};
#endif