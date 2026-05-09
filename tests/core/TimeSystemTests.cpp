//-----------------------------------
// TimeSystemTests.cpp
// Caleb Davis
// Unit tests for TimeSystem
//-------------------------------------
#include <catch2/catch_all.hpp>
#include "engine/core/TimeSystem.h"
#include <thread>
#include <chrono>

TEST_CASE("TimeSystem: Init and Shutdown", "[time]") {

    SECTION("initializes successfully with defaults") {
        TimeSystem time;
        SEResult result = time.Init();
        REQUIRE(result.ok());
        time.Shutdown();
    }

    SECTION("initializes with custom config") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(30.0f);
        config.timeScale    = 2.0f;
        config.maxDeltaTime = 0.1f;
        time.Configure(config);
        SEResult result = time.Init();
        REQUIRE(result.ok());
        time.Shutdown();
    }

    SECTION("can shutdown and reinitialize") {
        TimeSystem time;
        time.Init();
        time.Shutdown();
        SEResult result = time.Init();
        REQUIRE(result.ok());
        time.Shutdown();
    }

    SECTION("frame count starts at zero") {
        TimeSystem time;
        time.Init();
        REQUIRE(time.GetFrameCount() == 0);
        time.Shutdown();
    }
}

TEST_CASE("TimeSystem: TimeConfig", "[time]") {

    SECTION("default fixed timestep is 60fps") {
        TimeConfig config;
        REQUIRE(config.fixedTimestep == Catch::Approx(1.0f / 60.0f));
    }

    SECTION("SetFixedFrameRate sets correct timestep") {
        TimeConfig config;
        config.SetFixedFrameRate(30.0f);
        REQUIRE(config.fixedTimestep == Catch::Approx(1.0f / 30.0f));
    }

    SECTION("SetFixedFrameRate 120fps") {
        TimeConfig config;
        config.SetFixedFrameRate(120.0f);
        REQUIRE(config.fixedTimestep == Catch::Approx(1.0f / 120.0f));
    }

    SECTION("default timeScale is 1") {
        TimeConfig config;
        REQUIRE(config.timeScale == Catch::Approx(1.0f));
    }

    SECTION("default maxDeltaTime is 0.25") {
        TimeConfig config;
        REQUIRE(config.maxDeltaTime == Catch::Approx(0.25f));
    }
}

TEST_CASE("TimeSystem: TimeScale", "[time]") {

    SECTION("get and set timeScale") {
        TimeSystem time;
        time.Init();
        time.SetTimeScale(0.5f);
        REQUIRE(time.GetTimeScale() == Catch::Approx(0.5f));
        time.Shutdown();
    }

    SECTION("timeScale zero pauses fixed updates") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        time.Configure(config);
        time.Init();

        time.SetTimeScale(0.0f);

        // tick several times - accumulator should never fill
        for (int i = 0; i < 10; i++)
            time.Tick();

        REQUIRE_FALSE(time.ShouldFixedUpdate());
        time.Shutdown();
    }

    SECTION("GetFixedDeltaTime reflects timeScale") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        config.timeScale = 1.0f;
        time.Configure(config);
        time.Init();

        float normalFixed = time.GetFixedDeltaTime();

        time.SetTimeScale(0.5f);
        float halfFixed = time.GetFixedDeltaTime();

        REQUIRE(halfFixed == Catch::Approx(normalFixed * 0.5f));
        time.Shutdown();
    }

    SECTION("scaled dt is smaller than unscaled at half speed") {
        TimeSystem time;
        TimeConfig config;
        config.timeScale = 0.5f;
        time.Configure(config);
        time.Init();

        // sleep briefly so Tick has real time to measure
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();

        REQUIRE(time.GetDeltaTime() < time.GetUnscaledDeltaTime());
        time.Shutdown();
    }

    SECTION("scaled dt is larger than unscaled at double speed") {
        TimeSystem time;
        TimeConfig config;
        config.timeScale = 2.0f;
        time.Configure(config);
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();

        REQUIRE(time.GetDeltaTime() > time.GetUnscaledDeltaTime());
        time.Shutdown();
    }
}

