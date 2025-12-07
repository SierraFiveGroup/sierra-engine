#pragma once

#include <iostream>
#include <cstdio>

#ifndef LOG

#ifndef WIN32 || _WIN32
#define LOG(msg) \
    std::cout << "\033[0m(LOG) " << __PRETTY_FUNCTION__ << ": " << msg << "\n"
#else
#define LOG(msg) \
    std::cout << "\033[0m(LOG) " << ": " << msg << "\n"
#endif

#endif

#ifndef WARN
#ifndef WIN32 || _WIN32
#define WARN(msg) \
    std::cerr << "\033[33m(WARN) \033[0m" << __PRETTY_FUNCTION__ << ": " << msg << "\n"
#else
#define WARN(msg) \
    std::cerr << "\033[33m(WARN) \033[0m" << ": " << msg << "\n"
#endif
#endif

#ifndef ERROR
#ifndef WIN32 || _WIN32
#define ERROR(msg) \
    std::cerr << "\033[31m(ERROR) \033[0m" << __PRETTY_FUNCTION__ << ": " << msg << "\n"
#else
#define ERROR(msg) \
    std::cerr << "\033[31m(ERROR) \033[0m" << ": " << msg << "\n"
#endif
#endif

#ifndef DBG
#define DBG(msg) \
    std::cerr << "\033[95m(DEBUG) \033[0m" << #msg /*<< " - " << __PRETTY_FUNCTION__*/ << ": " << msg << "\n"
#endif

#ifndef ERRPRINTF
#define ERRPRINTF(...) \
    fprintf(stderr, "\033[31m"); \
    fprintf(stderr,  __VA_ARGS__); \
    fprintf(stderr, "\033[0m"); 
#endif
