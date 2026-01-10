#pragma once

#include <string>

// I stole this code :)
#if defined(_WIN32)
    #include <windows.h>
    using LibHandle = HMODULE;
    #define LOAD_LIB(name) LoadLibraryA(name)
    #define GET_SYM(handle, name) GetProcAddress(handle, name)
    #define CLOSE_LIB(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    using LibHandle = void*;
    #define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
    #define GET_SYM(handle, name) dlsym(handle, name)
    #define CLOSE_LIB(handle) dlclose(handle)
#endif

inline LibHandle loadLibrary(const std::string &path) {
    return LOAD_LIB(path.c_str());
}

inline void *getSymbol(LibHandle lib, const std::string &name) {
    return GET_SYM(lib, name.c_str());
}

inline void closeLibrary(LibHandle lib) {
    CLOSE_LIB(lib);
}