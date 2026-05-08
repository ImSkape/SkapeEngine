//-----------------------------------
// SEResult.h
// Caleb Davis
// Result handling with error information
//-------------------------------------

#pragma once
#include "SECore.h"
#include <cstdlib>

#ifdef ENGINE_DEBUG
#define ENGINE_ASSERT(condition, message) \
    if (!(condition)) { \
        printf("[Assert] %s\n", message); \
        SE_DEBUGBREAK(); \
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