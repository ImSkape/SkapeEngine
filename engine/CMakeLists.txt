# engine/CMakeLists.txt


set(ENGINE_HEADERS 
include/engine/core/SECore.h
include/engine/core/Application.h
include/engine/core/ServiceLocator.h
include/engine/core/SEResult.h
include/engine/core/System.h
include/engine/core/WindowSystem.h
include/engine/core/JobSystem.h
include/engine/core/TimeSystem.h
include/engine/core/memory/MemorySystem.h
include/engine/core/memory/PoolAllocator.h
include/engine/core/memory/StackAllocator.h
include/engine/core/memory/ArenaAllocator.h
include/engine/platform/Window.h
include/engine/platform/glfw/WindowGlfw.h
include/engine/render/CommandBuffer.h
include/engine/render/GFXContext.h
include/engine/render/GPUBuffer.h
include/engine/render/GPUShader.h
include/engine/render/GPUTexture.h
include/engine/render/Pipeline.h
include/engine/render/RenderSystem.h
include/engine/render/RenderPass.h
include/engine/render/opengl/GLContext.h
include/engine/render/opengl/GLCommandBuffer.h
include/engine/render/opengl/GLRenderPass.h
include/engine/render/opengl/GLBuffer.h
include/engine/render/opengl/GLShader.h
include/engine/render/opengl/GLTexture.h
)


set(ENGINE_SOURCES
src/core/Application.cpp
src/core/ServiceLocator.cpp
src/core/WindowSystem.cpp
src/core/JobSystem.cpp
src/core/TimeSystem.cpp
src/core/memory/MemorySystem.cpp
src/core/memory/StackAllocator.cpp
src/core/memory/ArenaAllocator.cpp
src/platform/glfw/WindowGlfw.cpp
src/render/GFXContext.cpp
src/render/RenderSystem.cpp
src/render/opengl/GLBuffer.cpp
src/render/opengl/GLCommandBuffer.cpp
src/render/opengl/GLContext.cpp
src/render/opengl/GLPipeline.cpp
src/render/opengl/GLRenderPass.cpp
src/render/opengl/GLShader.cpp
src/render/opengl/GLTexture.cpp
)

set(ENGINE_VENDOR
    ${CMAKE_SOURCE_DIR}/vendors/glad/src/glad.c
)

# Tell cmake all the files to include
add_library(Engine STATIC 
    ${ENGINE_HEADERS} 
    ${ENGINE_SOURCES} 
    ${ENGINE_VENDOR}
)
set_source_files_properties(${ENGINE_HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

# suppress warnings on all vendor files
if(MSVC)
    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/vendors/glad/src/glad.c
        PROPERTIES COMPILE_OPTIONS "/w"
    )
else()
    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/vendors/glad/src/glad.c
        PROPERTIES COMPILE_OPTIONS "-Wno-pedantic;-Wno-unused-parameter"
    )
endif()

if(MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# generate VS filters matching folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${ENGINE_HEADERS})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "Source Files"
    FILES ${ENGINE_SOURCES})

source_group("Vendor" FILES ${ENGINE_VENDOR})

# Debug vs. Release defines that can be used in code
target_compile_definitions(Engine PUBLIC
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
)
target_precompile_headers(Engine PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/engine/core/SECore.h>"
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

