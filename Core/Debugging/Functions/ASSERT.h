#pragma once
#include "ErrorHandling/ErrorMessage.h"
#include <format>
#include <string>

#ifdef _DEBUG
#define ASSERT(condition, message) \
        if (!(condition)) { \
            std::string AssertString = std::format( \
                "ASSERT failed because {} was null/nullptr - {}", \
                #condition, message); \
            MakeAError(AssertString); \
        }
#else
#define ASSERT(condition, message)
#endif