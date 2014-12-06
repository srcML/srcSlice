#ifndef UTILITY_HPP
#define UTILITY_HPP

template <typename T> 
std::unordered_set<T> SetUnion(std::unordered_set<T> set1  , std::unordered_set<T> set2){
    for(typename std::unordered_set<T>::iterator itr = set2.begin(); itr != set2.end(); ++itr){
        set1.insert(*itr);
    }
    return set1;
}

std::vector<std::string> SplitLhsRhs(const std::string& str){
    std::vector<std::string> expr;
    expr.push_back(std::string());
    for(int i = 0; i<str.size(); ++i){
        if(str[i] == '='){
            expr.push_back(str.substr(i+1, str.size()-1));
            break;
        }else{
            expr[0]+=str[i];
        }
    }
    return expr;
}

/* Split function for splitting strings by tokens. Works on sets of tokens or just one token*/
std::vector<std::string> SplitOnTok(const std::string& str, const char* tok){
    std::size_t tokPos = str.find_first_of(tok);
    std::vector<std::string> result;
    std::size_t nextPos = 0, curPos = 0;
    while(curPos != std::string::npos){
        result.push_back(str.substr(nextPos, tokPos - nextPos));
        nextPos = tokPos+1;
        curPos = tokPos;
        tokPos = str.find_first_of(tok, nextPos);
    }
    return result;
}
#endif