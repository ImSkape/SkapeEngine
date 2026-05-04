//-----------------------------------
// Engine.cpp
// Caleb Davis
// First engine file made, mainly for testing
//-------------------------------------
#include "engine/core/Engine.h"
#include "engine/platform/glfw/WindowGlfw.h"
#include "engine/core/SEResult.h"
#include "glad/glad.h"
#include <cstdio>
#include <memory>

int EngineRun()
 { 
    std::unique_ptr<Window> window = std::make_unique<WindowGlfw>();

    /* Initialize the library */
    SEResult result = window->Init(640, 480, "Hello World");
    if (!result) {
        printf("[Fatal] %s\n", result.message.c_str());
        return -1;
    }
    ENGINE_ASSERT(result, result.message.c_str());

    /* Loop until the user closes the window */
    while (!window->ShouldClose())
    {

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


        window->SwapBuffers();
        window->PollEvents();
    }

    window->Shutdown();
    return 0;
 }
