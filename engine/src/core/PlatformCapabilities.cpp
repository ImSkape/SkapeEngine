//-----------------------------------
// PlatformCapabilities.cpp
// Caleb Davis
// Defines hardware restrictions
//-------------------------------------
#include "engine/core/PlatformCapabilities.h"

PlatformCapabilities PlatformCapabilities::Detect() {
    PlatformCapabilities caps;

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    caps.isDesktop = true;
    caps.keyboard = DeviceState::Active;
    caps.mouse = DeviceState::Active;
    caps.touch = DeviceState::Unavailable;
    caps.accelerometer = DeviceState::Unavailable;
    caps.gyroscope = DeviceState::Unavailable;
    caps.isFullscreenOnly = false;
    caps.hasPhysicalBack = false;

    // controllers start unavailable - InputSystem detects connections
    for (auto& c : caps.controllers) {
        c.state = DeviceState::Unavailable;
        c.type = ControllerType::Unknown;
        c.glfwJoystickId = -1;
    }

#elif defined(PLATFORM_ANDROID)
    caps.isMobile = true;
    caps.keyboard = DeviceState::Unavailable;
    caps.mouse = DeviceState::Unavailable;
    caps.touch = DeviceState::Active;
    caps.accelerometer = DeviceState::Active;
    caps.gyroscope = DeviceState::Active;
    caps.isFullscreenOnly = true;
    caps.hasPhysicalBack = true;

    for (auto& c : caps.controllers) {
        c.state = DeviceState::Unavailable;
        c.type = ControllerType::Unknown;
        c.glfwJoystickId = -1;
    }

#elif defined(PLATFORM_IOS)
    caps.isMobile = true;
    caps.keyboard = DeviceState::Unavailable;
    caps.mouse = DeviceState::Unavailable;
    caps.touch = DeviceState::Active;
    caps.accelerometer = DeviceState::Active;
    caps.gyroscope = DeviceState::Active;
    caps.isFullscreenOnly = true;
    caps.hasPhysicalBack = false;

    for (auto& c : caps.controllers) {
        c.state = DeviceState::Unavailable;
        c.type = ControllerType::Unknown;
        c.glfwJoystickId = -1;
    }

#elif defined(PLATFORM_CONSOLE)
    caps.isConsole = true;
    caps.keyboard = DeviceState::Unavailable;
    caps.mouse = DeviceState::Unavailable;
    caps.touch = DeviceState::Unavailable;
    caps.isFullscreenOnly = true;

    // consoles always have at least one controller slot active
    caps.controllers[0].state = DeviceState::Available;

#endif

    return caps;
}