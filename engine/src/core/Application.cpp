//-----------------------------------
// Application.cpp
// Caleb Davis
// Main game loop for engine
//-------------------------------------
#include "engine/core/SECore.h"
#include "engine/core/Application.h"
#include "engine/core/SEResult.h"
#include <cstdio>
Application::Application()
{
}

void Application::Run(const std::string& projectPath)
{
    UNREFERENCED_PARAMETER(projectPath);
    InitEngine();

    // before anything starts - config, settings overrides
    PreInit();



    // renderer, audio, input etc will init here later
    Init();

    // everything is up, spawn entities, start audio, etc
    PostInit();


    m_running = true;


    // Loop until the user closes the window
    while (m_running && !m_windowSystem.ShouldClose())
    {
        m_memorySystem.ResetFrameArenas();  // first — wipe last frame's temp data

        // tick time - updates dt, accumulator, frame count
        m_timeSystem.Tick();


        // FIXED UPDATE
        while (m_timeSystem.ShouldFixedUpdate()) {
            FixedUpdate(m_timeSystem.GetFixedDeltaTime());
            m_timeSystem.ConsumeFixedUpdate();
        }

        // UPDATE
        // input, camera, game logic: variable timestep
        Update(m_timeSystem.GetDeltaTime());

        //  LATE UPDATE
        // camera follow, animation finalize, read-only
        LateUpdate(m_timeSystem.GetDeltaTime());

        Render();
        
        #ifdef ENGINE_EDITOR
            ImGuiRender();
        #endif
    }

    // save state, flush anything in flight
    PreShutdown();

    // engine systems tear down � logging flush etc
    Shutdown();

    //Anything left hanging
    PostShutdown();



    ShutdownEngine();
}

void Application::RunHeadless(const std::string& projectPath, int frames)
{
    UNREFERENCED_PARAMETER(projectPath);
    m_headless = true;
    InitEngine();
    PreInit();
    Init();
    PostInit();

    for (int i = 0; i < frames; i++) {
        m_memorySystem.ResetFrameArenas();
        m_timeSystem.Tick();

        while (m_timeSystem.ShouldFixedUpdate()) {
            FixedUpdate(m_timeSystem.GetFixedDeltaTime());
            m_timeSystem.ConsumeFixedUpdate();
        }

        Update(m_timeSystem.GetDeltaTime());
        LateUpdate(m_timeSystem.GetDeltaTime());
    }

    PreShutdown();
    Shutdown();
    PostShutdown();
    ShutdownEngine();
}




void Application::InitEngine()
{

    MemoryConfig memConfig;  // uses defaults
    m_memorySystem.Configure(memConfig);
    RegisterSystem(&m_memorySystem);


    // time before window - nothing depends on time but everything uses it
    RegisterSystem(&m_timeSystem);


    // job system early — asset manager and ECS need it
    RegisterSystem(&m_jobSystem);






    WindowConfig config;
    config.width = 1280;
    config.height = 720;
    config.title = "Skape Engine";
    config.vsync = true;
    m_windowSystem.Configure(config);
    if (!m_headless) {
        RegisterSystem(&m_windowSystem);
        RegisterSystem(&m_renderSystem);
    }


    //if (!m_headless) {

    //    /* Initialize the library */
    //    SEResult result = m_window->Init(1280, 720, "Skape Engine");
    //    if (!result) {
    //        printf("[Fatal] %s\n", result.message.c_str());
    //        return;
    //    }


    //    SEResult rendererResult = m_renderer.Init(static_cast<WindowGlfw*>(m_window.get())->GetGLFWWindow());
    //    if (!rendererResult) {
    //        printf("[Fatal] %s\n", rendererResult.message.c_str());
    //        return;
    //    }
    //}
    //ServiceLocator::Register<RenderSystem>(&m_renderer);

}

void Application::ShutdownEngine()
{
    for (auto it = m_systems.rbegin(); it != m_systems.rend(); ++it)
        (*it)->Shutdown();
    m_systems.clear();
    ServiceLocator::Clear();
}

void Application::PreInit()
{
}

void Application::Init()
{
    if (!m_headless) {
        // position (xyz), normal (xyz), uv (xy) per vertex
        static float s_triangleVertices[] = {
            // position          normal            uv
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
             0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // bottom right
             0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f,  // top middle
        };

        auto& ctx = m_renderSystem.GetContext();
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

        // create main render pass � width/height 0 = render to window
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

}

void Application::PostInit()
{
}

void Application::Update(float dt)
{
    for (auto* system : m_systems)
        system->Update(dt);
}

void Application::FixedUpdate(float dt)
{
    UNREFERENCED_PARAMETER(dt);
}

void Application::LateUpdate(float dt)
{
    UNREFERENCED_PARAMETER(dt);
}

void Application::Render()
{
    if (m_headless) return;
    m_renderSystem.BeginFrame();
    // scene rendering goes here later


    m_cmd->Begin();
    m_cmd->BeginRenderPass(*m_mainPass);
    m_cmd->SetTopology(PrimitiveTopology::Triangles);
    m_cmd->SetShader(*m_triangleShader);
    m_cmd->BindVertexBuffer(*m_triangleVBO);
    m_cmd->Draw(3, 0);
    m_cmd->EndRenderPass();
    m_cmd->End();

    m_renderSystem.Submit(*m_cmd);

    m_renderSystem.EndFrame();
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
    if (!m_headless) {
        m_triangleVBO.reset();
        m_triangleShader.reset();
        m_cmd.reset();
        m_mainPass.reset();
    }
    ServiceLocator::Clear();
}

void Application::PostShutdown()
{
}
