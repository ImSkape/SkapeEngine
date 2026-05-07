//-----------------------------------
// GFXContext.cpp
// Caleb Davis
// Sets and handles the GFX context, backend vs. frontend
//-------------------------------------

#include "engine/render_system/GFXContext.h"

#ifdef GFX_OPENGL
#include "engine/render_system/opengl/GLContext.h"
#endif
#ifdef GFX_VULKAN
#include "engine/render_system/vulkan/VKContext.h"
#endif

std::unique_ptr<GFXContext> GFXContext::Create() {
#ifdef GFX_OPENGL
    return std::make_unique<GLContext>();
#elif defined(GFX_VULKAN)
    return std::make_unique<VKContext>();
#else
    static_assert(false, "No GFX backend defined � set GFX_OPENGL or GFX_VULKAN in CMake");
    return nullptr;
#endif
}