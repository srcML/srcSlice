#ifndef SRCSLICEPOLICY
#define SRCSLICEPOLICY

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <FunctionSliceProfilePolicy.hpp>
#include <unordered_map>

class SrcSlicePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    public:
        SrcSlicePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners)
        {
            // making SSP a listener for FSPP
            funcSliceProfilePolicy.AddListener(this);
            InitializeEventHandlers();
        }

        /*This will contain the full map*/
        struct SliceProfileSet
        {
            SliceProfileSet(){}

            // map containing unique variable name key (possibly var name, line numb, func, file); tiers; return all and user can choose
            // and profile data (small maps from FuctionSliceProfilePolicy)
            std::unordered_map<std::string, FunctionSliceProfilePolicy::FunctionSliceProfileMap> varNameProf;

            void clear()
            {
                varNameProf.clear();

                /*
                for(auto it = varNameProf.begin(); it != varNameProf.end(); ++it)
                */
            }
        };

        // other policies notify that finished
        // passes back data that's casted into appropriate type into "fuctionSliceData"
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override 
        {
            using namespace srcSAXEventDispatch;
            functionSliceData = policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfileMap>();
        }

    protected:
        void *DataInner() const override 
        {
            // export profile to listeners
            return new SliceProfileSet(data);
        }
        
    private:
        SliceProfileSet data;
        FunctionSliceProfilePolicy funcSliceProfilePolicy;
        FunctionSliceProfilePolicy::FunctionSliceProfileMap *functionSliceData;
        int currentscopelevel;

        void InitializeEventHandlers()
        {
            using namespace srcSAXEventDispatch;
            // add policy to listener
            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->AddListenerDispatch(&funcSliceProfilePolicy);
            };

            closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->RemoveListenerDispatch(&funcSliceProfilePolicy);
            };
        }
};
#endif