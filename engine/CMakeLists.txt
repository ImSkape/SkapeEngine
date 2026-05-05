# engine/CMakeLists.txt


# Tell cmake all the files to include
add_library(Engine STATIC
include/engine/core/Application.h
include/engine/core/ServiceLocator.h
include/engine/core/SEResult.h
include/engine/platform/Window.h
include/engine/renderer/CommandBuffer.h
include/engine/renderer/GFXContext.h
include/engine/renderer/GPUBuffer.h
include/engine/renderer/GPUShader.h
include/engine/renderer/GPUTexture.h
include/engine/renderer/Pipeline.h
include/engine/renderer/Renderer.h
include/engine/renderer/RenderPass.h
include/engine/renderer/opengl/GLContext.h
src/core/Application.cpp
src/core/ServiceLocator.cpp
src/platform/glfw/WindowGlfw.cpp
src/platform/glfw/WindowGlfw.cpp
src/renderer/GFXContext.cpp
src/renderer/Renderer.cpp
src/renderer/opengl/GLBuffer.cpp
src/renderer/opengl/GLCommandBuffer.cpp
src/renderer/opengl/GLContext.cpp
src/renderer/opengl/GLPipeline.cpp
src/renderer/opengl/GLRenderPass.cpp
src/renderer/opengl/GLShader.cpp
src/renderer/opengl/GLTexture.cpp

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

