#include <srcSlice.hpp>
srcSlice::srcSlice(const char* filename, const char* encoding = 0){
    srcSAXController control(filename);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->ffvMap);
}
srcSlice::srcSlice(std::string buffer, const char* encoding = 0){
    srcSAXController control(buffer);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->ffvMap);
}
srcSlice::srcSlice(FILE* file, const char* encoding = 0){
    srcSAXController control(file);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->ffvMap);
}
srcSlice::srcSlice(int fd, const char* encoding = 0){
    srcSAXController control(fd);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->ffvMap);
}
void srcSlice::ReadArchiveFile(std::string filename){ //can call if default contructed
    srcSAXController control(filename.c_str());
    srcSliceHandler handler(&dictionary);
    control.parse(&handler); 
    DoComputation(handler, handler.sysDict->ffvMap);
}