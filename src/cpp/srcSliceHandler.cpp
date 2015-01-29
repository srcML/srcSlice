#include <srcSliceHandler.hpp>
#include <Utility.hpp>

/**
 * Find
 * @param varName - Name of the variable whose slice profile we want
 * @return Pointer to Slice Profile or Null
 * Find function takes a variable name and searches first the local table to the function it was in
 * and if it's not found there, then it searches the global table to see if it's in there.
 * If neither has the variable, it returns null otherwise returns a pointer to the slice profile
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
* ProcessDeclStmt
* Takes rhs of decl_stmt and processes it. Split by tokens and then throw against the map for an answer.
* corner case at new operator because new makes an object even if its argument is an alias.
*/
void srcSliceHandler::ProcessDeclStmt(){
    bool seenNew = false;
    auto sp = Find(currentDeclStmt.first);
    if(currentDeclStmt.first == "new"){seenNew = true;}
    if(sp){
        varIt->second.slines.insert(currentDeclStmt.second); //varIt is lhs
        sp->use.insert(currentDeclStmt.second);
        if(varIt->second.potentialAlias && !seenNew){ //new operator of the form int i = new int(tmp); screws around with aliasing
            varIt->second.lastInsertedAlias = varIt->second.aliases.insert(sp->variableName).first;
        }else{
            sp->dvars.insert(varIt->second.variableName);
            sp->use.insert(currentDeclStmt.second);
        }
    }
    currentDeclStmt.first.clear(); //because if it's a multi-init decl then inits will run into one another.
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
/**
* GetFunctionData
* Knows proper constraints for obtaining function's return type, name, and arguments. Stores all of this in
* functionTmplt.
*/
void srcSliceHandler::GetFunctionData(){
    //Get function type
    if(triggerField[type] && !(triggerField[parameter_list] || triggerField[block] || triggerField[member_list])){
        functionTmplt.returnType = currentFunctionBody.functionName;
    }
    //Get function name
    if(triggerField[name] == 1 && !(triggerField[argument_list] || 
        triggerField[block] || triggerField[type] || triggerField[parameter_list] || triggerField[member_list])){ 
        std::size_t pos = currentFunctionBody.functionName.find("::");
        if(pos != std::string::npos){
            currentFunctionBody.functionName.erase(0, pos+2);
        }
        if(isConstructor){
            std::stringstream ststrm;
            ststrm<<constructorNum;
            currentFunctionBody.functionName+=ststrm.str(); //number the constructor. Find a better way than stringstreams someday.
        }
        functionTmplt.functionLineNumber = currentFunctionBody.functionLineNumber;
        functionTmplt.functionName = currentFunctionBody.functionName; //give me the hash num for this name.
        //std::cerr<<"herher: "<<functionTmplt.functionName<<std::endl;
    }
    //Get param types
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type] && !triggerField[block]){
        //std::cerr<<"type: "<<currentParamType.first<<std::endl;
        unsigned int paramHash = paramTypeHash(currentParamType.first);
        functionTmplt.params.push_back(paramHash);
        functionTmplt.functionNumber += paramHash;
        sysDict.typeTable.insert(std::make_pair(paramHash, currentParamType.first));
    }
    //Get Param names
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[type] || triggerField[block])){
        //std::cerr<<"name: "<<currentParamType.first<<std::endl;
        varIt = FunctionIt->second.insert(std::make_pair(currentParam.first, 
            SliceProfile(declIndex, fileNumber, 
                functionTmplt.GetFunctionUID(), currentParam.second, currentParam.first, currentParamType.first, potentialAlias, inGlobalScope))).first;
        varIt->second.def.insert(currentDeclStmt.second);
    }
}

