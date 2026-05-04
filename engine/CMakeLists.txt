# engine/CMakeLists.txt
add_library(Engine STATIC
    src/Engine.cpp
)

target_link_libraries(Engine PUBLIC glfw)

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
