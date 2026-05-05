//-----------------------------------
// Application.h
// Caleb Davis
// First engine file made, mainly for testing
//-------------------------------------

#pragma once
#include "engine/platform/Window.h"
#include "engine/renderer/Renderer.h"
#include <memory>

class Application {
public:
    Application();
    void Run(const std::string& projectPath);
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
};