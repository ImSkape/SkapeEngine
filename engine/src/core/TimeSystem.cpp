//-----------------------------------
// TimeSystem.cpp
// Caleb Davis
// Handles engine time functions
//-------------------------------------

#include "engine/core/TimeSystem.h"
#include <cstdio>
#include <algorithm>

// platform time includes
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
#include <GLFW/glfw3.h>
#elif defined(PLATFORM_ANDROID)
#include <time.h>
#elif defined(PLATFORM_IOS)
#include <mach/mach_time.h>
#endif

void TimeSystem::Configure(const TimeConfig& config) {
    m_config = config;
}


SEResult TimeSystem::Init() {
    m_lastTime = GetPlatformTime();
    m_time = 0.0f;
    m_unscaledTime = 0.0f;
    m_deltaTime = 0.0f;
    m_unscaledDeltaTime = 0.0f;
    m_accumulator = 0.0f;
    m_frameCount = 0;

    m_initialized = true;
    printf("[TimeSystem] Initialized - fixed timestep: %.4fs (%.1f fps) timeScale: %.1f\n",
        m_config.fixedTimestep,
        1.0f / m_config.fixedTimestep,
        m_config.timeScale);

    return SEResult::ok();
}

void TimeSystem::Shutdown() {
    m_initialized = false;
    printf("[TimeSystem] Shutdown\n");
}


void TimeSystem::Tick() {
    double currentTime = GetPlatformTime();


    // compute raw delta in double precision - then clamp and cast to float
    double rawDelta = currentTime - m_lastTime;
    m_lastTime = currentTime;

    // clamp and convert to float at the boundary
    m_unscaledDeltaTime = static_cast<float>(
        rawDelta < m_config.maxDeltaTime ? 
        rawDelta : m_config.maxDeltaTime);

    // scaled delta - affected by timeScale
    m_deltaTime = m_unscaledDeltaTime * m_config.timeScale;

    // accumulate time
    m_unscaledTime += m_unscaledDeltaTime;
    m_time += m_deltaTime;

    // accumulator uses unscaled dt but fixed step is scaled at read time
    // this means pausing (timeScale=0) stops fixed updates naturally
    m_accumulator += m_deltaTime;

    m_frameCount++;
}

bool TimeSystem::ShouldFixedUpdate() const {
    return m_accumulator >= m_config.fixedTimestep * m_config.timeScale;
}

void TimeSystem::ConsumeFixedUpdate() {
    m_accumulator -= m_config.fixedTimestep * m_config.timeScale;
}




double TimeSystem::GetPlatformTime() const {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    return glfwGetTime();

#elif defined(PLATFORM_ANDROID)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec) +
        static_cast<double>(ts.tv_nsec) / 1000000000.0;

#elif defined(PLATFORM_IOS)
    static mach_timebase_info_data_t info = { 0, 0 };
    if (info.denom == 0)
        mach_timebase_info(&info);
    uint64_t ticks = mach_absolute_time();
    return static_cast<double>(ticks) *
        static_cast<double>(info.numer) /
        static_cast<double>(info.denom) /
        1000000000.0;

#else
    return 0.0;
#endif
}