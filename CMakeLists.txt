cmake_minimum_required(VERSION 3.25)
project(SkapeEngine)

set(CMAKE_CXX_STANDARD 20)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# vendors
add_subdirectory(vendors/glfw)
add_subdirectory(vendors/glm)

# internal
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(game)