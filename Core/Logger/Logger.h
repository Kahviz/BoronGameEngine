#pragma once
#include <iostream>
#include "GLOBALS.h"

namespace BoronLog {

    template <typename T>
    inline void Print(const char* color, const char* prefix, const T& printable) {
        #if PRINTDEBUGINFO == 1
                std::cout << color << prefix << printable << "\033[0m\n";
        #endif
    }

    template <typename T, typename U>
    inline void Print(const char* color, const char* prefix, const T& printable, const U& printable2) {
        #if PRINTDEBUGINFO == 1
            std::cout << color << prefix << printable << printable2 << "\033[0m\n";
        #endif
    }

}

template <typename T>
inline void CreateError(const T& printable) {
    BoronLog::Print("\033[31m", "", printable);
}

template <typename T, typename U>
inline void CreateError(const T& printable, const U& printable2) {
    BoronLog::Print("\033[31m", "", printable, printable2);
}

template <typename T>
inline void CreateWarning(const T& printable) {
    BoronLog::Print("\033[33m", "", printable);
}

template <typename T, typename U>
inline void CreateWarning(const T& printable, const U& printable2) {
    BoronLog::Print("\033[33m", "", printable, printable2);
}

template <typename T>
inline void CreateProblem(const T& printable) {
    BoronLog::Print("\033[36m", "", printable);
}

template <typename T, typename U>
inline void CreateProblem(const T& printable, const U& printable2) {
    BoronLog::Print("\033[36m", "", printable, printable2);
}

template <typename T>
inline void CreateSuccess(const T& printable) {
    BoronLog::Print("\033[32m", "Success: ", printable);
}

template <typename T, typename U>
inline void CreateSuccess(const T& printable, const U& printable2) {
    BoronLog::Print("\033[32m", "Success: ", printable, printable2);
}

template <typename T>
inline void CreateInfo(const T& printable) {
    BoronLog::Print("\033[1;30m", "Info: ", printable);
}

template <typename T, typename U>
inline void CreateInfo(const T& printable, const U& printable2) {
    BoronLog::Print("\033[1;30m", "Info: ", printable, printable2);
}

template <typename T>
inline void ProfilerInformation(const T& printable) {
    #if PRINTDEBUGINFO == 1
        #ifdef _DEBUG
            BoronLog::Print("\033[1;32m", "Profiler: ", printable);
        #endif
    #endif
}

template <typename T, typename U>
inline void ProfilerInformation(const T& printable, const U& printable2) {
    #if PRINTDEBUGINFO == 1
        #ifdef _DEBUG
            BoronLog::Print("\033[1;32m", "Profiler: ", printable, printable2);
        #endif
    #endif
}