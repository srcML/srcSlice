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
        auto sp2 = sysDict.globalMap.find(varName);
        if(sp2 != sysDict.globalMap.end()){
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
                sp->index = declIndex;
                sp->cfunctions.insert(std::make_pair(functionNameHash(nameOfCurrentClldFcn.top()), numArgs));
            }
        }
    }
}

void srcSliceHandler::GetParamType(){
    currentSliceProfile.variableType = currentParamType.first;
    unsigned int paramHash = paramTypeHash(currentParamType.first);
    functionTmplt.params.push_back(paramHash);
    functionTmplt.functionNumber += paramHash;
    sysDict.typeTable.insert(std::make_pair(paramHash, currentParamType.first));
    currentParamType.first.clear();
}

void srcSliceHandler::GetParamName(){
    currentSliceProfile.index = declIndex;
    currentSliceProfile.file = fileNumber;
    currentSliceProfile.function = functionTmplt.GetFunctionUID();
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
    unsigned int paramHash = paramTypeHash(currentParamType.first);
    functionTmplt.functionNumber += paramHash;
    functionTmplt.params.push_back(paramHash);
    sysDict.typeTable.insert(std::make_pair(paramHash, currentParamType.first));
}

/**
* GetDeclStmtData
* Knows proper constraints for obtaining DeclStmt type and name.
* creates a new slice profile and stores data about decle statement inside.
*/
void srcSliceHandler::GetDeclStmtData(){
    currentSliceProfile.index = declIndex;
    currentSliceProfile.file = fileNumber;
    currentSliceProfile.function = functionTmplt.GetFunctionUID();
    currentSliceProfile.variableName = currentDecl.first;
    currentSliceProfile.potentialAlias = potentialAlias;
    currentSliceProfile.isGlobal = inGlobalScope;
    if(!inGlobalScope){
        varIt = FunctionIt->second.insert(std::make_pair(currentDecl.first, std::move(currentSliceProfile))).first;
        varIt->second.def.insert(currentDecl.second);
    }else{ //TODO: Handle def use for globals
        sysDict.globalMap.insert(std::make_pair(currentDecl.first, std::move(currentSliceProfile)));
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
        SliceProfile* lhs = Find(lhsExprStmt.first);
        if(!lhs){
            currentSliceProfile.index = -1;
            currentSliceProfile.file = fileNumber;
            currentSliceProfile.function = functionTmplt.GetFunctionUID();
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
            sprIt->slines.insert(currentExprStmt.second);
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
    
    FileIt = sysDict.dictionary.find(f);
    if(FileIt == sysDict.dictionary.end()){
        std::cerr<<"FATAL ERROR";
        return;
    }
    
    FunctionIt = (FileIt)->second.begin();
    FunctionVarMap::iterator FunctionItEnd = (FileIt)->second.end();

    for(FunctionIt; FunctionIt != FunctionItEnd; ++FunctionIt){
        for(VarMap::iterator it = FunctionIt->second.begin(); it != FunctionIt->second.end(); ++it){
            if(it->second.visited == false){//std::unordered_set<NameLineNumberPair, NameLineNumberPairHash>::iterator - auto       
                for(auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF ){
                    unsigned int argumentIndex = itCF->second;
                    //std::cerr<<"caller: "<<itCF->first<<std::endl;
                    SliceProfile Spi = ArgumentProfile(itCF->first, argumentIndex);
                    SetUnion(it->second.slines, Spi.slines);
                    SetUnion(it->second.cfunctions, Spi.cfunctions);
                    SetUnion(it->second.aliases, Spi.aliases);
                    SetUnion(it->second.dvars, Spi.dvars);
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
SliceProfile srcSliceHandler::ArgumentProfile(unsigned int fname, unsigned int parameterIndex){
    SliceProfile Spi;
    auto funcIt = FileIt->second.find(fname);
    if(funcIt != FileIt->second.end()){
        VarMap::iterator v = funcIt->second.begin();    
        for(VarMap::iterator it = v; it != funcIt->second.end(); ++it){
            if (it->second.index == (parameterIndex)){
                if(it->second.visited == true){
                    Spi = it->second; 
                    return Spi;
                }else{//std::unordered_set<NameLineNumberPair, NameLineNumberPairHash>::iterator - auto
                    for(auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF ){
                        unsigned int newFunctionName = itCF->first;
                        unsigned int newParameterIndex = itCF->second; 
                        if(newFunctionName != fname){
                            //std::cerr<<"Now: "<<newFunctionName<<std::endl;
                            Spi = ArgumentProfile(newFunctionName, newParameterIndex);
                        }
                    }
                    it->second.visited = true;
                }
            }
        }
    }else{
        std::cerr<<"FATAL ERROR";
    }
    return Spi;
}