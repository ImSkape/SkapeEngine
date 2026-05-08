//-----------------------------------
// WindowSystem.h
// Caleb Davis
// Handles currect window for each platform
//-------------------------------------

#pragma once
#include "engine/core/System.h"
#include "engine/platform/Window.h"
#include <memory>
#include <string>

struct WindowConfig {
    int         width = 1280;
    int         height = 720;
    std::string title = "Skape Engine";
    bool        fullscreen = false;
    bool        vsync = true;
};

class WindowSystem : public System {
public:
    SEResult Init()          override;
    void     Shutdown()          override;
    void     Update(float dt)  override;  // PollEvents

    void Configure(const WindowConfig& config);

    // presentation — called by GFXContext::Present()
    void Present();

    // queries
    bool ShouldClose() const;
    int  GetWidth()    const;
    int  GetHeight()   const;
    bool IsFullscreen()const;
    bool IsFocused()   const;

    // native handle — RenderSystem casts to what it needs
    void* GetNativeHandle() const;

    const std::string& GetName() const override {
        static std::string name = "WindowSystem";
        return name;
    }

private:
    std::unique_ptr<Window> m_window;
    WindowConfig            m_config;
};