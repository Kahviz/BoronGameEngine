#pragma once
#include <string>
#include <iostream>
#include "GLOBALS.h"

inline void MakeAError(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[31m" << text << "\033[0m" << std::endl; // red
    #endif
}

inline void MakeAWarning(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[33m" << text << "\033[0m" << std::endl; // yellow
    #endif
}

inline void MakeAProblem(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[36m" << text << "\033[0m" << std::endl; // cyan
    #endif
}

inline void MakeASuccess(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[32mSuccess: " << text << "\033[0m" << std::endl; //green
    #endif
}

inline void MakeAInfo(const std::string& text) {
    #if PRINTDEBUGINFO == 1
        std::cout << "\033[1;30mInfo: " << text << "\033[0m" << std::endl;
    #endif
}

inline void ProfilerInformation(const std::string& text) {
    #ifdef _DEBUG
        std::cout << "\033[1;32m" << text << "\033[0m" << std::endl;
    #endif
}