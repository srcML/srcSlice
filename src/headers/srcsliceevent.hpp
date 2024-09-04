#ifndef SRCSLICEEVENT
#define SRCSLICEEVENT

#include <srcsliceprofile.hpp>
#include <srcslicehandler.hpp>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ExprPolicy.hpp>
#include <InitPolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <FunctionCallPolicy.hpp>
#include <fstream>
#include <ReturnPolicy.hpp>
#include <ConditionalPolicy.hpp>
#include <sstream>

class SrcSliceEvent
        : public srcSAXEventDispatch::EventListener,
          public srcSAXEventDispatch::PolicyDispatcher,
          public srcSAXEventDispatch::PolicyListener {
public:
    ~SrcSliceEvent() {};
    SrcSliceEvent(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {})
            : srcSAXEventDispatch::PolicyDispatcher(listeners) {
        // making SSP a listener for FSPP
        // InitializeEventHandlers();
    }

protected:
    void *DataInner() const override {
        return (void *) 0; // export profile to listeners
    }

private:
    DeclTypePolicy declPolicy;
    DeclData decldata;

    DeclData paramdata;

    InitPolicy initPolicy;
    InitPolicy::InitDataSet initDataSet;

    ExprPolicy::ExprDataSet exprDataSet;
    ExprPolicy exprPolicy;

    CallPolicy callPolicy;
    CallPolicy::CallData calldata;
    
    ReturnPolicy returnPolicy;
    std::unordered_map<std::string, std::vector<unsigned int>> funcDefMap;
    
    ConditionalPolicy conditionalPolicy;

    FunctionSignaturePolicy functionPolicy;
    SignatureData functionsigdata;
    std::unordered_map<std::string, SignatureData> functionSigMap;
    std::string currentExprName;
    std::vector<std::string> declDvars;

    std::vector<std::pair<int, int>> loopdata;
    std::vector<std::pair<int, int>> ifdata;
    std::vector<std::pair<int, int>> elsedata;
    std::vector<std::pair<std::string, unsigned int>> initDeclData;
    std::map<std::string, unsigned int> overloadFunctionCount;
    std::set<unsigned int> functionCallList;
    int startLine;
    int endLine;

    std::string currentName;

    void InitializeEventHandlers() {
        using namespace srcSAXEventDispatch;

        openEventMap[ParserState::decl] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&declPolicy);
        };
        closeEventMap[ParserState::decl] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListener(&declPolicy);
            declPolicy.Finalize(ctx);
            currentName.clear();
        };

        openEventMap[ParserState::name] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&returnPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::name] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&returnPolicy);
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&exprPolicy);
        };
        closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&exprPolicy);
            currentName.clear();
        };
        
        openEventMap[ParserState::expr] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::expr] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::switchstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
            conditionalPolicy.EditDepth(1);
        };
        closeEventMap[ParserState::switchstmt] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
            conditionalPolicy.EditDepth(-1);
        };
        
        openEventMap[ParserState::switchcase] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::switchcase] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::call] = [this](srcSAXEventContext &ctx) {
            //don't want multiple callPolicy parsers running
            if (ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                ctx.dispatcher->AddListenerDispatch(&callPolicy);
            }
        };
        closeEventMap[ParserState::call] = [this](srcSAXEventContext &ctx) {
            if (ctx.NumCurrentlyOpen(ParserState::call) < 2) {
                ctx.dispatcher->RemoveListenerDispatch(&callPolicy);
            }
        };

        openEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&initPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };
        closeEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&initPolicy);
            ctx.dispatcher->RemoveListener(&conditionalPolicy);
        };

        openEventMap[ParserState::forstmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::forstmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            loopdata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::whilestmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::whilestmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            loopdata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::ifstmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::ifstmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            ifdata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::elseif] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::elseif] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            elsedata.push_back(std::make_pair(startLine, endLine));
        };

        openEventMap[ParserState::elsestmt] = [this](srcSAXEventContext &ctx) {
            startLine = ctx.currentLineNumber;
        };
        closeEventMap[ParserState::elsestmt] = [this](srcSAXEventContext &ctx) {
            endLine = ctx.currentLineNumber;
            elsedata.push_back(std::make_pair(startLine, endLine));
        };

        closeEventMap[ParserState::op] = [this](srcSAXEventContext &ctx) {
            bool isAssignmentOperator = (ctx.currentToken == "=" || ctx.currentToken == "+=" ||
                                        ctx.currentToken == "-=" || ctx.currentToken == "*=" ||
                                        ctx.currentToken == "/=" || ctx.currentToken == "%=");
            if (isAssignmentOperator) {
                currentName = currentExprName;
            }
        };

        openEventMap[ParserState::function] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->AddListenerDispatch(&functionPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };

        openEventMap[ParserState::functionblock] = [this](srcSAXEventContext &ctx) {
            ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
            ctx.dispatcher->AddListenerDispatch(&conditionalPolicy);
        };

        closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext &ctx) {
            //TODO: possibly, this if-statement is suppressing more than just unmarked whitespace. Investigate.
            if (!(ctx.currentToken.empty() || ctx.currentToken == " ")) {
                if (ctx.And({ParserState::name, ParserState::expr, ParserState::exprstmt}) &&
                    ctx.Nor({ParserState::specifier, ParserState::modifier, ParserState::op})) {
                    currentExprName = ctx.currentToken;
                }
            }
        };
    } // End of Initialize Event Handlers
};

#endif