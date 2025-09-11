#pragma once

#include <iostream>
#include <cstdio>

#ifndef LOG
#define LOG(msg) \
    std::cout << "\033[0m(LOG) " << __PRETTY_FUNCTION__ << ": " << msg << "\n"
#endif

#ifndef WARN
#define WARN(msg) \
    std::cerr << "\033[33m(WARN) \033[0m" << __PRETTY_FUNCTION__ << ": " << msg << "\n"
#endif

#ifndef ERROR
#define ERROR(msg) \
    std::cerr << "\033[31m(ERROR) \033[0m" << __PRETTY_FUNCTION__ << ": " << msg << "\n"
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
