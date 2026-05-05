//-----------------------------------
// Application.cpp
// Caleb Davis
// Main game loop for engine
//-------------------------------------
#include "engine/core/Application.h"
#include "engine/platform/glfw/WindowGlfw.h"
#include "engine/core/SEResult.h"
#include "engine/core/ServiceLocator.h"
// TODO: remove — glClear moves to Renderer::BeginFrame(), 
//       glfwGetTime() moves to platform Time abstraction
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "engine/renderer/Renderer.h"
#include <cstdio>
#include <memory>

Application::Application()
{
    m_window = std::make_unique<WindowGlfw>();
}

void Application::Run(const std::string& projectPath)
{
    InitEngine();

    // before anything starts - config, settings overrides
    PreInit();



    // renderer, audio, input etc will init here later
    Init();

    // everything is up, spawn entities, start audio, etc
    PostInit();


    // TIMING SETUP
    float fixedTimestep = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float lastTime = (float)glfwGetTime();

    m_running = true;
    // Loop until the user closes the window
    while (m_running && !m_window->ShouldClose())
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
            FixedUpdate(fixedTimestep);
            accumulator -= fixedTimestep;
        }

        // UPDATE
        // input, camera, game logic — variable timestep
        Update(dt);

        //  LATE UPDATE
        // camera follow, animation finalize, read-only
        LateUpdate(dt);

        Render();
        
        #ifdef ENGINE_EDITOR
            ImGuiRender();
        #endif
        // PRESENT
            m_window->PollEvents();
    }

    // save state, flush anything in flight
    PreShutdown();

    // engine systems tear down — logging flush etc
    Shutdown();

    //Anything left hanging
    PostShutdown();



    ShutdownEngine();
}




void Application::InitEngine()
{
    /* Initialize the library */
    SEResult result = m_window->Init(1280, 720, "Skape Engine");
    if (!result) {
        printf("[Fatal] %s\n", result.message.c_str());
        return;
    }


    SEResult rendererResult = m_renderer.Init(static_cast<WindowGlfw*>(m_window.get())->GetGLFWWindow());
    if (!rendererResult) {
        printf("[Fatal] %s\n", rendererResult.message.c_str());
        return;
    }
}

void Application::ShutdownEngine()
{
    m_renderer.Shutdown();
    m_window->Shutdown();
}

void Application::PreInit()
{
}

void Application::Init()
{

}

void Application::PostInit()
{
}

void Application::Update(float dt)
{
}

void Application::FixedUpdate(float dt)
{
}

void Application::LateUpdate(float dt)
{
}

void Application::Render()
{
    m_renderer.BeginFrame();
    // scene rendering goes here later
    m_renderer.EndFrame();
}




#ifdef ENGINE_EDITOR
void Application::ImGuiRender()
{
}
#endif


void Application::PreShutdown()
{
}

void Application::Shutdown()
{
    ServiceLocator::Clear();
}

void Application::PostShutdown()
{
}