//-----------------------------------
// Application.h
// Caleb Davis
// First engine file made, mainly for testing
//-------------------------------------

#pragma once
#include "engine/platform/Window.h"
#include "engine/renderer/Renderer.h"
#include <memory>

#include "engine/renderer/GPUBuffer.h"
#include "engine/renderer/GPUShader.h"
#include "engine/renderer/CommandBuffer.h"
#include "engine/renderer/RenderPass.h"

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
    std::unique_ptr<Window> m_window;
    Renderer m_renderer;
    bool m_running = false;

    bool m_headless = false;

    // temporary triangle test — remove when scene system exists
    std::unique_ptr<GPUBuffer>     m_triangleVBO;
    std::unique_ptr<GPUShader>     m_triangleShader;
    std::unique_ptr<CommandBuffer> m_cmd;
    std::unique_ptr<RenderPass>    m_mainPass;

};