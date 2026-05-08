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

set(GFX_BACKEND "OpenGL" CACHE STRING "Graphics backend")
set_property(CACHE GFX_BACKEND PROPERTY STRINGS "OpenGL" "Vulkan" "Metal" "D3D12")

if(GFX_BACKEND STREQUAL "OpenGL")
    target_compile_definitions(Engine PUBLIC GFX_OPENGL)
elseif(GFX_BACKEND STREQUAL "Vulkan")
    target_compile_definitions(Engine PUBLIC GFX_VULKAN)
elseif(GFX_BACKEND STREQUAL "Metal")
    target_compile_definitions(Engine PUBLIC GFX_METAL)
elseif(GFX_BACKEND STREQUAL "D3D12")
    target_compile_definitions(Engine PUBLIC GFX_D3D12)
endif()




option(ENGINE_EDITOR "Build as editor" OFF)

if(ENGINE_EDITOR)
    target_compile_definitions(Engine PUBLIC ENGINE_EDITOR)
    add_subdirectory(editor)
else()
    add_subdirectory(game)
endif()


if(WIN32)
    target_compile_definitions(Engine PUBLIC PLATFORM_WINDOWS)
elseif(APPLE AND NOT IOS)
    target_compile_definitions(Engine PUBLIC PLATFORM_MAC)
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    target_compile_definitions(Engine PUBLIC PLATFORM_LINUX)
endif()