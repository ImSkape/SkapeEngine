//-----------------------------------
// PlatformCapabilities.h
// Caleb Davis
// Defines hardware restrictions
//-------------------------------------

#pragma once
#include <cstdint>

enum class DeviceState : uint8_t {
    Unavailable,  // hardware doesn't exist on this platform
    Available,    // hardware exists but not connected/active
    Active        // hardware exists and currently connected
};

enum class ControllerType : uint8_t {
    Unknown,
    Xbox,
    PlayStation,
    Nintendo,
    Generic
};

struct ControllerState {
    DeviceState   state = DeviceState::Unavailable;
    ControllerType type = ControllerType::Unknown;
    int           glfwJoystickId = -1;  // -1 = not connected
};

struct PlatformCapabilities {
    // input devices
    DeviceState keyboard = DeviceState::Unavailable;
    DeviceState mouse = DeviceState::Unavailable;
    DeviceState touch = DeviceState::Unavailable;
    DeviceState accelerometer = DeviceState::Unavailable;
    DeviceState gyroscope = DeviceState::Unavailable;

    // controllers - up to 4
    ControllerState controllers[4];

    // display
    bool isFullscreenOnly = false;  // mobile/console - no windowed mode
    bool hasPhysicalBack = false;  // Android back button

    // platform flags
    bool isDesktop = false;
    bool isMobile = false;
    bool isConsole = false;
    bool isVR = false;

    // helper queries
    bool HasAnyController() const {
        for (const auto& c : controllers)
            if (c.state == DeviceState::Active) return true;
        return false;
    }

    int ActiveControllerCount() const {
        int count = 0;
        for (const auto& c : controllers)
            if (c.state == DeviceState::Active) count++;
        return count;
    }

    // static factory - detects initial capabilities from CMake platform defines
    static PlatformCapabilities Detect();
};