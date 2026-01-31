#pragma once
#include <string>
#include <iostream>

inline void MakeAError(const std::string& text) {
    std::cout << "\033[31m" << text << "\033[0m" << std::endl; // red
}

inline void MakeAWarning(const std::string& text) {
    std::cout << "\033[33m" << text << "\033[0m" << std::endl; // yellow
}

inline void MakeAProblem(const std::string& text) {
    std::cout << "\033[36m" << text << "\033[0m" << std::endl; // cyan
}

inline void MakeASuccess(const std::string& text) {
    std::cout << "Success: " << text << std::endl;
}