cmake_minimum_required(VERSION 3.25)


include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)


set(TEST_SOURCES
    TestMain.cpp
    core/memory/ArenaAllocatorTests.cpp
    core/memory/StackAllocatorTests.cpp
    core/memory/PoolAllocatorTests.cpp
    core/memory/MemorySystemTests.cpp
    core/ServiceLocatorTests.cpp
)

add_executable(EngineTests ${TEST_SOURCES})


target_link_libraries(EngineTests PRIVATE
    Engine
    Catch2::Catch2WithMain
)


target_compile_definitions(EngineTests PRIVATE ENGINE_DEBUG)


# register with CTest so cmake --build then ctest runs them
include(CTest)
include(Catch)
catch_discover_tests(EngineTests)