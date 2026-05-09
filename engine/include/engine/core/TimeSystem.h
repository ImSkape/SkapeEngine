//-----------------------------------
// TimeSystem.h
// Caleb Davis
// Handles engine time functions
//-------------------------------------

#pragma once
#include "engine/core/System.h"
#include <string>
#include <cstdint>

struct TimeConfig {
    float fixedTimestep = 1.0f / 60.0f;  // how often FixedUpdate is called
    float maxDeltaTime = 0.25f;          // clamp - prevents debugger pauses exploding physics
    float timeScale = 1.0f;           // 0 = paused, 0.5 = half speed, 2.0 = double speed
    void SetFixedFrameRate(const float frameRate) {
        fixedTimestep = 1.0f / frameRate;
    }
};

class TimeSystem : public System {
public:
    SEResult Init() override;
    void     Shutdown() override;

    void Configure(const TimeConfig& config);

    // called once per frame at the start of the loop
    // updates all internal timing state
    void Tick();

    // fixed update control
    bool ShouldFixedUpdate() const;   // true when accumulator >= fixedTimestep
    void ConsumeFixedUpdate();        // drains one fixed step from accumulator

    // scaled time - affected by timeScale
    float GetDeltaTime()      const { return m_deltaTime; }
    float GetFixedDeltaTime() const { return m_config.fixedTimestep * m_config.timeScale; }
    float GetTime()           const { return m_time; }

    // unscaled time - real wall clock, ignores timeScale
    float GetUnscaledDeltaTime() const { return m_unscaledDeltaTime; }
    float GetUnscaledTime()      const { return m_unscaledTime; }

    // time scale control
    void  SetTimeScale(float scale) { m_config.timeScale = scale; }
    float GetTimeScale()      const { return m_config.timeScale; }

    // frame count since engine start
    uint64_t GetFrameCount() const { return m_frameCount; }

    const std::string& GetName() const override {
        static std::string name = "TimeSystem";
        return name;
    }

private:
    TimeConfig m_config;

    float    m_deltaTime = 0.0f;  // scaled dt
    float    m_unscaledDeltaTime = 0.0f;  // real dt
    float    m_time = 0.0f;  // scaled time since start
    float    m_unscaledTime = 0.0f;  // real time since start
    float    m_accumulator = 0.0f;  // fixed update accumulator
    double    m_lastTime = 0.0f;  // raw platform time last frame
    uint64_t m_frameCount = 0;     // frames since start

    double GetPlatformTime() const;  // platform specific time query
};