void srcSliceHandler::GetFunctionDeclData(){
    if(triggerField[type] && !(triggerField[parameter_list] || triggerField[block] || triggerField[member_list])){
        functionTmplt.returnType = currentFunctionDecl.functionName;
    }
    if(triggerField[name]){
        functionTmplt.functionLineNumber = currentFunctionDecl.functionLineNumber;
        functionTmplt.functionName = currentFunctionDecl.functionName;
        //std::cerr<<"name: "<<functionTmplt.functionName<<std::endl;
    }
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type]){
        //std::cerr<<"type: "<<currentParamType.first<<std::endl;
        unsigned int paramHash = paramTypeHash(currentParamType.first);
        functionTmplt.functionNumber += paramHash;
        functionTmplt.params.push_back(paramHash);
        sysDict.typeTable.insert(std::make_pair(paramHash, currentParamType.first));
    }
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[type])) {

    }
}
/**
* GetDeclStmtData
* Knows proper constraints for obtaining DeclStmt type and name.
* creates a new slice profile and stores data about decle statement inside.
*/
void srcSliceHandler::GetDeclStmtData(){
    //Get name of decl stmt
    if(triggerField[decl] && !(triggerField[type] || triggerField[init] || triggerField[expr] || triggerField[index])){
        
        if(triggerField[classn]){ //In a class so just get type and name then leave. No special processing needed. TODO might need to deal with commas
            //std::cerr<<"Type: "<<currentDeclType.first<<" Name: "<<currentDeclStmt.first<<std::endl;
            classIt->second.memberVariables.insert(std::make_pair(TypeNamePair(paramTypeHash(currentDeclType.first),currentDeclStmt.first), std::unordered_set<unsigned int>()));
            return;
        }

        if(currentDeclStmt.first[0] == ','){//corner case with decls like: int i, k, j. This is a patch, fix properly later.
            currentDeclStmt.first.erase(0,1);
        }//Globals won't be in FunctionIT
        if(!inGlobalScope){
            std::cerr<<"Inserting: "<<currentDeclStmt.first<<std::endl;
            varIt = FunctionIt->second.insert(std::make_pair(currentDeclStmt.first,
                SliceProfile(declIndex, fileNumber,
                    functionTmplt.GetFunctionUID(), currentDeclStmt.second, 
                    currentDeclStmt.first, currentDeclType.first, potentialAlias, inGlobalScope))).first;
            varIt->second.def.insert(currentDeclStmt.second);
            if(!triggerField[classn]){ //If this decl is an object then record whatever member variables it might have
                auto classIt2 = sysDict.classTable.find(currentDeclType.first); //look up class information for type
                if(classIt2 != sysDict.classTable.end()){
                    varIt->second.memberVariableLineNumberMap = classIt2->second.memberVariables; //store class information in variable's slice profile
                    //std::cerr<<"MemberVarMap: "<<varIt->second.memberVariableLineNumberMap.size()<<std::endl;
                }
            }
        }else{ //TODO: Handle def use for globals
            //std::cerr<<currentDeclStmt.first<<std::endl;
            //std::cout<<"Name: "<<currentDeclStmt.first<<std::endl;
            auto iter = sysDict.globalMap.insert(std::make_pair(currentDeclStmt.first, 
                        SliceProfile(declIndex, fileNumber, 
                            functionTmplt.GetFunctionUID(), currentDeclStmt.second, 
                            currentDeclStmt.first, currentDeclType.first, potentialAlias, inGlobalScope))).first;

        }
    }
    //Get Init of decl stmt
}
void srcSliceHandler::ProcessMemberDeref(){
    skipMember= false;
    auto* sp = memberAccessStack.front().second == nullptr ? nullptr : Find(memberAccessStack.front().second->variableName); //find slice profile for lhs
    if(sp){
        auto memIt = sp->memberVariableLineNumberMap.find(currentExprStmt.first); //find member variable
        if(memIt != sp->memberVariableLineNumberMap.end()){
            memIt->second.insert(currentExprStmt.second);
        }else{
            //TODO This is a hack. Make it so that we properly hash the function on more than just the name. Probably need to get a functiontmplt going here
            //biggest issue will be collecting all data about the function for the tmplt. Consider for future versions.
            auto fnh = functionNameHash(currentExprStmt.first);
            sysDict.functionTable.insert(std::make_pair(fnh, currentExprStmt.first));
            sp->cfunctions.insert(std::make_pair(fnh, 0));
        }
    }
}
/**
 * ProcessExprStmt
 * Get entire expression statement and then process by first splitting to lhs and rhs. Process the lhs
 * by saving its slines if it can be found in the map. After lhs is processed, keep track of it and then
 * process the rhs for any aliases, dvars, or function calls.
 */
void srcSliceHandler::ProcessExprStmt(){
  if(!opassign){ 
    //std::cerr<<"Checking for: "<<currentExprStmt.first<<std::endl;
    auto* tmp = Find(currentExprStmt.first);
    if(tmp){ //Found it so store what its current name and line number are.
        memberAccessStack.push_back(std::make_pair(currentExprStmt.second, tmp));
    }else if (skipMember){ //Todo: make sure this works
        SliceProfile* p = new SliceProfile(0,0,0,currentExprStmt.second, currentExprStmt.first,""); //(unsigned int idx, unsigned int fle, unsigned int fcn, unsigned int sline, std::string name, std::string type, bool alias = 0, bool global = 0)
        memberAccessStack.push_back(std::make_pair(currentExprStmt.second, p));
    }
  }else{
    if(memberAccessStack.back().second == nullptr){return;}
    auto sprIt = Find(currentExprStmt.first);//find the sp for the rhs
    if(sprIt){ //lvalue depends on this rvalue
        //std::cerr<<"Here2"<<std::endl;
        if(memberAccessStack.back().second->variableName != sprIt->variableName){    
            if(!memberAccessStack.back().second->potentialAlias || dereferenced){ //It is not potentially a reference and if it is, it must not have been dereferenced
                sprIt->dvars.insert(memberAccessStack.back().second->variableName); //it's not an alias so it's a dvar
            }else{//it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                //dirtyAlias = true;
                memberAccessStack.back().second->lastInsertedAlias = memberAccessStack.back().second->aliases.insert(sprIt->variableName).first;
            }
            sprIt->slines.insert(currentExprStmt.second);
            sprIt->use.insert(currentExprStmt.second);           
            if(sprIt->potentialAlias){//Union things together. If this was an alias of anoter thing, update the other thing
                if(!sprIt->aliases.empty()){
                    //std::cerr<<"Name1: "<<*(sprIt->lastInsertedAlias); //Get vars that sprit aliases
                    auto spaIt = FunctionIt->second.find(*(sprIt->lastInsertedAlias)); //problem  because last alias is an iterator and can reference things in other functions. Maybe make into a pointer. Figure out why I need it.
                    if(spaIt != FunctionIt->second.end()){
                        //std::cerr<<"Name: "<<spaIt->second.variableName<<" "<<memberAccessStack.back().second->variableName<<std::endl;
                        spaIt->second.dvars.insert(memberAccessStack.back().second->variableName);
                        spaIt->second.use.insert(currentExprStmt.second);  
                        spaIt->second.slines.insert(currentExprStmt.second);
                    }
                }
            }
        }
    }
    }
}


/*
 *ComputeInterprocedural
 *@param f- name of the file
 *No return value
 *
 *
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
 @Return SliceProfile of the variable
  *
 *
 *
 *
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