#ifndef SRC_SLICE_PARSE
#define SRC_SLICE_PARSE

#include <srcsliceprofile.hpp>

typedef std::unordered_map<std::string, std::vector<SliceProfile>>::iterator SliceProfileIterator;
typedef std::unordered_map<std::string, std::vector<SliceProfile>> SliceProfileMap;

namespace ExprParse {
    enum class TokenType {
        NONE,
        NAME,
        OPERATOR,
        LITERAL
    };
    struct Token {
        Token() {
            token = prevToken = "";
            type = TokenType::NONE;
        }
        std::string token;
        std::string prevToken;
        TokenType type;
    };

    struct ExprCtx {
        ExprCtx(SliceProfileMap& profileMap, std::vector<std::string>* LStack) {
            lhsStackPtr = LStack;
            spi = profileMap.end();
        }

        void resetDereferenceCtx() {
            if (dereferenced) dereferenced = false;
            if (dLength > 0) dLength = 0;
        };

        Token lastToken; // used to compute context
        bool dereferenced = false;
        int dLength = 0;
    
        bool addressOf = false;
        bool prefixed = false;
    
        bool cppOutput = false;
        bool cppInput = false;
    
        SlicePosition namePos; // position of the name element within an expression
    
        // slice profile collection based off recent_name
        SliceProfileIterator spi;
        
        // track first name token within a given expression
        srcDispatch::NameData* firstNameData = nullptr;

        std::vector<std::string>* lhsStackPtr = nullptr;
    };

    // Returns a numeric id based on the type of C++ IO operation is within given NameData
    // -1 => None | 0 => cout | 1 => cerr | 2 => cin
    int IsIO(const SliceCtx& sctx, const srcDispatch::NameData* nameDataPtr = nullptr);

    /**
     * Find the root name within a complex name
     * 
     * arr[2] => arr
     * obj_ptr->attribute => obj_ptr
     * obj.name => obj
     */

    std::string FindName(const std::vector<srcDispatch::DeltaElement<std::any>>& names, SlicePosition& namePos);

    // capture this to access the member variable profileMap to avoid a capture-all
    void pushDvar(SliceProfileMap& profileMap, std::string lhsName, std::string rhsName, SlicePosition& rhsPos);

    void pushAlias(SliceProfileMap& profileMap, std::string lhsName, std::string rhsName, SlicePosition& rhsPos);

    void pushUse(SliceProfileMap& profileMap, SliceProfileIterator spi, SlicePosition& pos);

    void popUse(SliceProfileMap& profileMap, SliceProfileIterator spi, SlicePosition& pos);
    
    void pushDef(SliceProfileMap& profileMap, SliceProfileIterator spi, SlicePosition& pos);
    void popDef(SliceProfileMap& profileMap, SliceProfileIterator spi, SlicePosition& pos);

    void updateLHS(SliceProfileMap& profileMap, ExprCtx& ectx, const std::string& target, int i = -1, int L = -1);
};

#endif