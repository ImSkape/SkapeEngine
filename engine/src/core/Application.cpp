//-----------------------------------
// Application.cpp
// Caleb Davis
// Main game loop for engine
//-------------------------------------
#include "engine/core/Application.h"
#include "engine/platform/glfw/WindowGlfw.h"
#include "engine/core/SEResult.h"
#include "engine/core/ServiceLocator.h"
// TODO: remove — glClear moves to Renderer::BeginFrame(), 
//       glfwGetTime() moves to platform Time abstraction
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "engine/renderer/Renderer.h"
#include <cstdio>
#include <memory>

Application::Application()
{
    m_window = std::make_unique<WindowGlfw>();
}

void Application::Run(const std::string& projectPath)
{
    InitEngine();

    // before anything starts - config, settings overrides
    PreInit();



    // renderer, audio, input etc will init here later
    Init();

    // everything is up, spawn entities, start audio, etc
    PostInit();


    // TIMING SETUP
    float fixedTimestep = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float lastTime = (float)glfwGetTime();

    m_running = true;
    // Loop until the user closes the window
    while (m_running && !m_window->ShouldClose())
    {
        //  DELTA TIME
        float currentTime = (float)glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;


        // clamp so debugger pauses don't explode physics
        dt = std::min(dt, 0.25f);
        accumulator += dt;

        // FIXED UPDATE
        while (accumulator >= fixedTimestep) {
            FixedUpdate(fixedTimestep);
            accumulator -= fixedTimestep;
        }

        // UPDATE
        // input, camera, game logic — variable timestep
        Update(dt);

        //  LATE UPDATE
        // camera follow, animation finalize, read-only
        LateUpdate(dt);

        Render();
        
        #ifdef ENGINE_EDITOR
            ImGuiRender();
        #endif
        // PRESENT
            m_window->PollEvents();
    }

    // save state, flush anything in flight
    PreShutdown();

    // engine systems tear down — logging flush etc
    Shutdown();

    //Anything left hanging
    PostShutdown();



    ShutdownEngine();
}




void Application::InitEngine()
{
    /* Initialize the library */
    SEResult result = m_window->Init(1280, 720, "Skape Engine");
    if (!result) {
        printf("[Fatal] %s\n", result.message.c_str());
        return;
    }


    SEResult rendererResult = m_renderer.Init(static_cast<WindowGlfw*>(m_window.get())->GetGLFWWindow());
    if (!rendererResult) {
        printf("[Fatal] %s\n", rendererResult.message.c_str());
        return;
    }
}

void Application::ShutdownEngine()
{
    m_renderer.Shutdown();
    m_window->Shutdown();
}

void Application::PreInit()
{
}

void Application::Init()
{
    // position (xyz), normal (xyz), uv (xy) per vertex
    static float s_triangleVertices[] = {
        // position          normal            uv
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // bottom right
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f,  // top middle
    };

    auto& ctx = m_renderer.GetContext();
    m_triangleVBO = ctx.CreateBuffer(
        BufferType::Vertex,
        BufferUsage::Static,
        sizeof(s_triangleVertices)
    );

    m_triangleVBO->Upload(s_triangleVertices, sizeof(s_triangleVertices));

    // create shader from source strings
    const char* vertSrc = R"(
        #version 410 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec3 a_Normal;
        layout(location = 2) in vec2 a_UV;
        void main() {
            gl_Position = vec4(a_Position, 1.0);
        }
    )";

    const char* fragSrc = R"(
        #version 410 core
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";

    m_triangleShader = ctx.CreateShader(vertSrc, fragSrc);

    m_cmd = ctx.CreateCommandBuffer();

    // create main render pass — width/height 0 = render to window
    RenderPassSpec passSpec;
    passSpec.width = 0;
    passSpec.height = 0;
    AttachmentSpec colorAttachment;
    colorAttachment.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    colorAttachment.loadOp = LoadOp::Clear;
    passSpec.colorAttachments.push_back(colorAttachment);
    passSpec.depthAttachment.loadOp = LoadOp::Clear;
    m_mainPass = ctx.CreateRenderPass(passSpec);

}

void Application::PostInit()
{
}

void Application::Update(float dt)
{
}

void Application::FixedUpdate(float dt)
{
}

void Application::LateUpdate(float dt)
{
}

void Application::Render()
{
    m_renderer.BeginFrame();
    // scene rendering goes here later


    m_cmd->Begin();
    m_cmd->BeginRenderPass(*m_mainPass);
    m_cmd->SetTopology(PrimitiveTopology::Triangles);
    m_cmd->SetShader(*m_triangleShader);
    m_cmd->BindVertexBuffer(*m_triangleVBO);
    m_cmd->Draw(3, 0);
    m_cmd->EndRenderPass();
    m_cmd->End();

    m_renderer.Submit(*m_cmd);

    m_renderer.EndFrame();
}




#ifdef ENGINE_EDITOR
void Application::ImGuiRender()
{
}
#endif


void Application::PreShutdown()
{
}

void Application::Shutdown()
{
    m_triangleVBO.reset();
    m_triangleShader.reset();
    m_cmd.reset();
    m_mainPass.reset();
    ServiceLocator::Clear();
}

void Application::PostShutdown()
{
}