#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>

class DeclTypePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        struct DeclTypeData{
            DeclTypeData(): linenumber{0}, isConst{false}, isReference{false}, isPointer{false}, isStatic{false} {}
            void clear(){
                nameoftype.clear();
                nameofidentifier.clear();
                namespaces.clear();
                linenumber = -1;
                isConst = false;
                isReference = false;
                isPointer = false;
                isStatic = false;
            }
            std::string nameoftype;
            std::string nameofidentifier;
            std::vector<std::string> namespaces;
            int linenumber;
            bool isConst;
            bool isReference;
            bool isPointer;
            bool isStatic;
        };
        DeclTypeData data;
        ~DeclTypePolicy(){}
        DeclTypePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            InitializeEventHandlers();
        }
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {} //doesn't use other parsers
    protected:
        void * DataInner() const override {
            return new DeclTypeData(data);
        }
    private:
        std::string currentTypeName, currentDeclName, currentModifier, currentSpecifier;
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            openEventMap[ParserState::op] = [this](srcSAXEventContext& ctx){
                if(ctx.And({ParserState::type, ParserState::declstmt}) && ctx.Nor({ParserState::specifier, ParserState::modifier, ParserState::genericargumentlist})){
                    data.namespaces.push_back(ctx.currentToken);
                }
            };
            closeEventMap[ParserState::modifier] = [this](srcSAXEventContext& ctx){
                if(ctx.IsOpen(ParserState::declstmt)){
                    if(currentModifier == "*"){
                        data.isPointer = true;
                    }
                    if(currentModifier == "&"){
                        data.isReference = true;
                    }
                }
            };

            closeEventMap[ParserState::decl] = [this](srcSAXEventContext& ctx){
                if(ctx.And({ParserState::declstmt})){
                    data.linenumber = ctx.currentLineNumber;
                    data.nameofidentifier = currentDeclName;
                }
            };

            closeEventMap[ParserState::type] = [this](srcSAXEventContext& ctx){
                if(ctx.And({ParserState::declstmt})){
                    data.nameoftype = currentTypeName;
                }
            };

            closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext& ctx){
                //TODO: possibly, this if-statement is suppressing more than just unmarked whitespace. Investigate.
                if(!(ctx.currentToken.empty() || ctx.currentToken[0] == ' ')){
                    if(ctx.And({ParserState::name, ParserState::type, ParserState::decl, ParserState::declstmt}) && ctx.Nor({ParserState::specifier, ParserState::modifier, ParserState::genericargumentlist})){
                        currentTypeName = ctx.currentToken;
                    }
                    if(ctx.And({ParserState::name, ParserState::decl, ParserState::declstmt}) && 
                       ctx.Nor({ParserState::type, ParserState::index/*skip array portion*/, ParserState::argumentlist/*skip init list portion*/, ParserState::init, ParserState::specifier, ParserState::modifier})){
                        currentDeclName = ctx.currentToken;
                    }
                    if(ctx.And({ParserState::specifier, ParserState::decl, ParserState::declstmt})){
                        currentSpecifier = ctx.currentToken;
                    }
                    if(ctx.And({ParserState::modifier, ParserState::type, ParserState::declstmt})){
                        currentModifier = ctx.currentToken;
                    }
                }
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                NotifyAll(ctx);
                data.clear();
            };
            closeEventMap[ParserState::specifier] = [this](srcSAXEventContext& ctx){
                if(ctx.IsOpen(ParserState::declstmt)){
                    if(currentSpecifier == "const"){
                        data.isConst = true;
                    }
                    if(currentSpecifier == "static"){
                        data.isStatic = true;
                    }
                }
                currentSpecifier.clear();
            };

        }
};