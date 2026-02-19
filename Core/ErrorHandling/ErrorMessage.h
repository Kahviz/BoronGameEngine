#pragma once
#include <string>
#include <iostream>

inline void MakeAError(const std::string& text) {
    #ifdef _DEBUG
        std::cout << "\033[31m" << text << "\033[0m" << std::endl; // red
    #endif
}

inline void MakeAWarning(const std::string& text) {
    #ifdef _DEBUG
        std::cout << "\033[33m" << text << "\033[0m" << std::endl; // yellow
    #endif
}

inline void MakeAProblem(const std::string& text) {
    #ifdef _DEBUG
        std::cout << "\033[36m" << text << "\033[0m" << std::endl; // cyan
    #endif
}

inline void MakeASuccess(const std::string& text) {
    #ifdef _DEBUG
        std::cout << "Success: " << text << std::endl;
    #endif
}