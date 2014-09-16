#include <srcSliceHandler.hpp>
#include <Utility.hpp>
void srcSliceHandler::ProcessConstructorDecl(){
    auto strVec = SplitOnTok(currentDeclStmt.first, "+<.*->&=():,");
    for(std::string str : strVec){
        auto sp = Find(str,currentFunctionBody.functionName);
        if(sp){
            varIt->second.dvars.insert(currentFunctionBody.functionName+":"+sp->variableName);
            //std::cerr<<"This: "<<str<<std::endl;
        }
    }
}
void srcSliceHandler::ProcessDeclStmt(){
    if(triggerField[decl_stmt] && triggerField[decl] && triggerField[init] && 
    !(triggerField[type] || triggerField[argument_list] || triggerField[call])){ //do something if there's an init
        //std::cerr<<"INit: "<<currentDeclStmt.first<<std::endl;
        auto expr = SplitLhsRhs(currentDeclStmt.first);
        //std::cerr<<"GO: "<<expr.front()<<" : "<<expr.back()<<std::endl;
        if(expr.size() > 1){ //found an expression of the form lhs = rhs. Make a new lhs slice profile and then iterate over rhs to insert.
            auto strVec = SplitOnTok(expr.back(), "+<.*->&=(),"); //split rhs
            //std::cerr<<"Prof: " <<  currentFunctionBody.functionName+":"+expr.front()<<std::endl;
            if(!inGlobalScope){
                varIt = FunctionIt->second.insert(std::make_pair(currentFunctionBody.functionName+":"+expr.front(), 
                SliceProfile(currentDeclStmt.second - functionTmplt.functionLineNumber, fileNumber, 
                    functionTmplt.functionNumber, currentDeclStmt.second, 
                    currentFunctionBody.functionName+":"+expr.front(), potentialAlias))).first;
            }else{
                FunctionIt->second.insert(std::make_pair(expr.front(), 
                SliceProfile(currentDeclStmt.second - functionTmplt.functionLineNumber, fileNumber, 
                    functionTmplt.functionNumber, currentDeclStmt.second, 
                    currentFunctionBody.functionName+":"+expr.front(), potentialAlias))).first;
            }
            for(std::string str : strVec){
                if(str != expr.front()){
                    //std::cerr<<"Name: "<<currentFunctionBody.functionName+":"+str<<std::endl;
                    auto sp = Find(str,currentFunctionBody.functionName);
                    if(sp){
                        varIt->second.slines.insert(currentDeclStmt.second);
                        sp->slines.insert(currentDeclStmt.second);
                        if(varIt->second.potentialAlias){
                            varIt->second.lastInsertedAlias = varIt->second.aliases.insert(currentFunctionBody.functionName+":"+str).first;
                        }else{
                            sp->dvars.insert(currentFunctionBody.functionName+":"+varIt->second.variableName);
                        }
                    }
                }
            }
        }else{ //found an expression on the rhs of something. Because lhs gets strings first, we need to process that.
            auto strVec = SplitOnTok(expr.front(), "+<:.*->&=(),");
            for(std::string str : strVec){
                //std::cerr<<"Name: "<<currentFunctionBody.functionName+":"+str<<std::endl;
                auto sp = Find(str,currentFunctionBody.functionName);
                if(sp){
                    varIt->second.slines.insert(currentDeclStmt.second);
                    sp->slines.insert(currentDeclStmt.second);
                    if(varIt->second.potentialAlias){
                        varIt->second.lastInsertedAlias = varIt->second.aliases.insert(currentFunctionBody.functionName+":"+str).first;
                    }else{
                        sp->dvars.insert(currentFunctionBody.functionName+":"+varIt->second.variableName);
                    }
                }
            }
        }
        currentDeclStmt.first.clear(); //because if it's a multi-init decl then inits will run into one another.
        //ProcessDeclStmtInit();
        //
    }
}
/**
    * GetCallData
    *
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
        //std::cerr<<"Func: "<<nameOfCurrentClldFcn.top()<<" "<<currentCallArgData.first<<std::endl;
        for(std::string str : strVec){
            //std::cerr<<"Attempt: "<<currentFunctionBody.functionName<<":"<<str<<std::endl;
            auto sp = Find(str,currentFunctionBody.functionName); //check to find sp for the variable being called on fcn
            //std::cerr<<std::endl<<sp<<std::endl;
            if(sp){
                //std::cerr<<"in: "<<str<<std::endl;
                sp->slines.insert(currentCallArgData.second);
                sp->index = currentCallArgData.second - functionTmplt.functionLineNumber;
                for(std::string spltStr : spltVec){ //iterate over strings split from the function being called (becaused it might be object -> function)
                    //std::cerr<<"Pre if: "<<spltStr<<std::endl;
                    spltStr.erase(//remove anything weird like empty arguments.
                        std::remove_if(spltStr.begin(), spltStr.end(), [](const char ch){return !std::isalnum(ch);}), 
                        spltStr.end());
                    if(!spltStr.empty()){//If the string isn't empty, we got a good variable and can insert it.
                        //std::cerr<<"Here: "<<sysDict.fileTable.find(FileIt->first)->second<<" : "<<spltStr<<" : "<<currentCallArgData.second<<std::endl;
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
        }else{
            //std::cerr<<"NAME: "<<currentFunctionBody.functionName<<std::endl;
        }
        functionTmplt.functionLineNumber = currentFunctionBody.functionLineNumber;
        functionTmplt.functionNumber = functionAndFileNameHash(currentFunctionBody.functionName); //give me the hash num for this name.            
    }
    //Get param types
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && triggerField[type] && !triggerField[block]){
        //std::cerr<<"Param type: "<<currentParam.first<<std::endl;
    }
    //Get Param names
    if(triggerField[parameter_list] && triggerField[param] && triggerField[decl] && !(triggerField[type] || triggerField[block])){
        //std::cerr<<"Param name: "<<currentParam.first<<std::endl;
        varIt = FunctionIt->second.insert(std::make_pair(currentFunctionBody.functionName+":"+currentParam.first, 
            SliceProfile(currentParam.second - functionTmplt.functionLineNumber, fileNumber, 
                currentFunctionBody.functionLineNumber, currentParam.second, currentParam.first, potentialAlias))).first;
    }        
}
/**
    * GetDeclStmtData
    * Knows proper constraints for obtaining DeclStmt type and name.
    * creates a new slice profile and stores data about decle statement inside.
    */
