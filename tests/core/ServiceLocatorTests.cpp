//-----------------------------------
// ServiceLocatorTests.cpp
// Caleb Davis
// Unit tests for ServiceLocator
//-------------------------------------
#include <catch2/catch_all.hpp>
#include "engine/core/ServiceLocator.h"

// test services
struct ServiceA {
    int value = 1;
    const std::string& GetName() const {
        static std::string name = "ServiceA";
        return name;
    }
};

struct ServiceB {
    int value = 2;
    const std::string& GetName() const {
        static std::string name = "ServiceB";
        return name;
    }
};

struct ServiceC {
    int value = 3;
    const std::string& GetName() const {
        static std::string name = "ServiceC";
        return name;
    }
};

TEST_CASE("ServiceLocator: Register and Get", "[servicelocator]") {

    SECTION("can register and retrieve a service") {
        ServiceA a;
        ServiceLocator::Register<ServiceA>(&a);
        ServiceA* retrieved = ServiceLocator::Get<ServiceA>();
        REQUIRE(retrieved != nullptr);
        REQUIRE(retrieved == &a);
        ServiceLocator::Clear();
    }

    SECTION("retrieved service has correct data") {
        ServiceA a;
        a.value = 42;
        ServiceLocator::Register<ServiceA>(&a);
        ServiceA* retrieved = ServiceLocator::Get<ServiceA>();
        REQUIRE(retrieved->value == 42);
        ServiceLocator::Clear();
    }

    SECTION("can register multiple services independently") {
        ServiceA a;
        ServiceB b;
        ServiceLocator::Register<ServiceA>(&a);
        ServiceLocator::Register<ServiceB>(&b);

        REQUIRE(ServiceLocator::Get<ServiceA>() == &a);
        REQUIRE(ServiceLocator::Get<ServiceB>() == &b);
        ServiceLocator::Clear();
    }

    SECTION("registering same type twice overwrites previous") {
        ServiceA a1;
        ServiceA a2;
        a1.value = 1;
        a2.value = 2;

        ServiceLocator::Register<ServiceA>(&a1);
        ServiceLocator::Register<ServiceA>(&a2);

        ServiceA* retrieved = ServiceLocator::Get<ServiceA>();
        REQUIRE(retrieved->value == 2);
        ServiceLocator::Clear();
    }
}

TEST_CASE("ServiceLocator: Get unregistered", "[servicelocator]") {

    SECTION("returns nullptr for unregistered service") {
        ServiceLocator::Clear();
        ServiceA* retrieved = ServiceLocator::Get<ServiceA>();
        REQUIRE(retrieved == nullptr);
    }

    SECTION("returns nullptr after clear") {
        ServiceA a;
        ServiceLocator::Register<ServiceA>(&a);
        ServiceLocator::Clear();
        REQUIRE(ServiceLocator::Get<ServiceA>() == nullptr);
    }
}

TEST_CASE("ServiceLocator: Clear", "[servicelocator]") {

    SECTION("clear removes all services") {
        ServiceA a;
        ServiceB b;
        ServiceC c;

        ServiceLocator::Register<ServiceA>(&a);
        ServiceLocator::Register<ServiceB>(&b);
        ServiceLocator::Register<ServiceC>(&c);

        ServiceLocator::Clear();

        REQUIRE(ServiceLocator::Get<ServiceA>() == nullptr);
        REQUIRE(ServiceLocator::Get<ServiceB>() == nullptr);
        REQUIRE(ServiceLocator::Get<ServiceC>() == nullptr);
    }

    SECTION("can register again after clear") {
        ServiceA a;
        ServiceLocator::Register<ServiceA>(&a);
        ServiceLocator::Clear();

        ServiceA a2;
        a2.value = 99;
        ServiceLocator::Register<ServiceA>(&a2);

        ServiceA* retrieved = ServiceLocator::Get<ServiceA>();
        REQUIRE(retrieved != nullptr);
        REQUIRE(retrieved->value == 99);
        ServiceLocator::Clear();
    }

    SECTION("multiple clears are safe") {
        ServiceLocator::Clear();
        ServiceLocator::Clear();
        ServiceLocator::Clear();
        REQUIRE(ServiceLocator::Get<ServiceA>() == nullptr);
    }
}

TEST_CASE("ServiceLocator: Type safety", "[servicelocator]") {

    SECTION("different types do not collide") {
        ServiceA a;
        ServiceB b;
        a.value = 10;
        b.value = 20;

        ServiceLocator::Register<ServiceA>(&a);
        ServiceLocator::Register<ServiceB>(&b);

        REQUIRE(ServiceLocator::Get<ServiceA>()->value == 10);
        REQUIRE(ServiceLocator::Get<ServiceB>()->value == 20);
        ServiceLocator::Clear();
    }

    SECTION("pointer identity is preserved") {
        ServiceA a;
        ServiceLocator::Register<ServiceA>(&a);
        ServiceA* retrieved = ServiceLocator::Get<ServiceA>();

        // same pointer — not a copy
        retrieved->value = 99;
        REQUIRE(a.value == 99);
        ServiceLocator::Clear();
    }
}