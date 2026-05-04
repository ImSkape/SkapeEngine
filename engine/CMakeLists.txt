# engine/CMakeLists.txt



add_library(Engine STATIC
    src/core/Engine.cpp
    include/engine/core/Engine.h
    include/engine/core/SEResult.h
    include/engine/platform/Window.h
    src/platform/glfw/WindowGlfw.cpp
    include/engine/platform/glfw/WindowGlfw.h
    ${CMAKE_SOURCE_DIR}/vendors/glad/src/glad.c
)

target_compile_definitions(Engine PUBLIC
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
)

target_link_libraries(Engine PUBLIC 
    glfw 
    glm::glm
    )

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendors/glad/include
)
