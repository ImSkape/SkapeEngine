#include <catch2/catch_all.hpp>
#include "engine/core/memory/PoolAllocator.h"

// simple test struct
struct TestObject {
    int   id    = 0;
    float value = 0.0f;
    bool  active = false;
};


TEST_CASE("PoolAllocator: Init and Shutdown", "[memory][pool]") {

    SECTION("initializes with correct capacity") {
        PoolAllocator<TestObject> pool;
        pool.Init(16);
        REQUIRE(pool.GetCapacity() == 16);
        REQUIRE(pool.GetUsed() == 0);
        pool.Shutdown();
    }

    SECTION("shutdown zeroes state") {
        PoolAllocator<TestObject> pool;
        pool.Init(16);
        pool.Shutdown();
        REQUIRE(pool.GetCapacity() == 0);
        REQUIRE(pool.GetUsed() == 0);
    }
}

TEST_CASE("PoolAllocator: Allocation", "[memory][pool]") {
    PoolAllocator<TestObject> pool;
    pool.Init(16);

    SECTION("returns non-null pointer") {
        TestObject* obj = pool.Alloc();
        REQUIRE(obj != nullptr);
    }

    SECTION("advances used count") {
        pool.Alloc();
        REQUIRE(pool.GetUsed() == 1);
    }

    SECTION("returns nullptr when pool exhausted") {
        for (size_t i = 0; i < 16; i++)
            pool.Alloc();
        TestObject* overflow = pool.Alloc();
        REQUIRE(overflow == nullptr);
        REQUIRE(pool.GetUsed() == 16);
    }

    SECTION("multiple allocations return distinct pointers") {
        TestObject* a = pool.Alloc();
        TestObject* b = pool.Alloc();
        TestObject* c = pool.Alloc();
        REQUIRE(a != b);
        REQUIRE(b != c);
        REQUIRE(a != c);
    }

    SECTION("allocated memory is writable") {
        TestObject* obj = pool.Alloc();
        obj->id     = 42;
        obj->value  = 3.14f;
        obj->active = true;
        REQUIRE(obj->id     == 42);
        REQUIRE(obj->value  == Catch::Approx(3.14f));
        REQUIRE(obj->active == true);
    }

    pool.Shutdown();
}

TEST_CASE("PoolAllocator: Free", "[memory][pool]") {
    PoolAllocator<TestObject> pool;
    pool.Init(16);

    SECTION("free decrements used count") {
        TestObject* obj = pool.Alloc();
        REQUIRE(pool.GetUsed() == 1);
        pool.Free(obj);
        REQUIRE(pool.GetUsed() == 0);
    }

    SECTION("freed slot can be reallocated") {
        TestObject* obj = pool.Alloc();
        pool.Free(obj);
        TestObject* reused = pool.Alloc();
        REQUIRE(reused != nullptr);
        REQUIRE(pool.GetUsed() == 1);
    }

    SECTION("freeing null is safe") {
        REQUIRE_NOTHROW(pool.Free(nullptr));
        REQUIRE(pool.GetUsed() == 0);
    }

    SECTION("free and reallocate many times") {
        for (int i = 0; i < 100; i++) {
            TestObject* obj = pool.Alloc();
            REQUIRE(obj != nullptr);
            obj->id = i;
            pool.Free(obj);
        }
        REQUIRE(pool.GetUsed() == 0);
    }

    pool.Shutdown();
}

TEST_CASE("PoolAllocator: Free list integrity", "[memory][pool]") {
    PoolAllocator<TestObject> pool;
    pool.Init(4);

    SECTION("allocate all then free all then allocate all again") {
        TestObject* a = pool.Alloc();
        TestObject* b = pool.Alloc();
        TestObject* c = pool.Alloc();
        TestObject* d = pool.Alloc();

        REQUIRE(pool.GetUsed() == 4);
        REQUIRE(pool.Alloc() == nullptr);

        pool.Free(a);
        pool.Free(b);
        pool.Free(c);
        pool.Free(d);

        REQUIRE(pool.GetUsed() == 0);

        // all four slots should be available again
        TestObject* e = pool.Alloc();
        TestObject* f = pool.Alloc();
        TestObject* g = pool.Alloc();
        TestObject* h = pool.Alloc();

        REQUIRE(e != nullptr);
        REQUIRE(f != nullptr);
        REQUIRE(g != nullptr);
        REQUIRE(h != nullptr);
        REQUIRE(pool.GetUsed() == 4);

        pool.Free(e);
        pool.Free(f);
        pool.Free(g);
        pool.Free(h);
    }

    SECTION("interleaved alloc and free") {
        TestObject* a = pool.Alloc();
        TestObject* b = pool.Alloc();
        pool.Free(a);
        TestObject* c = pool.Alloc();  // should reuse a's slot
        pool.Free(b);
        TestObject* d = pool.Alloc();  // should reuse b's slot

        REQUIRE(c != nullptr);
        REQUIRE(d != nullptr);
        REQUIRE(pool.GetUsed() == 2);

        pool.Free(c);
        pool.Free(d);
    }

    pool.Shutdown();
}

TEST_CASE("PoolAllocator: Capacity limits", "[memory][pool]") {

    SECTION("pool of 1 works correctly") {
        PoolAllocator<TestObject> pool;
        pool.Init(1);
        REQUIRE(pool.GetCapacity() == 1);

        TestObject* obj = pool.Alloc();
        REQUIRE(obj != nullptr);
        REQUIRE(pool.Alloc() == nullptr);

        pool.Free(obj);
        REQUIRE(pool.GetUsed() == 0);

        TestObject* reused = pool.Alloc();
        REQUIRE(reused != nullptr);
        pool.Shutdown();
    }

    SECTION("large pool allocates all slots") {
        PoolAllocator<TestObject> pool;
        pool.Init(256);

        for (size_t i = 0; i < 256; i++) {
            TestObject* obj = pool.Alloc();
            REQUIRE(obj != nullptr);
        }

        REQUIRE(pool.GetUsed() == 256);
        REQUIRE(pool.Alloc() == nullptr);
        pool.Shutdown();
    }
}

TEST_CASE("PoolAllocator: Different types", "[memory][pool]") {

    SECTION("works with int") {
        PoolAllocator<int> pool;
        pool.Init(8);
        int* a = pool.Alloc();
        REQUIRE(a != nullptr);
        *a = 42;
        REQUIRE(*a == 42);
        pool.Free(a);
        pool.Shutdown();
    }

    SECTION("works with large struct") {
        struct BigObject {
            float data[64];
            int   id       = 0;
        };

        PoolAllocator<BigObject> pool;
        pool.Init(4);
        BigObject* obj = pool.Alloc();
        REQUIRE(obj != nullptr);
        obj->id      = 99;
        obj->data[0] = 1.0f;
        obj->data[63] = 2.0f;
        REQUIRE(obj->id       == 99);
        REQUIRE(obj->data[0]  == Catch::Approx(1.0f));
        REQUIRE(obj->data[63] == Catch::Approx(2.0f));
        pool.Free(obj);
        pool.Shutdown();
    }
}