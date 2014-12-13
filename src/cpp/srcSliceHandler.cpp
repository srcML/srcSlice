#include <srcSliceHandler.hpp>
#include <Utility.hpp>

/**
 * Find
 * @param varName - Name of the variable whose slice profile we want
 * @Return Pointer to Slice Profile or Null
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
    auto strVec = SplitOnTok(currentDeclStmt.first, "+<.*->&=():,");
    for(std::string str : strVec){
        auto sp = Find(str);
        if(sp){
            sp->dvars.insert(varIt->second.variableName);
        }
    }
}

/*
* ProcessDeclStmt
* Takes rhs of decl_stmt and processes it. Split by tokens and then throw against the map for an answer.
* corner case at new operator because new makes an object even if its argument is an alias.
*/
void srcSliceHandler::ProcessDeclStmt(){
    auto strVec = SplitOnTok(currentDeclStmt.first, "+<:.*->&=(),");
    bool seenNew = false;
    for(std::string str : strVec){
        auto sp = Find(str);
        if(str == "new"){seenNew = true;}
        if(sp){
            varIt->second.slines.insert(currentDeclStmt.second); //varIt is lhs
            //varIt->second.def.insert(currentDeclStmt.second);
            sp->slines.insert(currentDeclStmt.second);
            //sp->use.insert(currentDeclStmt.second);
            if(varIt->second.potentialAlias && !seenNew){ //new operator of the form int i = new int(tmp); screws around with aliasing
                dirtyAlias = true;
                varIt->second.lastInsertedAlias = sp->aliases.insert(varIt->second.variableName).first;
            }else{
                sp->dvars.insert(varIt->second.variableName);
            }
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
        auto strVec = SplitOnTok(currentCallArgData.first, ":+<.*->&=(),"); //Split the current string of call/arguments so we can get the variables being called
        auto spltVec = SplitOnTok(nameOfCurrentClldFcn.top(), ":+<.*->&=(),"); //Split the current string which contains the function name (might be object->function)
        for(std::string str : strVec){
            auto sp = Find(str); //check to find sp for the variable being called on fcn
            if(sp){
                sp->slines.insert(currentCallArgData.second);
                sp->use.insert(currentCallArgData.second);
                sp->index = currentCallArgData.second - functionTmplt.functionLineNumber;
                for(std::string spltStr : spltVec){ //iterate over strings split from the function being called (becaused it might be object -> function)
                    spltStr.erase(//remove anything weird like empty arguments.
                        std::remove_if(spltStr.begin(), spltStr.end(), [](const char ch){return !std::isalnum(ch);}), 
                        spltStr.end());
                    if(!spltStr.empty()){//If the string isn't empty, we got a good variable and can insert it.
                        sp->cfunctions.push_back(std::make_pair(spltStr, numArgs));
                        std::size_t pos = currentCallArgData.first.rfind(str);
                        if(pos != std::string::npos){
                            currentCallArgData.first.erase(pos, str.size()); //remove the argument from the string so it won't concat with the next
                        }
                    }
                }
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
        functionTmplt.functionNumber = functionNameHash(currentFunctionBody.functionName); //give me the hash num for this name.            
    }
    //Get param types
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type] && !triggerField[block]){
    }
    //Get Param names
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[type] || triggerField[block])){
        varIt = FunctionIt->second.insert(std::make_pair(currentParam.first, 
            SliceProfile(currentParam.second - functionTmplt.functionLineNumber, fileNumber, 
                currentFunctionBody.functionLineNumber, currentParam.second, currentParam.first, potentialAlias, inGlobalScope))).first;
        varIt->second.def.insert(currentDeclStmt.second);
    }
}

/**
* GetDeclStmtData
* Knows proper constraints for obtaining DeclStmt type and name.
* creates a new slice profile and stores data about decle statement inside.
*/
void srcSliceHandler::GetDeclStmtData(){
    if(triggerField[decl] && triggerField[type] && !(triggerField[init])){
        //functionTmplt.declstmt.type = currentCallArgData.first; //store type
        //agglomerate string into type. Clear when we exit the decl_stmt
    }
    //Get name of decl stmt
    if(triggerField[decl] && !(triggerField[type] || triggerField[init] || triggerField[expr] || triggerField[index] || triggerField[classn])){
        if(currentDeclStmt.first[0] == ','){//corner case with decls like: int i, k, j. This is a patch, fix properly later.
            currentDeclStmt.first.erase(0,1);
        }//Globals won't be in FunctionIT
        if(!inGlobalScope){
            varIt = FunctionIt->second.insert(std::make_pair(currentDeclStmt.first, 
                SliceProfile(currentDeclStmt.second - functionTmplt.functionLineNumber, fileNumber, 
                    functionTmplt.functionNumber, currentDeclStmt.second, 
                    currentDeclStmt.first, potentialAlias, inGlobalScope))).first;
            varIt->second.def.insert(currentDeclStmt.second);
        }else{ //TODO: Handle def use for globals
            //std::cout<<"Name: "<<currentDeclStmt.first<<std::endl;
            sysDict.globalMap.insert(std::make_pair(currentDeclStmt.first, 
            SliceProfile(currentDeclStmt.second - functionTmplt.functionLineNumber, fileNumber, 
                functionTmplt.functionNumber, currentDeclStmt.second, 
                currentDeclStmt.first, potentialAlias, inGlobalScope)));
        }
    }
    //Get Init of decl stmt
}

