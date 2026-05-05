# engine/CMakeLists.txt


# Tell cmake all the files to include
add_library(Engine STATIC
    src/core/Application.cpp
    include/engine/core/Application.h
    include/engine/core/ServiceLocator.h
    include/engine/core/SEResult.h
    include/engine/platform/Window.h
    src/platform/glfw/WindowGlfw.cpp
    include/engine/platform/glfw/WindowGlfw.h
    ${CMAKE_SOURCE_DIR}/vendors/glad/src/glad.c
)

# Debug vs. Release defines that can be used in code
target_compile_definitions(Engine PUBLIC
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
)

# Link vendor libs
target_link_libraries(Engine PUBLIC 
    glfw 
    glm::glm
    )

# Add all include folders
target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendors/glad/include
)

if(APPLE)
    target_compile_definitions(Engine PUBLIC GL_SILENCE_DEPRECATION)
    target_compile_options(Engine PUBLIC -Wno-deprecated-declarations)
endif()

