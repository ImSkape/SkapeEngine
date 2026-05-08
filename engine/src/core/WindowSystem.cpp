//-----------------------------------
// WindowSystem.cpp
// Caleb Davis
// Handles currect window for each platform
//-------------------------------------

#include "engine/core/WindowSystem.h"
#include "engine/core/SEResult.h"
#include <cstdio>

// platform window includes
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
#include "engine/platform/glfw/WindowGlfw.h"
#elif defined(PLATFORM_ANDROID)
#include "engine/platform/android/WindowAndroid.h"
#elif defined(PLATFORM_IOS)
#include "engine/platform/ios/WindowIOS.h"
#endif

void WindowSystem::Configure(const WindowConfig& config) {
    m_config = config;
}

SEResult WindowSystem::Init() {
    // create correct window implementation for this platform
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    m_window = std::make_unique<WindowGlfw>();
#elif defined(PLATFORM_ANDROID)
    m_window = std::make_unique<WindowAndroid>();
#elif defined(PLATFORM_IOS)
    m_window = std::make_unique<WindowIOS>();
#else
    static_assert(false, "No window platform defined — set PLATFORM_X in CMake");
#endif

    SEResult result = m_window->Init(
        m_config.width,
        m_config.height,
        m_config.title
    );
    if (!result) return result;

    m_initialized = true;
    printf("[WindowSystem] %dx%d vsync:%s\n",
        m_config.width,
        m_config.height,
        m_config.vsync ? "on" : "off");
    return SEResult::ok();
}

void WindowSystem::Shutdown() {
    if (m_window)
        m_window->Shutdown();
    m_initialized = false;
}

void WindowSystem::Update(float dt) {
    if (m_window)
        m_window->PollEvents();
}

void WindowSystem::Present() {
    if (m_window)
        m_window->SwapBuffers();
}

bool WindowSystem::ShouldClose() const {
    return m_window ? m_window->ShouldClose() : true;
}

int WindowSystem::GetWidth() const {
    return m_window ? m_window->GetWidth() : m_config.width;
}

int WindowSystem::GetHeight() const {
    return m_window ? m_window->GetHeight() : m_config.height;
}
bool WindowSystem::IsFullscreen() const {
    return m_config.fullscreen;
}

bool WindowSystem::IsFocused() const {
    return m_window ? m_window->IsFocused() : false;
}

void* WindowSystem::GetNativeHandle() const {
    return m_window ? m_window->GetNativeHandle() : nullptr;
}