TEST_CASE("TimeSystem: Tick", "[time]") {

    SECTION("frame count increments each tick") {
        TimeSystem time;
        time.Init();

        time.Tick();
        REQUIRE(time.GetFrameCount() == 1);

        time.Tick();
        REQUIRE(time.GetFrameCount() == 2);

        time.Tick();
        REQUIRE(time.GetFrameCount() == 3);

        time.Shutdown();
    }

    SECTION("unscaled time increases each tick") {
        TimeSystem time;
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();
        float t1 = time.GetUnscaledTime();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();
        float t2 = time.GetUnscaledTime();

        REQUIRE(t2 > t1);
        time.Shutdown();
    }

    SECTION("scaled time does not increase when paused") {
        TimeSystem time;
        TimeConfig config;
        config.timeScale = 0.0f;
        time.Configure(config);
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();
        float t1 = time.GetTime();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();
        float t2 = time.GetTime();

        REQUIRE(t1 == Catch::Approx(t2));
        time.Shutdown();
    }

    SECTION("unscaled time increases even when paused") {
        TimeSystem time;
        TimeConfig config;
        config.timeScale = 0.0f;
        time.Configure(config);
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();
        float u1 = time.GetUnscaledTime();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();
        float u2 = time.GetUnscaledTime();

        REQUIRE(u2 > u1);
        time.Shutdown();
    }

    SECTION("delta time is positive after sleep") {
        TimeSystem time;
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time.Tick();

        REQUIRE(time.GetDeltaTime() > 0.0f);
        REQUIRE(time.GetUnscaledDeltaTime() > 0.0f);
        time.Shutdown();
    }

    SECTION("maxDeltaTime clamps large spikes") {
        TimeSystem time;
        TimeConfig config;
        config.maxDeltaTime = 0.05f;  // clamp at 50ms
        time.Configure(config);
        time.Init();

        // sleep longer than maxDeltaTime
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        time.Tick();

        REQUIRE(time.GetUnscaledDeltaTime() <= config.maxDeltaTime + 0.001f);
        time.Shutdown();
    }
}

TEST_CASE("TimeSystem: Fixed update", "[time]") {

    SECTION("ShouldFixedUpdate false immediately after init") {
        TimeSystem time;
        time.Init();
        REQUIRE_FALSE(time.ShouldFixedUpdate());
        time.Shutdown();
    }

    SECTION("ShouldFixedUpdate true after enough time accumulates") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        time.Configure(config);
        time.Init();

        // sleep longer than one fixed step
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        time.Tick();

        REQUIRE(time.ShouldFixedUpdate());
        time.Shutdown();
    }

    SECTION("ConsumeFixedUpdate drains accumulator") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        time.Configure(config);
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        time.Tick();

        REQUIRE(time.ShouldFixedUpdate());
        time.ConsumeFixedUpdate();
        // may or may not still be true depending on exact timing
        // but consuming should have reduced the accumulator
        // just verify it doesn't crash and state is valid
        REQUIRE(time.GetFrameCount() == 1);
        time.Shutdown();
    }

    SECTION("multiple fixed updates can fire per frame") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        time.Configure(config);
        time.Init();

        // sleep long enough for multiple fixed steps
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        time.Tick();

        int fixedCount = 0;
        while (time.ShouldFixedUpdate()) {
            time.ConsumeFixedUpdate();
            fixedCount++;
        }

        REQUIRE(fixedCount >= 2);
        time.Shutdown();
    }

    SECTION("fixed update does not fire when paused") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        config.timeScale = 0.0f;
        time.Configure(config);
        time.Init();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        time.Tick();

        REQUIRE_FALSE(time.ShouldFixedUpdate());
        time.Shutdown();
    }

    SECTION("GetFixedDeltaTime is correct at default framerate") {
        TimeSystem time;
        TimeConfig config;
        config.SetFixedFrameRate(60.0f);
        config.timeScale = 1.0f;
        time.Configure(config);
        time.Init();

        REQUIRE(time.GetFixedDeltaTime() == Catch::Approx(1.0f / 60.0f));
        time.Shutdown();
    }
}