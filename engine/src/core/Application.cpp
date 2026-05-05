//-----------------------------------
// Application.cpp
// Caleb Davis
// Main game loop for engine
//-------------------------------------
#include "engine/core/Application.h"
#include "engine/platform/glfw/WindowGlfw.h"
#include "engine/core/SEResult.h"
// TODO: remove — glClear moves to Renderer::BeginFrame(), 
//       glfwGetTime() moves to platform Time abstraction
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <cstdio>
#include <memory>

Application::Application()
{
    window = std::make_unique<WindowGlfw>();
}

void Application::Run()
{
    // before anything starts - config, settings overrides
    OnPreInit();

    /* Initialize the library */
    SEResult result = window->Init(640, 480, "Hello World");
    if (!result) {
        printf("[Fatal] %s\n", result.message.c_str());
        return;
    }

    // renderer, audio, input etc will init here later
    OnInit();

    // everything is up, spawn entities, start audio, etc
    OnPostInit();


    // TIMING SETUP
    float fixedTimestep = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float lastTime = (float)glfwGetTime();

    m_running = true;
    // Loop until the user closes the window
    while (m_running && !window->ShouldClose())
    {
        //  DELTA TIME
        float currentTime = (float)glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;


        // clamp so debugger pauses don't explode physics
        dt = std::min(dt, 0.25f);
        accumulator += dt;

        // FIXED UPDATE
        while (accumulator >= fixedTimestep) {
            OnFixedUpdate(fixedTimestep);
            accumulator -= fixedTimestep;
        }

        // UPDATE
        // input, camera, game logic — variable timestep
        OnUpdate(dt);

        //  LATE UPDATE
        // camera follow, animation finalize, read-only
        OnLateUpdate(dt);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        OnRender();
        
        #ifdef ENGINE_EDITOR
            OnImGuiRender();
        #endif
        // PRESENT
        window->SwapBuffers();
        window->PollEvents();
    }

    // save state, flush anything in flight
    OnPreShutdown();

    // engine systems tear down — logging flush etc
    OnShutdown();

    //Anything left hanging
    OnPostShutdown();

    window->Shutdown();
}
