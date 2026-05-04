//-----------------------------------
// WindowGlfw.h
// Caleb Davis
// glfw window implementation
//-------------------------------------

#pragma once
#include <engine/platform/Window.h>
struct GLFWwindow;


class WindowGlfw : public Window {
public:
    SEResult Init(int width, int height, const std::string& title) override;
    void PollEvents() override;
    bool ShouldClose() override;
    void SwapBuffers() override;
    void Shutdown() override;

private:
    GLFWwindow* window = nullptr;
};