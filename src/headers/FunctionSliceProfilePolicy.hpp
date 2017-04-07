#ifndef FUNCTIONSLICEPOLICY
#define FUNCTIONSLICEPOLICY

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>
#include <stack>

using std::stack;

// problem with variables with same name in different scopes (Use a stack, every time change of scope push onto stack)
// possibly add line numbers to names

class FunctionSliceProfilePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    public:
        struct FunctionSliceProfile
        {
            FunctionSliceProfile(){}

            FunctionSliceProfile(std::string name, std::string type, std::set<unsigned int> lines)
            {
                identifierName = name + std::to_string(linenumber); // name of var and line number
                identifierType = type;
                scopelevel = -1;
            }

            void clear() // the rest in srcSlicePolicy
            {
                identifierName.clear();
                identifierType.clear();

                linenumber = -1;
                scopelevel = -1;

                isConst = false;
                isReference = false;
                isPointer = false;
                isStatic = false;
            }

            std::vector<std::string> namespaces;
            
            int linenumber;
            int scopelevel;

            bool isConst;
            bool isReference;
            bool isPointer;
            bool isStatic;

            std::string identifierName;
            std::string identifierType;

            std::set<unsigned int> def;
            std::set<unsigned int> use;
        };

        struct FunctionSliceProfileMap // the mapping of var name string and the profile
        {
           	FunctionSliceProfileMap() = default;

           	FunctionSliceProfileMap(std::map<std::string, FunctionSliceProfile> dat)
           	{
            	dataset = dat;
           	}
           	
           	void clear()
           	{
            	dataset.clear();
           	}
           	
           	std::map<std::string, FunctionSliceProfile> dataset;
        };
        
        FunctionSliceProfileMap profileset;

        FunctionSliceProfile data;

        ~FunctionSliceProfilePolicy(){}

        FunctionSliceProfilePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners)
        {
            currentscopelevel = 0;
            // adds these as listeners
            exprPolicy.AddListener(this); 
            declTypePolicy.AddListener(this);
            InitializeEventHandlers(); // handles the events
        }

        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override 
        {
            using namespace srcSAXEventDispatch;
            
            // "declestmt" is open and "exprestmt" is closed
            if(ctx.IsOpen(ParserState::declstmt) && ctx.IsClosed(ParserState::exprstmt))
            {
                std::cerr<<"Call decl"<<std::endl;
                declData = *policy->Data<DeclTypePolicy::DeclTypeData>();

                // generates the profile
                auto it = profileset.dataset.find(declData.nameofidentifier);
                if(it != profileset.dataset.end())
                {
                    it->second.identifierName = declData.nameofidentifier;
                    it->second.identifierType = declData.nameoftype;
                    it->second.isConst = declData.isConst;
                    it->second.isReference = declData.isReference;
                    it->second.isPointer = declData.isPointer;
                    it->second.isStatic = declData.isStatic;
                }
                else // inserts into map if it's not there
                {
                    data.identifierName = declData.nameofidentifier;
                    data.identifierType = declData.nameoftype;
                    data.isConst = declData.isConst;
                    data.isReference = declData.isReference;
                    data.isPointer = declData.isPointer;
                    data.isStatic = declData.isStatic;
                    profileset.dataset.insert(std::make_pair(declData.nameofidentifier, data));
                    std::cout << profileset.dataset.size();
                }
            }
            // "declestmt" is closed and "exprestmt" is open
            else if(ctx.IsOpen(ParserState::exprstmt) && ctx.IsClosed(ParserState::declstmt))
            {
                exprData = *policy->Data<ExprPolicy::ExprDataSet>();
                for(auto var : exprData.dataset)
                {
                    auto it = profileset.dataset.find(var.first);
                    if(it != profileset.dataset.end()) // if "it" hadn't reached the end, meaning the string was found
                    {
                        it->second.def = var.second.def;
                        it->second.use = var.second.use;
                    }
                    else // if the string for the var name wasn't found
                        std::cerr<<"Couldn't find identifier named: "<<var.first<<std::endl;
                }
            }
        }

    protected:
        void *DataInner() const override 
        {
            // export profile to listeners
            return new FunctionSliceProfileMap(profileset);
        }
    private:
        ExprPolicy exprPolicy;
        ExprPolicy::ExprDataSet exprData;

        DeclTypePolicy declTypePolicy;
        DeclTypePolicy::DeclTypeData declData;

        int currentscopelevel;

        void InitializeEventHandlers()
        {
            using namespace srcSAXEventDispatch;

            // opening the EventMap
            openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx) 
            {
                ctx.dispatcher->AddListenerDispatch(&declTypePolicy);
            };
            openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx) 
            {
                ctx.dispatcher->AddListenerDispatch(&exprPolicy);
            };
            openEventMap[ParserState::block] = [this](srcSAXEventContext& ctx) 
            {
                ++currentscopelevel;
            };

            // closing the EventMap
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->RemoveListenerDispatch(&declTypePolicy);
            };
            closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx)
            {
                ctx.dispatcher->RemoveListenerDispatch(&exprPolicy);
            };
            closeEventMap[ParserState::block] = [this](srcSAXEventContext& ctx)
            {
              	--currentscopelevel;  
            };
            closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx)
            {
                NotifyAll(ctx);
            };
        }
};
#endif