/**
 * ProcessExprStmt
 * Get entire expression statement and then process by first splitting to lhs and rhs. Process the lhs
 * by saving its slines if it can be found in the map. After lhs is processed, keep track of it and then
 * process the rhs for any aliases, dvars, or function calls.
 */
void srcSliceHandler::ProcessExprStmt(){
    auto lhsrhs = SplitLhsRhs(currentExprStmt.first);
    auto resultVecl = SplitOnTok(lhsrhs.front(), "+<.*->&");
    SliceProfile* splIt(nullptr);        
    //First, take the left hand side and mark sline information. Doing it first because later I'll be iterating purely
    //over the rhs.
    for(auto rVecIt = resultVecl.begin(); rVecIt!= resultVecl.end(); ++rVecIt){            
        splIt = Find(*rVecIt);
        if(splIt){ //Found it so add statement line.
            splIt->slines.insert(currentExprStmt.second);
            splIt->def.insert(currentExprStmt.second);
            break; //found it, don't care about the rest (ex. in: bottom -> next -- all I need is bottom.)
        }            
    }
    //Doing rhs now. First check to see if there's anything to process
    if(splIt){
        auto resultVecr = SplitOnTok(lhsrhs.back(), "+<.*->&"); //Split on tokens. Make these const... or standardize them. Or both.
        for(auto rVecIt = resultVecr.begin(); rVecIt != resultVecr.end(); ++rVecIt){ //loop over words and check them against map
            if(splIt->variableName != *rVecIt){//lhs != rhs
                auto sprIt = Find(*rVecIt);//find the sp for the rhs
                if(sprIt){ //lvalue depends on this rvalue
                    if(!splIt->potentialAlias){
                        sprIt->dvars.insert(splIt->variableName); //it's not an alias so it's a dvar
                    }else{//it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                        dirtyAlias = true;
                        sprIt->lastInsertedAlias = sprIt->aliases.insert(splIt->variableName).first;
                    }
                    sprIt->slines.insert(currentExprStmt.second);
                    sprIt->use.insert(currentExprStmt.second);                            
                    if(sprIt->potentialAlias){//Union things together. If this was an alias of anoter thing, update the other thing
                        if(!sprIt->aliases.empty()){
                            if(!dirtyAlias){
                                auto spaIt = FunctionIt->second.find(*sprIt->lastInsertedAlias); //problem  because last alias is an iterator and can reference things in other functions. Maybe make into a pointer. Figure out why I need it.
                                if(spaIt != FunctionIt->second.end()){
                                    spaIt->second.dvars.insert(splIt->variableName);
                                    spaIt->second.use.insert(currentExprStmt.second);  
                                    spaIt->second.slines.insert(currentExprStmt.second);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}






/*
 *ComputeInterprocedural
 * Takes slicing criterion as input
 *outputs a slicing profile for each varible in the procedure
 *
 *
 */
void srcSliceHandler::ComputeInterprocedural(const std::string& f){
    FileIt =sysDict.dictionary.find(f);
    FunctionIt = (FileIt)->second.begin();
    FunctionVarMap::iterator FunctionItEnd = (FileIt)->second.end();
    std::string functionName;
    unsigned int argumentIndex = 0;
    SliceProfile Spi;

    for(FunctionIt; FunctionIt != FunctionItEnd; ++FunctionIt){
        for(VarMap::iterator it = FunctionIt->second.begin(); it != FunctionIt->second.end(); ++it){  
            if(it->second.visited == false){       
                for(int i = 0; i < it->second.cfunctions.size(); ++i ){
                    functionName = it->second.cfunctions[i].first;
                    argumentIndex = it->second.cfunctions[i].second;
                    Spi = ArgumentProfile(FunctionIt, argumentIndex);
                    it->second.slines = SetUnion(it->second.slines, Spi.slines);
                    //it->second.cfunctions = SetUnion(it->second.cfunctions, Spi.cfunctions);
                    it->second.aliases = SetUnion(it->second.aliases, Spi.aliases);
                    it->second.dvars = SetUnion(it->second.dvars, Spi.dvars);
                    }
                it->second.visited = true;
            } 
        }
    }      
}


SliceProfile srcSliceHandler::ArgumentProfile(FunctionVarMap::iterator functIt, unsigned int parameterIndex){
    VarMap::iterator v = functIt->second.begin();
    SliceProfile Spi;
    std::string newFunctionName;
    unsigned int newParameterIndex;
    std::string functionName;
    std::unordered_map<unsigned int, FunctionData>::iterator funcNameItr = sysDict.functionTable.find(functIt->first);
    if(funcNameItr != sysDict.functionTable.end()){
        functionName = funcNameItr->second.functionName;
    }
    else{
        std::cerr<< "FATAL ERROR: Cound not find function. ";
        return Spi; 
    }

    for(VarMap::iterator it = v; it != functIt->second.end(); ++it){
        if (it->second.index == parameterIndex){
            Spi = it->second; 
            return Spi;
        }
        else{
            for(int i = 0; i < it->second.cfunctions.size(); ++i ){
                newFunctionName = it->second.cfunctions[i].first;
                newParameterIndex = it->second.cfunctions[i].second; 
                if(newFunctionName != functionName){
                    
                    unsigned int hash = functionNameHash(newFunctionName);
                    FunctionVarMap::iterator newFunct = (FileIt->second.find(hash));
                    if(newFunct != FileIt->second.end())
                        Spi = ArgumentProfile(newFunct, newParameterIndex);
                 }
            } 
            it->second.visited = true;
        }
    }
return Spi;
}
