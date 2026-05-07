# engine/CMakeLists.txt


# Tell cmake all the files to include
add_library(Engine STATIC
include/engine/core/Application.h
include/engine/core/ServiceLocator.h
include/engine/core/SEResult.h
include/engine/platform/Window.h
include/engine/render_system/CommandBuffer.h
include/engine/render_system/GFXContext.h
include/engine/render_system/GPUBuffer.h
include/engine/render_system/GPUShader.h
include/engine/render_system/GPUTexture.h
include/engine/render_system/Pipeline.h
include/engine/render_system/RenderSystem.h
include/engine/render_system/RenderPass.h
include/engine/render_system/opengl/GLContext.h
include/engine/render_system/opengl/GLCommandBuffer.h
include/engine/render_system/opengl/GLRenderPass.h
include/engine/render_system/opengl/GLBuffer.h
include/engine/render_system/opengl/GLShader.h
include/engine/render_system/opengl/GLTexture.h
src/core/Application.cpp
src/core/ServiceLocator.cpp
src/platform/glfw/WindowGlfw.cpp
src/platform/glfw/WindowGlfw.cpp
src/render_system/GFXContext.cpp
src/render_system/RenderSystem.cpp
src/render_system/opengl/GLBuffer.cpp
src/render_system/opengl/GLCommandBuffer.cpp
src/render_system/opengl/GLContext.cpp
src/render_system/opengl/GLPipeline.cpp
src/render_system/opengl/GLRenderPass.cpp
src/render_system/opengl/GLShader.cpp
src/render_system/opengl/GLTexture.cpp

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

