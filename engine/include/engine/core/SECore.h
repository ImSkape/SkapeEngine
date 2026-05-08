//-----------------------------------
// SECre.h
// Caleb Davis
// Skape Engine Precompiled Header
//-------------------------------------

#pragma once


// standard library
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <cassert>

// suppress unused parameter warnings cross-platform
#define UNREFERENCED_PARAMETER(P) (void)(P)


// platform detection
#if defined(_WIN32)
#define PLATFORM_WINDOWS
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#define PLATFORM_IOS
#else
#define PLATFORM_MAC
#endif
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif


#if defined(PLATFORM_WINDOWS)
#define SE_DEBUGBREAK() __debugbreak()
#elif defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
#define SE_DEBUGBREAK() __builtin_trap()
#else
#define SE_DEBUGBREAK() ((void)0)
#endif


#ifdef PLATFORM_MAC
    // GL_SILENCE_DEPRECATION is set via CMake
    // no need to define it here
#endif