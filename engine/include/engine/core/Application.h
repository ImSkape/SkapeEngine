//-----------------------------------
// Application.h
// Caleb Davis
// First engine file made, mainly for testing
//-------------------------------------

#pragma once
#include "engine/platform/Window.h"
#include <memory>

class Application {
public:
    Application();
    virtual ~Application() = default;

    void Run();

protected:
    // -- INIT ------------------------------------------
    virtual void OnPreInit() {}  // before engine systems start
    // set config, override settings
    // nothing is loaded yet

    virtual void OnInit() {}  // engine systems are up
    // load assets, create entities
    // main setup lives here

    virtual void OnPostInit() {}  // everything is loaded and ready
    // spawn starting entities
    // start audio, trigger intro, etc.

// --- RUNTIME ------------------------------------------
    virtual void OnUpdate(float dt) {}  // variable timestep
    // input, camera, UI
    // anything that should
    // match frame rate

    virtual void OnFixedUpdate(float dt) {}  // fixed timestep
    // physics, networking
    // deterministic logic
    // called 0..N times per frame

    virtual void OnLateUpdate(float dt) {}  // after all updates
    // camera follow
    // animation finalize
    // read-only logic

    virtual void OnRender() {}  // submit draw calls
    // separate from update
    // never put logic here
    #ifdef ENGINE_EDITOR
    virtual void OnImGuiRender() {}  // editor/debug UI only
    // compiled out in Release
    #endif

// -- EVENTS ------------------------------------------
    virtual void OnWindowResize(int w, int h) {}  // rebuild framebuffers
    virtual void OnWindowFocus(bool focused) {}  // pause/resume logic
    virtual void OnFileDrop(const std::string& path) {} // asset drag-drop

    // -- SHUTDOWN ------------------------------------------
    virtual void OnPreShutdown() {}  // save state, flush async ops
    // last chance before teardown
    virtual void OnShutdown() {}  // destroy game objects
    // free your resources
    virtual void OnPostShutdown() {}  // engine systems shutting down
    // logging, telemetry flush

private:
    std::unique_ptr<Window> window;
    bool m_running = false;
};