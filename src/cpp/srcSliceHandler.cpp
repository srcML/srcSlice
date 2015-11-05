/**
 * @file srcSliceHandler.cpp
 *
 * @copyright Copyright (C) 2013-2014 SDML (www.srcML.org)
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <srcSliceHandler.hpp>
#include <Utility.hpp>
/**
 *Find
 *@param varName - Name of the variable whose slice profile we want
 *@return Pointer to Slice Profile or Null
 *Find function takes a variable name and searches first the local table to the function it was in
 *and if it's not found there, then it searches the global table to see if it's in there.
 *If neither has the variable, it returns null otherwise returns a pointer to the slice profile
 */
SliceProfile* srcSliceHandler::Find(const std::string& varName){    
    auto sp = FunctionIt->second.find(varName);
    if(sp != FunctionIt->second.end()){
        return &(sp->second);
    }else{ //check global map
        auto sp2 = sysDict->globalMap.find(varName);
        if(sp2 != sysDict->globalMap.end()){
            return &(sp2->second);
        }
    }
    return nullptr;
}
/**
 *ProcessConstructorDecl
 *Processes decls of the form object(arg,arg)
 */
void srcSliceHandler::ProcessConstructorDecl(){
    auto sp = Find(currentDeclArg.first);
    if(sp){
        sp->dvars.insert(varIt->second.variableName);
    }
}

/*
*ProcessDeclStmt
*Takes rhs of decl_stmt and processes it. Split by tokens and then throw against the map for an answer.
*corner case at new operator because new makes an object even if its argument is an alias.
*/
void srcSliceHandler::ProcessDeclStmt(){
    auto sp = Find(currentDeclInit.first);
    if(sawnew){sawnew = false;}
    if(sp){
        varIt->second.slines.insert(currentDeclInit.second); //varIt is lhs
        sp->use.insert(currentDeclInit.second);
        if(varIt->second.potentialAlias && !sawnew){ //new operator of the form int i = new int(tmp); screws around with aliasing
            varIt->second.lastInsertedAlias = varIt->second.aliases.insert(sp->variableName).first;
        }else{
            sp->dvars.insert(varIt->second.variableName);
            sp->use.insert(currentDeclInit.second);
        }
    }
    currentDeclInit.first.clear();
}

/*
* GetCallData
* Knows the proper constrains for obtaining the name of arguments of currently called function
* It stores data about those variables if it can find a slice profile entry for them.
* Essentially, update the slice profile of the argument to reflect new data.
*/
void srcSliceHandler::GetCallData(){
    //Get function arguments
    if(triggerField[argument_list] && triggerField[argument] && 
        triggerField[expr] && triggerField[name] && !nameOfCurrentClldFcn.empty()){
        if(!callArgData.empty()){
            auto sp = Find(callArgData.top().first); //check to find sp for the variable being called on fcn
            if(sp){
                sp->slines.insert(callArgData.top().second);
                sp->use.insert(callArgData.top().second);
                sp->index = numArgs;
                sp->cfunctions.insert(std::make_pair(nameOfCurrentClldFcn.top(), numArgs));
            }
        }
    }
}

void srcSliceHandler::GetParamType(){
    currentSliceProfile.variableType = currentParamType.first;
    currentParamType.first.clear();
}

void srcSliceHandler::GetParamName(){
    currentSliceProfile.index = declIndex;
    currentSliceProfile.file = fileName;
    currentSliceProfile.function = functionTmplt.functionName;
    currentSliceProfile.variableName = currentParam.first;
    currentSliceProfile.potentialAlias = potentialAlias;
    currentSliceProfile.isGlobal = inGlobalScope;

    //std::cerr<<"PARA: "<<currentParam.first<<std::endl;
    varIt = FunctionIt->second.insert(std::make_pair(currentParam.first, std::move(currentSliceProfile))).first;
    varIt->second.def.insert(currentParam.second);

    currentParam.first.clear();
}

/**
* GetFunctionData
* Knows proper constraints for obtaining function's return type, name, and arguments. Stores all of this in
* functionTmplt.
*/
void srcSliceHandler::GetFunctionData(){
    //Get function name
    if(isConstructor){
        std::stringstream ststrm;
        ststrm<<constructorNum;
        currentFunctionBody.first+=ststrm.str(); //number the constructor. Find a better way than stringstreams someday.
    }
    functionTmplt.functionLineNumber = currentFunctionBody.second;
    functionTmplt.functionName = currentFunctionBody.first; //give me the hash num for this name.
    currentFunctionBody.first.clear();
}

