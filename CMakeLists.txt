cmake_minimum_required(VERSION 3.25)
project(SkapeEngine)

set(CMAKE_CXX_STANDARD 20)


# vendors
add_subdirectory(vendors/glfw)


# internal
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(game)