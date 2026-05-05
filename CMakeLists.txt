cmake_minimum_required(VERSION 3.25)
project(SkapeEngine)

set(CMAKE_CXX_STANDARD 20)




set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# vendors
add_subdirectory(vendors/glfw)
add_subdirectory(vendors/glm)


add_library(EditorConfig INTERFACE)
target_compile_definitions(EditorConfig INTERFACE ENGINE_EDITOR)

# internal
add_subdirectory(engine)

option(ENGINE_EDITOR "Build as editor" OFF)

if(ENGINE_EDITOR)
    target_compile_definitions(Engine PUBLIC ENGINE_EDITOR)
    add_subdirectory(editor)
else()
    add_subdirectory(game)
endif()


if(MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# engine/CMakeLists.txt
if(APPLE)
    target_compile_definitions(Engine PUBLIC GL_SILENCE_DEPRECATION)
endif()
