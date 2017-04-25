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

        SrcSlicePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners)
        {
            // making SSP a listener for FSPP
            funcSliceProfilePolicy.AddListener(this);
            funcSigPol.AddListener(this);

            InitializeEventHandlers();
        }

        struct SliceProfileSet
        {
            SliceProfileSet(){};

            void clear()
            {
                varNameProf.clear();
            }

            std::unordered_map<std::string, FunctionSliceProfilePolicy::FunctionSliceProfile> varNameProf;
        };

        // other policies notify that finished
        // passes back data that's casted into appropriate type into "fuctionSliceData"
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override 
        {
            using namespace srcSAXEventDispatch;

            if(ctx.IsOpen(ParserState::functionblock))
            {
                funcSigDat = policy->Data<FunctionSignaturePolicy::SignatureData>();

                keyName = "";

                for(auto f : funcSigDat->parameters) // loop through vector of parameters
                {
                    keyName = keyName + f.nameofidentifier; // HOW TO MAKE SEPARATE ELEMENTS FOR EACH PARAMETER? 
                    // THIS ADDS THE NAMES OF ALL THE PARAMETERS ONTO THE END
                }
                keyName = funcSigDat->functionName + keyName; // function name then parameter names
            }
            else
            {
                functionSliceData = policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfileMap>();
                for(auto e : functionSliceData->dataset)
                {
                    keyName = keyName + funcSliceProfilePolicy.data.identifierName; // function names, parameter names, then identifier name

                    // keyName = FILENAME + keyName

                    data.varNameProf.insert(std::make_pair(keyName, e.second));
                }
            }
        }

        SliceProfileSet data;

        FunctionSliceProfilePolicy funcSliceProfilePolicy;
        FunctionSliceProfilePolicy::FunctionSliceProfileMap *functionSliceData;

        FunctionSignaturePolicy funcSigPol;
        FunctionSignaturePolicy::SignatureData *funcSigDat;

        std::string keyName;
 
    protected:
        void *DataInner() const override 
        {
            return new SliceProfileSet(data); // export profile to listeners
        }
        
    private:
       int currentscopelevel;

        void InitializeEventHandlers()
        {
            using namespace srcSAXEventDispatch;

            // add policy to listener
            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->AddListenerDispatch(&funcSliceProfilePolicy);
                ctx.dispatcher->AddListenerDispatch(&funcSigPol);
            };

            closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->RemoveListenerDispatch(&funcSliceProfilePolicy);
                ctx.dispatcher->RemoveListenerDispatch(&funcSigPol);
            };
        }
};
#endif