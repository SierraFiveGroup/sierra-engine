//Basic main-thread panic handler so we actually get semi-useful information about a crash outside of the debugger.

#pragma once

#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <ctime>
#include <string>
#include <chrono>

//Dogshite because windows doesn't like being normal or something
#ifdef _WIN32
    // Windows includes
    #include <windows.h>
    #include <dbghelp.h>
    #include <psapi.h>
    #include <processthreadsapi.h>
    #pragma comment(lib, "dbghelp.lib")
#else
    // POSIX includes
    #include <execinfo.h>
    #include <unistd.h>
    #include <sys/utsname.h>
    #include <sys/resource.h>
#endif

namespace Sierra {
    class PanicHandler {
        public:
            static void init() {
                std::set_terminate(terminateHandler);
                std::signal(SIGSEGV, signalHandler);
                std::signal(SIGABRT, signalHandler);
                std::signal(SIGFPE,  signalHandler);
                std::signal(SIGILL,  signalHandler);
                std::signal(SIGTERM, signalHandler);
            }

        private:
            static void signalHandler(int signal) {
                std::string reason;
                switch (signal) {
                    case SIGSEGV: reason = "Segmentation Fault"; break;
                    case SIGABRT: reason = "Abort (SIGABRT)"; break;
                    case SIGFPE:  reason = "Floating Point Exception"; break;
                    case SIGILL:  reason = "Illegal Instruction"; break;
                    case SIGTERM: reason = "Termination Signal"; break;
                    default: reason = "Unknown Fatal Signal"; break;
                }
                panic(reason);
                std::_Exit(EXIT_FAILURE);  // immediate exit, no cleanup
            }

            static void terminateHandler() {
                if (auto eptr = std::current_exception()) {
                    try {
                        std::rethrow_exception(eptr);
                    } catch (const std::exception& e) {
                        panic(std::string("Unhandled exception: ") + e.what());
                    } catch (...) {
                        panic("Unhandled non-standard exception");
                    }
                } else {
                    panic("Unknown termination cause");
                }
                std::_Exit(EXIT_FAILURE);
            }

            // I stole this code :fire: :speaking_head:
            static void dumpStack(std::ostream& out) {
                #ifdef _WIN32
                    void* stack[64];
                    HANDLE process = GetCurrentProcess();
                    SymInitialize(process, NULL, TRUE);
                
                    USHORT frames = CaptureStackBackTrace(0, 64, stack, NULL);
                    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256, 1);
                    symbol->MaxNameLen = 255;
                    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                
                    for (USHORT i = 0; i < frames; i++) {
                        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
                        out << i << ": " << symbol->Name << " - 0x" 
                            << std::hex << symbol->Address << std::dec << "\n";
                    }
                
                    free(symbol);
                #else
                    void* buffer[50];
                    int nptrs = backtrace(buffer, 50);
                    char** strings = backtrace_symbols(buffer, nptrs);
                    if (strings) {
                        for (int i = 0; i < nptrs; ++i)
                            out << strings[i] << "\n";
                        free(strings);
                    }
                #endif
            }

            static void dumpSystemInfo(std::ostream& out) {
                out << "---- System Info ----\n";
            
                #ifdef _WIN32
                    out << "Platform: Windows\n";
                    out << "Process ID: " << GetCurrentProcessId() << "\n";
                
                    char cwd[MAX_PATH];
                    GetCurrentDirectoryA(MAX_PATH, cwd);
                    out << "Working Dir: " << cwd << "\n";
                
                    MEMORYSTATUSEX memInfo;
                    memInfo.dwLength = sizeof(memInfo);
                    GlobalMemoryStatusEx(&memInfo);
                    out << "Memory Load: " << memInfo.dwMemoryLoad << "%\n";
                    out << "Total Phys: " << memInfo.ullTotalPhys / (1024*1024) << " MB\n";
                    out << "Avail Phys: " << memInfo.ullAvailPhys / (1024*1024) << " MB\n";
                #else
                    struct utsname unameData;
                    uname(&unameData);
                    out << "Platform: " << unameData.sysname << " " << unameData.release 
                        << " (" << unameData.machine << ")\n";
                    out << "Process ID: " << getpid() << "\n";
                
                    char cwd[256];
                    if (getcwd(cwd, sizeof(cwd)))
                        out << "Working Dir: " << cwd << "\n";
                
                    struct rusage usage;
                    getrusage(RUSAGE_SELF, &usage);
                    out << "Memory usage: " << usage.ru_maxrss << " KB\n";
                #endif
                
                out << "----------------------\n";
            }            

            static void panic(const std::string& reason) {
                std::cerr << "\n***\033[31m MAIN THREAD PANIC! \033[0m***\n";
                std::cerr << "Reason: " << reason << "\n";
                std::cerr << "Dumping panic trace...\n";

                std::ofstream dump("panic_dump.txt", std::ios::app);
                if (dump.is_open()) {
                    dump << "==== PANIC DUMP ====\n";
                    dump << "Time: " << currentTime() << "\n";
                    dump << "Reason: " << reason << "\n";
                    dumpSystemInfo(dump);
                    dump << "---- Stack Trace ----\n";
                    dumpStack(dump);

                    dump << "====================\n\n";
                }

                std::cerr << "Panic trace written to panic_dump.txt\n";
            }

            static std::string currentTime() {
                std::time_t t = std::time(nullptr);
                char buf[64];
                std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
                return buf;
            }
    };
}