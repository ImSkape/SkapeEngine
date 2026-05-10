//-----------------------------------
// PlatformCapabilitiesTests.cpp
// Caleb Davis
// Unit tests for PlatformCapabilities
//-------------------------------------
#include <catch2/catch_all.hpp>
#include "engine/core/PlatformCapabilities.h"


TEST_CASE("PlatformCapabilities: Detect", "[platform]") {

    SECTION("detects correctly on current platform") {
        auto caps = PlatformCapabilities::Detect();

        #if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
            REQUIRE(caps.isDesktop);
            REQUIRE(caps.keyboard == DeviceState::Active);
            REQUIRE(caps.mouse    == DeviceState::Active);
            REQUIRE(caps.touch    == DeviceState::Unavailable);
            REQUIRE_FALSE(caps.isMobile);
            REQUIRE_FALSE(caps.isConsole);
        #elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            REQUIRE(caps.isMobile);
            REQUIRE(caps.touch    == DeviceState::Active);
            REQUIRE(caps.keyboard == DeviceState::Unavailable);
        #endif
    }

    SECTION("controllers start unavailable on desktop") {
        auto caps = PlatformCapabilities::Detect();
        #if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
            REQUIRE_FALSE(caps.HasAnyController());
            REQUIRE(caps.ActiveControllerCount() == 0);
        #endif
    }

    SECTION("HasAnyController false with no controllers") {
        PlatformCapabilities caps;
        REQUIRE_FALSE(caps.HasAnyController());
    }

    SECTION("HasAnyController true when one active") {
        PlatformCapabilities caps;
        caps.controllers[0].state = DeviceState::Active;
        REQUIRE(caps.HasAnyController());
    }

    SECTION("ActiveControllerCount correct") {
        PlatformCapabilities caps;
        caps.controllers[0].state = DeviceState::Active;
        caps.controllers[2].state = DeviceState::Active;
        REQUIRE(caps.ActiveControllerCount() == 2);
    }
}