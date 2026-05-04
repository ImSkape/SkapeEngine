//-----------------------------------
// SEResult.h
// Caleb Davis
// Result handling with error information
//-------------------------------------

#pragma once
#include <string>
#include <cstdio>
#include <cstdlib>

#ifdef ENGINE_DEBUG
#define ENGINE_ASSERT(condition, msg) \
    if (!(condition)) { \
        printf("[ASSERT FAILED] %s\n  File: %s\n  Line: %d\n", \
               msg, __FILE__, __LINE__); \
        __debugbreak(); \
    }
#else
#define ENGINE_ASSERT(condition, msg) // compiled out in Release
#endif


struct SEResult {
    bool success;
    std::string message;

    static SEResult ok() {
        return { true, "" };
    }

    static SEResult fail(const std::string& msg) {
        return { false, msg };
    }

    operator bool() const { return success; }
};