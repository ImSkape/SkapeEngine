//-----------------------------------
// MemorySystemTests.cpp
// Caleb Davis
// Unit tests for MemorySystem.cpp
//-------------------------------------
#include <catch2/catch_all.hpp>
#include "engine/core/memory/MemorySystem.h"

TEST_CASE("MemorySystem: Init and Shutdown", "[memory][system]") {

    SECTION("initializes with default config") {
        MemorySystem memory;
        SEResult result = memory.Init();
        REQUIRE(result.ok());
        memory.Shutdown();
    }

    SECTION("initializes with custom config") {
        MemorySystem memory;
        MemoryConfig config;
        config.frameSize   = 1 * 1024 * 1024;
        config.ecsSize     = 2 * 1024 * 1024;
        config.assetSize   = 2 * 1024 * 1024;
        config.renderSize  = 1 * 1024 * 1024;
        config.audioSize   = 1 * 1024 * 1024;
        config.generalSize = 2 * 1024 * 1024;
        config.stackSize   = 1 * 1024 * 1024;
        memory.Configure(config);
        SEResult result = memory.Init();
        REQUIRE(result.ok());
        memory.Shutdown();
    }

    SECTION("shutdown cleans up all arenas") {
        MemorySystem memory;
        memory.Init();
        memory.Shutdown();

        // after shutdown all arenas should report zero capacity
        // re-init should work cleanly
        SEResult result = memory.Init();
        REQUIRE(result.ok());
        memory.Shutdown();
    }
}

TEST_CASE("MemorySystem: Arena access", "[memory][system]") {
    MemorySystem memory;
    MemoryConfig config;
    config.frameSize   = 1 * 1024 * 1024;
    config.ecsSize     = 1 * 1024 * 1024;
    config.assetSize   = 1 * 1024 * 1024;
    config.renderSize  = 1 * 1024 * 1024;
    config.audioSize   = 1 * 1024 * 1024;
    config.generalSize = 1 * 1024 * 1024;
    config.stackSize   = 1 * 1024 * 1024;
    memory.Configure(config);
    memory.Init();

    SECTION("can get Frame arena") {
        ArenaAllocator& arena = memory.GetArena(MemoryCategory::Frame);
        REQUIRE(arena.GetCapacity() > 0);
    }

    SECTION("can get ECS arena") {
        ArenaAllocator& arena = memory.GetArena(MemoryCategory::ECS);
        REQUIRE(arena.GetCapacity() > 0);
    }

    SECTION("can get Asset arena") {
        ArenaAllocator& arena = memory.GetArena(MemoryCategory::Asset);
        REQUIRE(arena.GetCapacity() > 0);
    }

    SECTION("can get Render arena") {
        ArenaAllocator& arena = memory.GetArena(MemoryCategory::Render);
        REQUIRE(arena.GetCapacity() > 0);
    }

    SECTION("can get Audio arena") {
        ArenaAllocator& arena = memory.GetArena(MemoryCategory::Audio);
        REQUIRE(arena.GetCapacity() > 0);
    }

    SECTION("can get General arena") {
        ArenaAllocator& arena = memory.GetArena(MemoryCategory::General);
        REQUIRE(arena.GetCapacity() > 0);
    }

    SECTION("arenas are independent") {
        ArenaAllocator& frame   = memory.GetArena(MemoryCategory::Frame);
        ArenaAllocator& general = memory.GetArena(MemoryCategory::General);

        frame.Alloc(256);
        REQUIRE(frame.GetUsed()   >= 256);
        REQUIRE(general.GetUsed() == 0);
    }

    SECTION("can allocate from each arena") {
        for (int i = 0; i < (int)MemoryCategory::Count; i++) {
            ArenaAllocator& arena = memory.GetArena((MemoryCategory)i);
            void* ptr = arena.Alloc(64);
            REQUIRE(ptr != nullptr);
        }
    }

    memory.Shutdown();
}

