#ifndef UTILITY_HPP
#define UTILITY_HPP
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>

enum PrimTypes{INT, UINT, SIINT, SHINT, SISINT, USISINT, LINT, SLINT, USILINT, SILINT, LDOUBLE, WCHAR, CHAR, UCHAR, SCHAR, VOID, FLOAT, BOOL, DOUBLE, SIZE_T};

template <typename T> 
void SetUnion(std::unordered_set<T>& set1, std::unordered_set<T> set2){
    if(set1.empty() || set2.empty()){
        return;
    }
    for(typename std::unordered_set<T>::iterator itr = set2.begin(); itr != set2.end(); ++itr){
        set1.insert(*itr);
    }
}

template <typename T, typename U>
void SetUnion(std::unordered_set<T, U>& set1, std::unordered_set<T, U>& set2){
    if(set1.empty() || set2.empty()){
        return;
    }
    for(typename std::unordered_set<T, U>::iterator itr = set2.begin(); itr != set2.end(); ++itr){
        set1.insert(*itr);
    }
}


#endif



/*
static const std::unordered_map<std::string, PrimTypes> cppPrimTypes{
{"int", INT},
{"unsigned int", UINT},
{"signed int", SIINT},
{"short int", SHINT},
{"signed short int", SISINT},
{"unsigned short int", USISINT},
{"long int", LINT},
{"signed long int", SILINT},
{"unsigned long int", USILINT},
{"float", FLOAT},
{"double", DOUBLE},
{"long double", LDOUBLE},
{"wchar_t", WCHAR},
{"char", CHAR},
{"unsigned char", UCHAR},
{"signed char", SCHAR},
{"void", VOID},
{"float", FLOAT},
{"bool", BOOL},
{"double", DOUBLE},
{"size_t", SIZE_T}};
static const std::unordered_set<std::string> strToPrimTypes = {"int", "unsigned int", "signed int", "short int", 
                                                          "signed short int", "usigned short int", "long int", 
                                                          "signed long int", "unsigned long int", "float", 
                                                          "double", "long double", "wchar_t", "char", "unsigned char",
                                                          "signed char", "void", "float", "bool", "double", "size_t"};*/