void srcSliceHandler::GetDeclStmtData(){
    if(triggerField[decl] && triggerField[type] && !(triggerField[init])){
        //std::cerr<<currentCallArgData.first<<std::endl;
        //functionTmplt.declstmt.type = currentCallArgData.first; //store type
        //agglomerate string into type. Clear when we exit the decl_stmt
    }
    //Get name of decl stmt
    if(triggerField[decl] && !(triggerField[type] || triggerField[init] || triggerField[expr] || triggerField[index] || triggerField[classn])){
        if(currentDeclStmt.first[0] == ','){//corner case with decls like: int i, k, j. This is a patch, fix properly later.
            currentDeclStmt.first.erase(0,1);
        }//Globals won't be in FunctionIT
        //std::cerr<<"Decl: "<<currentDeclStmt.first<<std::endl;
        if(!inGlobalScope){
        varIt = FunctionIt->second.insert(std::make_pair(currentFunctionBody.functionName+":"+currentDeclStmt.first, 
            SliceProfile(currentDeclStmt.second - functionTmplt.functionLineNumber, fileNumber, 
                functionTmplt.functionNumber, currentDeclStmt.second, 
                currentDeclStmt.first, potentialAlias))).first;
        }else{
            //std::cout<<"Name: "<<currentFunctionBody.functionName+":"+currentDeclStmt.first<<std::endl;
            sysDict.globalMap.insert(std::make_pair(currentDeclStmt.first, 
            SliceProfile(currentDeclStmt.second - functionTmplt.functionLineNumber, fileNumber, 
                functionTmplt.functionNumber, currentDeclStmt.second, 
                currentDeclStmt.first, potentialAlias)));
        }
    }
    //Get Init of decl stmt
}
void srcSliceHandler::ProcessExprStmt(){
    auto lhsrhs = SplitLhsRhs(currentExprStmt.first);
    auto resultVecl = SplitOnTok(lhsrhs.front(), "+<.*->&");
    SliceProfile* splIt(nullptr);        
    //First, take the left hand side and mark sline information. Doing it first because later I'll be iterating purely
    //over the rhs.
    for(auto rVecIt = resultVecl.begin(); rVecIt!= resultVecl.end(); ++rVecIt){            
        splIt = Find(*rVecIt, currentFunctionBody.functionName);
        if(splIt){ //Found it so add statement line.
            splIt->slines.insert(currentExprStmt.second);
            break; //found it, don't care about the rest (ex. in: bottom -> next -- all I need is bottom.)
        }            
    }
    //Doing rhs now. First check to see if there's anything to process (Note: Check to make sure Op even needs to be here anymore)
    if(splIt){
        auto resultVecr = SplitOnTok(lhsrhs.back(), "+<.*->&"); //Split on tokens. Make these const... or standardize them. Or both.
        for(auto rVecIt = resultVecr.begin(); rVecIt != resultVecr.end(); ++rVecIt){ //loop over words and check them against map
            std::string fullName = currentFunctionBody.functionName+":"+*rVecIt;//fix                
            if(currentFunctionBody.functionName+":"+splIt->variableName != fullName){//lhs != rhs
                auto sprIt = Find(*rVecIt, currentFunctionBody.functionName);//find the sp for the rhs
                if(sprIt){ //lvalue depends on this rvalue
                    //std::cerr<<"LHS: "<<splIt->variableName<<" RHS: "<<sprIt-><" "<<splIt->potentialAlias<<std::endl;
                    if(splIt && !splIt->potentialAlias){
                        sprIt->dvars.insert(currentFunctionBody.functionName+":"+splIt->variableName); //it's not an alias so it's a dvar
                    }else{//it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                        splIt->isAlias = true;
                        sprIt->lastInsertedAlias = sprIt->aliases.insert(splIt->variableName).first;
                    }
                    sprIt->slines.insert(currentExprStmt.second);                            
                    if(sprIt->isAlias){//Union things together. If this was an alias of anoter thing, update the other thing
                        if(!sprIt->aliases.empty()){
                            auto spaIt = FunctionIt->second.find(*sprIt->lastInsertedAlias);
                            if(spaIt != FunctionIt->second.end()){
                                spaIt->second.dvars.insert(currentFunctionBody.functionName+":"+splIt->variableName);
                                spaIt->second.slines.insert(currentExprStmt.second);
                            }
                        }
                    }
                }
            }
        }
    }
}
SliceProfile* srcSliceHandler::Find(const std::string& varName, const std::string& functionName){    
        auto sp = FunctionIt->second.find(functionName+":"+varName);
        if(sp != FunctionIt->second.end()){
            return &(sp->second);
        }else{
            auto sp2 = sysDict.globalMap.find(varName);
            if(sp2 != sysDict.globalMap.end()){
                //std::cerr<<"functionName:"<<functionName<<"Varname:"<<varName<<std::endl;
                return &(sp2->second);
            }
            //find global
        }
        return nullptr;
}