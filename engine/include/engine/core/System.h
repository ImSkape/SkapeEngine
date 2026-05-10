//-----------------------------------
// System.h
// Caleb Davis
// Core system functions for systems to inherit
//-------------------------------------

#pragma once
#include "engine/core/SECore.h"
#include "engine/core/SEResult.h"
#include <string>

class System {
public:
   // Init() -> Update() -> FixedUpdate() -> LateUpdate() -> Render() -> Shutdown()
    virtual ~System() = default;
    //called on EngineInit
    virtual SEResult Init()     { return SEResult::ok(); }

    virtual void     Shutdown() {}

    virtual void     Update(float dt) { }
    virtual void     FixedUpdate(float dt) { }
    virtual void     LateUpdate(float dt) { }
    virtual void     Render() { }


    // name for debugging, logging, profiler display
    virtual const std::string& GetName() const = 0;

    bool IsInitialized() const { return m_initialized; }

protected:
    bool m_initialized = false;
};