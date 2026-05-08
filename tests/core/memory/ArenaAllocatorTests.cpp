//-----------------------------------
// ArenaAllocatorTests.cpp
// Caleb Davis
// Unit tests for ArenaAllocator
//-------------------------------------
#include <catch2/catch_all.hpp>
#include "engine/core/memory/ArenaAllocator.h"


TEST_CASE("ArenaAllocator: Init and Shutdown", "[memory][arena]") {

    SECTION("initializes with correct capacity") {
        ArenaAllocator arena;
        arena.Init(1024);
        REQUIRE(arena.GetCapacity() == 1024);
        REQUIRE(arena.GetUsed() == 0);
        arena.Shutdown();
    }

    SECTION("shutdown zeroes state") {
        ArenaAllocator arena;
        arena.Init(1024);
        arena.Shutdown();
        REQUIRE(arena.GetCapacity() == 0);
        REQUIRE(arena.GetUsed() == 0);
    }
}

TEST_CASE("ArenaAllocator: Allocation", "[memory][arena]") {
    ArenaAllocator arena;
    arena.Init(1024);

    SECTION("returns non-null pointer") {
        void* ptr = arena.Alloc(64);
        REQUIRE(ptr != nullptr);
    }

    SECTION("advances used bytes") {
        arena.Alloc(64);
        REQUIRE(arena.GetUsed() >= 64);
    }

    SECTION("returns nullptr when out of memory") {
        void* ptr = arena.Alloc(2048);
        REQUIRE(ptr == nullptr);
    }

    SECTION("sequential allocations do not overlap") {
        void* a = arena.Alloc(64);
        void* b = arena.Alloc(64);
        uintptr_t addrA = reinterpret_cast<uintptr_t>(a);
        uintptr_t addrB = reinterpret_cast<uintptr_t>(b);
        REQUIRE(addrB >= addrA + 64);
    }

    SECTION("typed alloc returns correct pointer") {
        float* f = arena.Alloc<float>(4);
        REQUIRE(f != nullptr);
        f[0] = 1.0f;
        f[1] = 2.0f;
        f[2] = 3.0f;
        f[3] = 4.0f;
        REQUIRE(f[0] == 1.0f);
        REQUIRE(f[3] == 4.0f);
    }

    arena.Shutdown();
}

TEST_CASE("ArenaAllocator: Alignment", "[memory][arena]") {
    ArenaAllocator arena;
    arena.Init(1024);

    SECTION("default alignment of 8") {
        arena.Alloc(1);  // misalign offset deliberately
        void* ptr = arena.Alloc(8);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 8) == 0);
    }

    SECTION("alignment of 4") {
        arena.Alloc(1);
        void* ptr = arena.Alloc(4, 4);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 4) == 0);
    }

    SECTION("alignment of 16") {
        arena.Alloc(1);
        void* ptr = arena.Alloc(16, 16);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 16) == 0);
    }

    SECTION("alignment of 64") {
        arena.Alloc(1);
        void* ptr = arena.Alloc(64, 64);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 64) == 0);
    }

    arena.Shutdown();
}

TEST_CASE("ArenaAllocator: Reset", "[memory][arena]") {
    ArenaAllocator arena;
    arena.Init(1024);

    SECTION("reset brings used back to zero") {
        arena.Alloc(256);
        REQUIRE(arena.GetUsed() > 0);
        arena.Reset();
        REQUIRE(arena.GetUsed() == 0);
    }

    SECTION("can allocate again after reset") {
        arena.Alloc(512);
        arena.Reset();
        void* ptr = arena.Alloc(512);
        REQUIRE(ptr != nullptr);
    }

    SECTION("capacity unchanged after reset") {
        arena.Alloc(256);
        arena.Reset();
        REQUIRE(arena.GetCapacity() == 1024);
    }

    arena.Shutdown();
}

TEST_CASE("ArenaAllocator: Contains", "[memory][arena]") {
    ArenaAllocator arena;
    arena.Init(1024);

    SECTION("pointer inside arena returns true") {
        void* ptr = arena.Alloc(64);
        REQUIRE(arena.Contains(ptr));
    }

    SECTION("null pointer returns false") {
        REQUIRE_FALSE(arena.Contains(nullptr));
    }

    SECTION("pointer outside arena returns false") {
        int external = 42;
        REQUIRE_FALSE(arena.Contains(&external));
    }

    SECTION("pointer from different arena returns false") {
        ArenaAllocator other;
        other.Init(1024);
        void* ptr = other.Alloc(64);
        REQUIRE_FALSE(arena.Contains(ptr));
        other.Shutdown();
    }

    arena.Shutdown();
}

TEST_CASE("ArenaAllocator: Fill capacity", "[memory][arena]") {
    ArenaAllocator arena;
    arena.Init(256);

    SECTION("can fill entire arena") {
        void* ptr = arena.Alloc(256);
        REQUIRE(ptr != nullptr);
        REQUIRE(arena.GetUsed() == 256);
    }

    SECTION("cannot exceed capacity") {
        arena.Alloc(256);
        void* overflow = arena.Alloc(1);
        REQUIRE(overflow == nullptr);
    }

    arena.Shutdown();
}