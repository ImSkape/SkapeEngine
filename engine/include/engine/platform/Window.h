//-----------------------------------
// Window.h
// Caleb Davis
// Pure Virtual Window API
//-------------------------------------
#pragma once
#include <string>
class SEResult;
class Window {
public:
    virtual ~Window() = default;

    virtual SEResult Init(int width, int height, const std::string& title) = 0;
    virtual void PollEvents() = 0;
    virtual bool ShouldClose() = 0;
    virtual void SwapBuffers() = 0;
    virtual void Shutdown() = 0;
};