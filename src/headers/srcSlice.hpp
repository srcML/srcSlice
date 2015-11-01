#include <srcSliceHandler.hpp>
#include <Utility.hpp>
#ifndef SRCSLICEHPP
#define SRCSLICEHPP
struct srcSlice{
/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
SliceDictionary dictionary;
srcSlice(){}
srcSlice(const char* filename, const char* encoding);
srcSlice(std::string buffer, const char* encoding);
srcSlice(FILE* file, const char* encoding);
srcSlice(int fd, const char* encoding);
void ReadArchiveFile(std::string filename);    
    int size()const {return dictionary.ffvMap.size();}
    
    bool SetContext(std::string fle, std::string fn, int linenumber){
        FileFunctionVarMap::iterator fleIt = dictionary.ffvMap.find(fle);
        if(fleIt != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fnIt = fleIt->second.find(fn);
            if(fnIt != fleIt->second.end()){
                dictionary.currentContext.currentFile = fleIt;
                dictionary.currentContext.currentFunc = fnIt;
                dictionary.currentContext.ln = linenumber;
                dictionary.currentContext.functionName = fle;
                dictionary.currentContext.functionName = fn;
                return true;
            }            
        }
        return false;
    }

    bool SetContext(std::string fn, int linenumber){
        if(dictionary.currentContext.currentFile != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fnIt = dictionary.currentContext.currentFile->second.find(fn);
            if(fnIt != dictionary.currentContext.currentFile->second.end()){
                dictionary.currentContext.currentFunc = fnIt;
                dictionary.currentContext.ln = linenumber;
                dictionary.currentContext.functionName = fn;
                return true;
            }            
        }
        return false;
    }
    bool SetContext(int linenumber){ //it enough to just check function? Need to check file?
        if(dictionary.currentContext.currentFunc != dictionary.currentContext.currentFile->second.end()){
            dictionary.currentContext.ln = linenumber;
            return true;
        }
        return false;
    }
    //Definition of find that assumes the user didn't give a context (They should just give a context, though, tbh).
    std::pair<bool, SliceProfile> Find(std::string flename, std::string funcname, std::string varname, int lineNumber)const{
        FileFunctionVarMap::const_iterator ffvmIt = dictionary.ffvMap.find(flename);
        if(ffvmIt != dictionary.ffvMap.end()){
            FunctionVarMap::const_iterator fvmIt = ffvmIt->second.find(funcname);
            if(fvmIt != ffvmIt->second.end()){
                VarMap::const_iterator vtmIt = fvmIt->second.find(varname);
                if(vtmIt != fvmIt->second.end()){
                    return std::make_pair(true, vtmIt->second);
                }
            }            
        }

        return std::make_pair(false, SliceProfile());
    }
    //Definition of find that assumes the user didn't give a context (They should just give a context, though, tbh).
    std::pair<bool, SliceProfile> Find(std::string funcname, std::string varname, int lineNumber)const{
        FunctionVarMap::const_iterator fvmIt = dictionary.currentContext.currentFile->second.find(funcname);
        if(fvmIt != dictionary.currentContext.currentFile->second.end()){
            VarMap::const_iterator vtmIt = fvmIt->second.find(varname);
            if(vtmIt != fvmIt->second.end()){
                return std::make_pair(true, vtmIt->second);
            }
        }
        return std::make_pair(false, SliceProfile());
    }
    //Definition of find that uses the context (so it doesn't need to take a function name as context)
    std::pair<bool, SliceProfile> Find(std::string varname) const{
        if(!dictionary.currentContext.IsSet()){
            throw std::runtime_error("Context not set"); //for now, std exception
        }else{
            VarMap::const_iterator it = dictionary.currentContext.currentFunc->second.find(varname);
            if(it != dictionary.currentContext.currentFunc->second.end()){
                return std::make_pair(true, it->second);
            }
            return std::make_pair(false, SliceProfile());
        }
    }
    bool Insert(std::string flename, std::string funcname, const SliceProfile& np){
        FileFunctionVarMap::iterator ffvmIt = dictionary.ffvMap.find(flename);
        if(ffvmIt != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fvmIt = ffvmIt->second.find(funcname);
            if(fvmIt != ffvmIt->second.end()){
                VarMap::iterator vtmIt = fvmIt->second.find(np.variableName);
                if(vtmIt != fvmIt->second.end()){
                    vtmIt->second = np;
                    return true;
                }else{
                    fvmIt->second.insert(std::make_pair(np.variableName, np));
                    return true;
                }
            }
        }
        return false;
    }

    bool Insert(std::string funcname, const SliceProfile& np){
        if(dictionary.currentContext.currentFile != dictionary.ffvMap.end()){
            FunctionVarMap::iterator fvmIt = dictionary.currentContext.currentFile->second.find(funcname);
            if(fvmIt != dictionary.currentContext.currentFile->second.end()){
                VarMap::iterator vtmIt = fvmIt->second.find(np.variableName);
                if(vtmIt != fvmIt->second.end()){
                    vtmIt->second = np;
                    return true;
                }else{
                    fvmIt->second.insert(std::make_pair(np.variableName, np));
                    return true;
                }
            }            
        }
        return false;
    }

    bool Insert(const SliceProfile& np){
        if(dictionary.currentContext.ln == -1){ //TODO: Make better
            throw std::runtime_error("Context not set"); //for now, std exception
        }else{
            auto it = dictionary.currentContext.currentFunc->second.find(np.variableName);
            if(it != dictionary.currentContext.currentFunc->second.end()){
                it->second = np;
                return true;
            }else{
                dictionary.currentContext.currentFunc->second.insert(std::make_pair(np.variableName, np));
                return true;
            }
        }
        return false;
    }

};
#endif