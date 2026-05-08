//-----------------------------------
// StackAllocatorTests.cpp
// Caleb Davis
// Unit tests for StackAllocator
//-------------------------------------
#include <catch2/catch_all.hpp>
#include "engine/core/memory/StackAllocator.h"

TEST_CASE("StackAllocator: Init and Shutdown", "[memory][stack]") {

    SECTION("initializes with correct capacity") {
        StackAllocator stack;
        stack.Init(1024);
        REQUIRE(stack.GetCapacity() == 1024);
        REQUIRE(stack.GetUsed() == 0);
        stack.Shutdown();
    }

    SECTION("shutdown zeroes state") {
        StackAllocator stack;
        stack.Init(1024);
        stack.Shutdown();
        REQUIRE(stack.GetCapacity() == 0);
        REQUIRE(stack.GetUsed() == 0);
    }
}

TEST_CASE("StackAllocator: Allocation", "[memory][stack]") {
    StackAllocator stack;
    stack.Init(1024);

    SECTION("returns non-null pointer") {
        void* ptr = stack.Alloc(64);
        REQUIRE(ptr != nullptr);
    }

    SECTION("advances used bytes") {
        stack.Alloc(64);
        REQUIRE(stack.GetUsed() >= 64);
    }

    SECTION("returns nullptr when out of memory") {
        void* ptr = stack.Alloc(2048);
        REQUIRE(ptr == nullptr);
    }

    SECTION("sequential allocations do not overlap") {
        void* a = stack.Alloc(64);
        void* b = stack.Alloc(64);
        uintptr_t addrA = reinterpret_cast<uintptr_t>(a);
        uintptr_t addrB = reinterpret_cast<uintptr_t>(b);
        REQUIRE(addrB >= addrA + 64);
    }

    SECTION("typed alloc returns correct pointer") {
        float* f = stack.Alloc<float>(4);
        REQUIRE(f != nullptr);
        f[0] = 1.0f;
        f[1] = 2.0f;
        f[2] = 3.0f;
        f[3] = 4.0f;
        REQUIRE(f[0] == 1.0f);
        REQUIRE(f[3] == 4.0f);
    }

    stack.Shutdown();
}

TEST_CASE("StackAllocator: Alignment", "[memory][stack]") {
    StackAllocator stack;
    stack.Init(1024);

    SECTION("default alignment of 8") {
        stack.Alloc(1);
        void* ptr = stack.Alloc(8);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 8) == 0);
    }

    SECTION("alignment of 4") {
        stack.Alloc(1);
        void* ptr = stack.Alloc(4, 4);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 4) == 0);
    }

    SECTION("alignment of 16") {
        stack.Alloc(1);
        void* ptr = stack.Alloc(16, 16);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 16) == 0);
    }

    SECTION("alignment of 64") {
        stack.Alloc(1);
        void* ptr = stack.Alloc(64, 64);
        REQUIRE((reinterpret_cast<uintptr_t>(ptr) % 64) == 0);
    }

    stack.Shutdown();
}

TEST_CASE("StackAllocator: Markers", "[memory][stack]") {
    StackAllocator stack;
    stack.Init(1024);

    SECTION("marker at start is zero") {
        size_t marker = stack.GetMarker();
        REQUIRE(marker == 0);
    }

    SECTION("marker advances after allocation") {
        stack.Alloc(64);
        size_t marker = stack.GetMarker();
        REQUIRE(marker >= 64);
    }

    SECTION("FreeToMarker restores used bytes") {
        size_t marker = stack.GetMarker();
        stack.Alloc(256);
        REQUIRE(stack.GetUsed() >= 256);
        stack.FreeToMarker(marker);
        REQUIRE(stack.GetUsed() == marker);
    }

    SECTION("can allocate again after FreeToMarker") {
        size_t marker = stack.GetMarker();
        stack.Alloc(512);
        stack.FreeToMarker(marker);
        void* ptr = stack.Alloc(512);
        REQUIRE(ptr != nullptr);
    }

    SECTION("nested markers work correctly") {
        size_t marker0 = stack.GetMarker();
        stack.Alloc(128);

        size_t marker1 = stack.GetMarker();
        stack.Alloc(128);

        size_t marker2 = stack.GetMarker();
        stack.Alloc(128);

        // pop innermost
        stack.FreeToMarker(marker2);
        REQUIRE(stack.GetUsed() == marker2);

        // pop middle
        stack.FreeToMarker(marker1);
        REQUIRE(stack.GetUsed() == marker1);

        // pop outermost
        stack.FreeToMarker(marker0);
        REQUIRE(stack.GetUsed() == 0);
    }

    SECTION("capacity unchanged after FreeToMarker") {
        size_t marker = stack.GetMarker();
        stack.Alloc(256);
        stack.FreeToMarker(marker);
        REQUIRE(stack.GetCapacity() == 1024);
    }

    stack.Shutdown();
}

TEST_CASE("StackAllocator: Reset", "[memory][stack]") {
    StackAllocator stack;
    stack.Init(1024);

    SECTION("reset brings used back to zero") {
        stack.Alloc(256);
        REQUIRE(stack.GetUsed() > 0);
        stack.Reset();
        REQUIRE(stack.GetUsed() == 0);
    }

    SECTION("can allocate again after reset") {
        stack.Alloc(512);
        stack.Reset();
        void* ptr = stack.Alloc(512);
        REQUIRE(ptr != nullptr);
    }

    SECTION("capacity unchanged after reset") {
        stack.Alloc(256);
        stack.Reset();
        REQUIRE(stack.GetCapacity() == 1024);
    }

    SECTION("marker is zero after reset") {
        stack.Alloc(256);
        stack.Reset();
        REQUIRE(stack.GetMarker() == 0);
    }

    stack.Shutdown();
}

TEST_CASE("StackAllocator: Contains", "[memory][stack]") {
    StackAllocator stack;
    stack.Init(1024);

    SECTION("pointer inside stack returns true") {
        void* ptr = stack.Alloc(64);
        REQUIRE(stack.Contains(ptr));
    }

    SECTION("null pointer returns false") {
        REQUIRE_FALSE(stack.Contains(nullptr));
    }

    SECTION("pointer outside stack returns false") {
        int external = 42;
        REQUIRE_FALSE(stack.Contains(&external));
    }

    SECTION("pointer from different stack returns false") {
        StackAllocator other;
        other.Init(1024);
        void* ptr = other.Alloc(64);
        REQUIRE_FALSE(stack.Contains(ptr));
        other.Shutdown();
    }

    stack.Shutdown();
}

TEST_CASE("StackAllocator: Fill capacity", "[memory][stack]") {
    StackAllocator stack;
    stack.Init(256);

    SECTION("can fill entire stack") {
        void* ptr = stack.Alloc(256);
        REQUIRE(ptr != nullptr);
        REQUIRE(stack.GetUsed() == 256);
    }

    SECTION("cannot exceed capacity") {
        stack.Alloc(256);
        void* overflow = stack.Alloc(1);
        REQUIRE(overflow == nullptr);
    }

    stack.Shutdown();
}