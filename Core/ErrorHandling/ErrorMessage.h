#pragma once
#include <string>
#include <iostream>
#include "GLOBALS.h"

inline void CreateError(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[31m" << text << "\033[0m\n"; // red
    #endif
}

inline void CreateWarning(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[33m" << text << "\033[0m\n"; // yellow
    #endif
}

inline void CreateProblem(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[36m" << text << "\033[0m\n"; // cyan
    #endif
}

inline void CreateSuccess(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[32mSuccess: " << text << "\033[0m\n"; //green
    #endif
}

inline void CreateInfo(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[1;30mInfo: " << text << "\033[0m\n";
    #endif
}

inline void ProfilerInformation(const std::string& text) {
    #ifdef _DEBUG
        std::cout << "\033[1;32m" << text << "\033[0m\n";
    #endif
}