void srcSliceHandler::GetFunctionDeclData(){
    functionTmplt.params.push_back(currentParamType.first);
}

/**
* GetDeclStmtData
* Knows proper constraints for obtaining DeclStmt type and name.
* creates a new slice profile and stores data about decle statement inside.
*/
void srcSliceHandler::GetDeclStmtData(){
    currentSliceProfile.index = declIndex;
    currentSliceProfile.file = fileName;
    currentSliceProfile.function = functionTmplt.functionName;
    currentSliceProfile.variableName = currentDecl.first;
    currentSliceProfile.potentialAlias = potentialAlias;
    currentSliceProfile.isGlobal = inGlobalScope;
    if(!inGlobalScope){
        varIt = FunctionIt->second.insert(std::make_pair(currentDecl.first, std::move(currentSliceProfile))).first;
        varIt->second.def.insert(currentDecl.second);
    }else{ //TODO: Handle def use for globals
        sysDict->globalMap.insert(std::make_pair(currentDecl.first, std::move(currentSliceProfile)));
    }
}

/**
 * ProcessExprStmt
 * Get entire expression statement and then process by first splitting to lhs and rhs. Process the lhs
 * by saving its slines if it can be found in the map. After lhs is processed, keep track of it and then
 * process the rhs for any aliases, dvars, or function calls.
 */
void srcSliceHandler::ProcessExprStmtPreAssign(){
    if(!lhsExprStmt.first.empty()){
        //std::cerr<<"lhs: "<<lhsExprStmt.first<<" "<<lhsExprStmt.second<<std::endl;
        SliceProfile* lhs = Find(lhsExprStmt.first);
        if(!lhs){
            currentSliceProfile.index = -1;
            currentSliceProfile.file = fileName;
            currentSliceProfile.function = functionTmplt.functionName;
            currentSliceProfile.variableName = lhsExprStmt.first;
            currentSliceProfile.potentialAlias = false;
            currentSliceProfile.isGlobal = inGlobalScope;
            
            varIt = FunctionIt->second.insert(std::make_pair(lhsExprStmt.first, std::move(currentSliceProfile))).first;
            varIt->second.def.insert(lhsExprStmt.second);
        }else{
            lhs->def.insert(lhsExprStmt.second);
        }        
    }
}

/**
 * ProcessExprStmt
 * Get entire expression statement and then process by first splitting to lhs and rhs. Process the lhs
 * by saving its slines if it can be found in the map. After lhs is processed, keep track of it and then
 * process the rhs for any aliases, dvars, or function calls.
 */
void srcSliceHandler::ProcessExprStmtPostAssign(){
    SliceProfile* lhs = Find(lhsName);
    if(!lhs){return;}

    auto sprIt = Find(currentExprStmt.first);//find the sp for the rhs
    if(sprIt){ //lvalue depends on this rvalue
        if(lhs->variableName != sprIt->variableName){    
            if(!lhs->potentialAlias || dereferenced){ //It is not potentially a reference and if it is, it must not have been dereferenced
                sprIt->dvars.insert(lhs->variableName); //it's not an alias so it's a dvar
            }else{//it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                lhs->lastInsertedAlias = lhs->aliases.insert(sprIt->variableName).first;
            }
            sprIt->use.insert(currentExprStmt.second);           
            if(sprIt->potentialAlias && !dereferenced){//Union things together. If this was an alias of anoter thing, update the other thing
                if(!sprIt->aliases.empty()){
                    auto spaIt = FunctionIt->second.find(*(sprIt->lastInsertedAlias)); //problem  because last alias is an iterator and can reference things in other functions. Maybe make into a pointer. Figure out why I need it.
                    if(spaIt != FunctionIt->second.end()){
                        spaIt->second.dvars.insert(lhs->variableName);
                        spaIt->second.use.insert(currentExprStmt.second);  
                        spaIt->second.slines.insert(currentExprStmt.second);
                    }
                }
            }
        }
    }
}
void srcSliceHandler::ProcessExprStmtNoAssign(){
    for(NameLineNumberPair name : useExprStack){
        //std::cerr<<"Name: "<<name.first<<" "<<name.second<<std::endl;
        SliceProfile* useProfile = Find(name.first);
        if(!useProfile){continue;}
        else{//it's running on the same word as the other two exprstmt functions
            useProfile->use.insert(name.second);
        }        
    }

}
void srcSliceHandler::ProcessDeclCtor(){
    SliceProfile* lhs = Find(currentDecl.first);
    if(!lhs){
        return;
    }else{
        lhs->use.insert(currentDecl.second);
    }
    SliceProfile* rhs = Find(currentDeclCtor.first);
    if(rhs){
        rhs->dvars.insert(lhs->variableName);
        rhs->use.insert(currentDecl.second);
    }
    
}

