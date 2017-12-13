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
            std::set<std::string> dvars;
            std::set<std::string> pointers;

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
                //std::cerr << "declstmt is open && exprstmt is close\n";
                //std::cerr<<"Call decl"<<std::endl;
                declData = *policy->Data<DeclTypePolicy::DeclTypeData>();

                // generates the profile
                // TODO: overwrites current profile; should store both/unique key
                auto it = profileset.dataset.find(declData.nameofidentifier);
                if(it != profileset.dataset.end())
                {
                    //std::cerr << "found " << it->second.identifierName << "\n";
                    //std::cerr << "overwriting with " << declData.nameofidentifier << "\n";
                    it->second.identifierName = declData.nameofidentifier;
                    it->second.identifierType = declData.nameoftype;
                    it->second.isConst = declData.isConst;
                    it->second.isReference = declData.isReference;
                    it->second.isPointer = declData.isPointer;
                    it->second.isStatic = declData.isStatic;
                    it->second.linenumber = declData.linenumber;
                    it->second.def.insert(declData.linenumber);
                }
                else // inserts into map if it's not there
                {
                    //need to clear out def if new var
                    data.def.clear();

                    data.identifierName = declData.nameofidentifier;
                    data.identifierType = declData.nameoftype;
                    data.isConst = declData.isConst;
                    data.isReference = declData.isReference;
                    data.isPointer = declData.isPointer;
                    data.isStatic = declData.isStatic;
                    data.linenumber = declData.linenumber;
                    data.def.insert(declData.linenumber);

                    // need to clear out pointers if new var
                    data.pointers.clear();
                    for(auto pnt : declData.pointers) {
                        data.pointers.insert(pnt);
                    }

                    profileset.dataset.insert(std::make_pair(declData.nameofidentifier, data));

                    // std::cout << profileset.dataset.size();
                    // uses of other variables

                    for (auto varname : declData.exprvars) {
                      auto it = profileset.dataset.find(varname);
                      if (it != profileset.dataset.end()) {
                        // varname was used at linenumber, varname has impact on nameofidentifier
                        it->second.use.insert(declData.linenumber);
                        it->second.dvars.insert(declData.nameofidentifier);
                      }
                    }




                }
            
            }
            // "declestmt" is closed and "exprestmt" is open
            else if (ctx.IsOpen(ParserState::exprstmt) && ctx.IsClosed(ParserState::declstmt))
            {
                //std::cerr << "declstmt is closed && exprstmt is open\n";
                exprData = *policy->Data<ExprPolicy::ExprDataSet>();
                for(auto var : exprData.dataset)
                {
                    auto it = profileset.dataset.find(var.first);
                    //std::cout << profileset.dataset.size() << "\n";
                    if(it != profileset.dataset.end()) // if "it" hadn't reached the end, meaning the string was found
                    {
                        //need to insert into def/use, not overwrite it
                       // std::cout << "profileset dataset end\n";
                        for(auto i : var.second.def)
                            it->second.def.insert(i);
                        for(auto i : var.second.use) {
                            it->second.use.insert(i);
                        }

                    }
                }

                /* */
                for(auto var : exprData.dataset) {
                    auto it = profileset.dataset.find(var.first);
                    if(it != profileset.dataset.end()) {
                        if(it->second.isPointer) {
                            for(auto s : profileset.dataset) {
                                for(auto def : it->second.def) {
                                    auto it2 = s.second.use.find(def);
                                    if(it2 != s.second.use.end()) {
                                        it->second.pointers.insert(s.second.identifierName);
                                    }
                                }
                            }
                        }
                    }
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