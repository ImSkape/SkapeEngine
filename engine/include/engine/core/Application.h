//-----------------------------------
// Application.h
// Caleb Davis
// First engine file made, mainly for testing
//-------------------------------------

#pragma once
#include "engine/core/WindowSystem.h"
#include "engine/render/RenderSystem.h"
#include "engine/core/memory/MemorySystem.h"
#include "engine/core/JobSystem.h"
#include "engine/render/GPUBuffer.h"
#include "engine/render/GPUShader.h"
#include "engine/render/RenderPass.h"
#include "engine/core/ServiceLocator.h"
#include <memory>
class Application {
public:
    Application();
    void Run(const std::string& projectPath);
    void RunHeadless(const std::string& projectPath, int frames = 10);
    void Quit();

private:

    void InitEngine();
    void ShutdownEngine();

    // -- INIT ------------------------------------------
    void PreInit();  // before engine systems start
    // set config, override settings
    // nothing is loaded yet

    void Init();  // engine systems are up
    // load assets, create entities
    // main setup lives here

    void PostInit();  // everything is loaded and ready
    // spawn starting entities
    // start audio, trigger intro, etc.

// --- RUNTIME ------------------------------------------
    void Update(float dt);  // variable timestep
    // input, camera, UI
    // anything that should
    // match frame rate

    void FixedUpdate(float dt);  // fixed timestep
    // physics, networking
    // deterministic logic
    // called 0..N times per frame

    void LateUpdate(float dt);  // after all updates
    // camera follow
    // animation finalize
    // read-only logic

    void Render();  // submit draw calls
    // separate from update
    // never put logic here
    #ifdef ENGINE_EDITOR
    void ImGuiRender();  // editor/debug UI only
    // compiled out in Release
    #endif

    // -- EVENTS ------------------------------------------
    void WindowResize(int w, int h);  // rebuild framebuffers
    void WindowFocus(bool focused);  // pause/resume logic
    void FileDrop(const std::string& path); // asset drag-drop

    // -- SHUTDOWN ------------------------------------------
    void PreShutdown();  // save state, flush async ops
    // last chance before teardown
    void Shutdown();  // destroy game objects
    // free your resources
    void PostShutdown();  // engine systems shutting down
    // logging, telemetry flush

private:

    std::vector<System*> m_systems;
    MemorySystem m_memorySystem;
    WindowSystem m_windowSystem;
    RenderSystem m_renderSystem;
    JobSystem    m_jobSystem;
    bool m_running = false;

    bool m_headless = false;

    // temporary triangle test � remove when scene system exists
    std::unique_ptr<GPUBuffer>     m_triangleVBO;
    std::unique_ptr<GPUShader>     m_triangleShader;
    std::unique_ptr<CommandBuffer> m_cmd;
    std::unique_ptr<RenderPass>    m_mainPass;


    template<typename T>
    void RegisterSystem(T* system) {
        SEResult result = system->Init();
        if (!result) {
            printf("[Fatal] %s failed: %s\n",
                system->GetName().c_str(),
                result.message.c_str());
            return;
        }
        ServiceLocator::Register<T>(system);
        m_systems.push_back(system);
    }


};