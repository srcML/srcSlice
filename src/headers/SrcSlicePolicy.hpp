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
                FunctionSliceProfilePolicy::FunctionSliceProfile funcSliceDat;

                for(auto f : funcSigDat->parameters) // loop through vector of parameters
                {
                    funcSliceDat.linenumber = f.linenumber;
                    funcSliceDat.scopelevel = 1; // PROBABLY NOT RIGHT
                    funcSliceDat.isConst = f.isConst;
                    funcSliceDat.isReference = f.isReference;
                    funcSliceDat.isPointer = f.isPointer;
                    funcSliceDat.isStatic = f.isStatic;
                    funcSliceDat.identifierName = f.nameofidentifier;
                    funcSliceDat.identifierType = f.nameoftype;
                    funcSliceDat.def.insert(f.linenumber);
                
                    funcName = funcSigDat->functionName;

                    keyName = keyName + f.nameofidentifier; 
                    keyName = funcName + keyName; // function name then parameter name

                    keyName = ctx.currentFilePath + keyName;

                    data.varNameProf.insert(std::make_pair(keyName, funcSliceDat));

                    std::cout << ctx.currentFilePath << std::endl;

                    keyName.clear();

                }
            }
            else
            {
                functionSliceData = policy->Data<FunctionSliceProfilePolicy::FunctionSliceProfileMap>();

                for(auto e : functionSliceData->dataset)
                {
                    keyName = keyName + funcName + e.second.identifierName; // function name then identifier name

                    keyName = ctx.currentFilePath + keyName;

                    data.varNameProf.insert(std::make_pair(keyName, e.second));

                    std::cout << ctx.currentFilePath << std::endl;

                    keyName.clear();
                }
            }
        }

        SliceProfileSet data;

        FunctionSliceProfilePolicy funcSliceProfilePolicy;
        FunctionSliceProfilePolicy::FunctionSliceProfileMap *functionSliceData;

        FunctionSignaturePolicy funcSigPol;
        FunctionSignaturePolicy::SignatureData *funcSigDat;

        std::string keyName = "";
        std::string funcName = "";
 
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

            closeEventMap[ParserState::archive] = [this](srcSAXEventContext& ctx)
            {
                NotifyAll(ctx);
            };
        }
};
#endif