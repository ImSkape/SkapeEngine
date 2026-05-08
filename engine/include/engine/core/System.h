//-----------------------------------
// System.h
// Caleb Davis
// Core system functions for systems to inherit
//-------------------------------------

#pragma once
#include "engine/core/SEResult.h"
#include <string>

class System {
public:
    virtual ~System() = default;

    virtual SEResult Init()     { return SEResult::ok(); }
    virtual void     Shutdown() {}
    virtual void     Update(float dt) {}

    // name for debugging, logging, profiler display
    virtual const std::string& GetName() const = 0;

    bool IsInitialized() const { return m_initialized; }

protected:
    bool m_initialized = false;
};