/*
 *ComputeInterprocedural
 *@param f- name of the file
 *No return value
 */
void srcSliceHandler::ComputeInterprocedural(const std::string& f){   
    FileIt = sysDict->ffvMap.find(f);
    if(FileIt == sysDict->ffvMap.end()){
        std::cerr<<"CAN'T FIND FILE"<<std::endl;
        return;
    }
    
    FunctionIt = (FileIt)->second.begin();
    FunctionVarMap::iterator FunctionItEnd = FileIt->second.end();

    for(FunctionIt; FunctionIt != FunctionItEnd; ++FunctionIt){
        for(VarMap::iterator it = FunctionIt->second.begin(); it != FunctionIt->second.end(); ++it){
            if(it->second.visited == false){//std::unordered_set<NameLineNumberPair, NameLineNumberPairHash>::iterator - auto       
                for(auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF ){
                    unsigned int argumentIndex = itCF->second;
                    SliceProfile Spi = ArgumentProfile(itCF->first, argumentIndex, it);
                    SetUnion(it->second.use, Spi.def);
                    SetUnion(it->second.use, Spi.use);
                    SetUnion(it->second.cfunctions, Spi.cfunctions);
                    SetUnion(it->second.dvars, Spi.dvars);
                    //SetUnion(it->second.aliases, Spi.aliases); //I suspect this is wrong, but I'll leave it here in case I'm wrong.
                }
                it->second.visited = true;
            } 
        }
    }  
}

/*
 *ArgumentProfile
 *@param functIt- iterator to the FunctionVarMap, parameterIndex- index of the parameter 
 *@Return SliceProfile of the variable
*/
SliceProfile srcSliceHandler::ArgumentProfile(std::string fname, unsigned int parameterIndex, VarMap::iterator vIt){ //TODO varIt is a hack here. Fix. We shouldn't need to pass an extra param to do this.
    SliceProfile Spi;
    auto funcIt = FileIt->second.find(fname);
    if(funcIt != FileIt->second.end()){
        VarMap::iterator v = funcIt->second.begin();    
        for(VarMap::iterator it = v; it != funcIt->second.end(); ++it){
            //std::cerr<<fname<<" "<<it->second.variableName<<" "<<it->second.index<<" "<<vIt->second.variableName<<" "<<parameterIndex<<" "<<it->second.potentialAlias<<std::endl;
            if (it->second.index == (parameterIndex)){
                if(it->second.visited == true){
                    if(it->second.potentialAlias){
                        it->second.aliases.insert(vIt->second.variableName);
                    }
                    return it->second;
                }else{
                    for(auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF ){
                        std::string newFunctionName = itCF->first;
                        unsigned int newParameterIndex = itCF->second; 
                        if(newFunctionName != fname){
                            Spi = ArgumentProfile(newFunctionName, newParameterIndex, it);
                            SetUnion(it->second.use, Spi.def);
                            SetUnion(it->second.use, Spi.use);
                            SetUnion(it->second.cfunctions, Spi.cfunctions);
                            SetUnion(it->second.dvars, Spi.dvars);
                            SetUnion(it->second.aliases, Spi.aliases);
                        }
                    }
                    if(it->second.potentialAlias){
                        it->second.aliases.insert(vIt->second.variableName);
                    }
                    it->second.visited = true;
                    return it->second;
                }
            }
        }
    }else{
        std::cerr<<"ERROR IN ARGUMENT PROFILE"<<std::endl;
    }
    return Spi;
}