TEST_CASE("MemorySystem: Stack access", "[memory][system]") {
    MemorySystem memory;
    MemoryConfig config;
    config.stackSize = 1 * 1024 * 1024;
    memory.Configure(config);
    memory.Init();

    SECTION("can get stack allocator") {
        StackAllocator& stack = memory.GetStack();
        REQUIRE(stack.GetCapacity() > 0);
    }

    SECTION("can allocate from stack") {
        StackAllocator& stack = memory.GetStack();
        void* ptr = stack.Alloc(64);
        REQUIRE(ptr != nullptr);
    }

    SECTION("stack marker works through system") {
        StackAllocator& stack = memory.GetStack();
        size_t marker = stack.GetMarker();
        stack.Alloc(256);
        stack.FreeToMarker(marker);
        REQUIRE(stack.GetUsed() == marker);
    }

    memory.Shutdown();
}

TEST_CASE("MemorySystem: ResetFrameArenas", "[memory][system]") {
    MemorySystem memory;
    MemoryConfig config;
    config.frameSize   = 1 * 1024 * 1024;
    config.generalSize = 1 * 1024 * 1024;
    memory.Configure(config);
    memory.Init();

    SECTION("resets frame arena") {
        ArenaAllocator& frame = memory.GetArena(MemoryCategory::Frame);
        frame.Alloc(256);
        REQUIRE(frame.GetUsed() >= 256);
        memory.ResetFrameArenas();
        REQUIRE(frame.GetUsed() == 0);
    }

    SECTION("does not reset general arena") {
        ArenaAllocator& frame   = memory.GetArena(MemoryCategory::Frame);
        ArenaAllocator& general = memory.GetArena(MemoryCategory::General);

        frame.Alloc(256);
        general.Alloc(256);

        memory.ResetFrameArenas();

        REQUIRE(frame.GetUsed()   == 0);
        REQUIRE(general.GetUsed() >= 256);
    }

    SECTION("does not reset ECS arena") {
        ArenaAllocator& frame = memory.GetArena(MemoryCategory::Frame);
        ArenaAllocator& ecs   = memory.GetArena(MemoryCategory::ECS);

        frame.Alloc(256);
        ecs.Alloc(256);

        memory.ResetFrameArenas();

        REQUIRE(frame.GetUsed() == 0);
        REQUIRE(ecs.GetUsed()   >= 256);
    }

    SECTION("can allocate from frame arena after reset") {
        ArenaAllocator& frame = memory.GetArena(MemoryCategory::Frame);
        frame.Alloc(512 * 1024);
        memory.ResetFrameArenas();
        void* ptr = frame.Alloc(512 * 1024);
        REQUIRE(ptr != nullptr);
    }

    SECTION("multiple resets work correctly") {
        ArenaAllocator& frame = memory.GetArena(MemoryCategory::Frame);
        for (int i = 0; i < 10; i++) {
            frame.Alloc(1024);
            REQUIRE(frame.GetUsed() >= 1024);
            memory.ResetFrameArenas();
            REQUIRE(frame.GetUsed() == 0);
        }
    }

    memory.Shutdown();
}

TEST_CASE("MemorySystem: CreatePool", "[memory][system]") {
    MemorySystem memory;
    memory.Init();

    SECTION("can create and use a pool") {
        struct Entity { int id = 0; float x = 0.0f; float y = 0.0f; };
        auto pool = memory.CreatePool<Entity>(64);

        Entity* e = pool.Alloc();
        REQUIRE(e != nullptr);
        e->id = 1;
        e->x  = 10.0f;
        e->y  = 20.0f;
        REQUIRE(e->id == 1);
        REQUIRE(e->x  == Catch::Approx(10.0f));
        REQUIRE(e->y  == Catch::Approx(20.0f));
        pool.Free(e);
        pool.Shutdown();
    }

    SECTION("pool capacity matches requested count") {
        auto pool = memory.CreatePool<int>(32);
        REQUIRE(pool.GetCapacity() == 32);
        pool.Shutdown();
    }

    memory.Shutdown();
}