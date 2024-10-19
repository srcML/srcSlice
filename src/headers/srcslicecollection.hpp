#ifndef SRCSLICECOLLETION
#define SRCSLICECOLLETION

#include <exception>
#include <unordered_map>
#include <set>
#include <memory>
#include <any>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <FunctionPolicySingleEvent.hpp>
#include <ClassPolicySingleEvent.hpp>
#include <UnitPolicySingleEvent.hpp>

struct VariableData{
    VariableData(){}
    VariableData(std::shared_ptr<ExpressionElement> elem){ lhsElem = elem; }

    // Basic clean up of this struct to allow simple re-purposing
    void clear(){
        lhsElem = nullptr;
        rhsElems.clear();
        lhs = false;
        uses.clear();
        definitions.clear();
    }

    // Name of an expression variable that may be a LHS/RHS var
    // potentially have none to many RHS variables the LHS uses
    std::string GetNameOfIdentifier() const {
        return lhsElem->name->name;
    }

    std::shared_ptr<ExpressionElement> lhsElem;
    std::vector<VariableData> rhsElems;

    bool lhs = false;
    std::set<unsigned int> uses;
    std::set<unsigned int> definitions;
};

#endif