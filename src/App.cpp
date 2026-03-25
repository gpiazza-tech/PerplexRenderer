#include <pch.h>
#include "App.h"

#include "Util.h"

#include "Renderer/Renderer.h"
#include "Renderer/ParticleSystem.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/TextureAtlas.h"
#include "Renderer/TextureRegistry.h"
#include "Renderer/Postprocessing/BloomRenderer.h"
#include "Renderer/Postprocessing/Tonemapper.h"

#include <glm.hpp>

#include <cstdint>
#include <algorithm>

namespace fs = std::filesystem;

uint32_t g_Width = 1920;
uint32_t g_Height = 1080;
uint32_t g_PixelsPerUnit = 16;

Texture g_Pixel;
Texture g_Bob;
Texture g_Bobby;
Texture g_Blaster;

// TextureAtlas g_ColorAtlas;
// TextureAtlas g_EmmissionAtlas;
// TextureAtlas g_NormalAtlas;

ParticleSystem g_ParticleSystem;

BloomRenderer g_BloomRenderer;
Tonemapper g_Tonemapper;

Framebuffer g_Framebuffer;

glm::mat4 g_Model = glm::mat4(1.0f);
glm::mat4 g_View = glm::mat4(1.0f);
glm::mat4 g_Proj = glm::mat4(1.0f);

float g_CameraZoom = 2.0f;

static void UpdateProjection()
{
    float ratio = std::min(2.0 / (float)g_Width, 1.5 / (float)g_Height);
    float semiWidth = RoundToNearestFraction((float)g_Width * ratio * g_CameraZoom, g_PixelsPerUnit);
    float semiHeight = RoundToNearestFraction((float)g_Height * ratio * g_CameraZoom, g_PixelsPerUnit);

    g_Proj = glm::ortho(-semiWidth, semiWidth, -semiHeight, semiHeight, -1.0f, 1.0f);
}

void AppStart()
{
    // Debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
    glViewport(0, 0, g_Width, g_Height);

    Renderer::Init(g_PixelsPerUnit);

    // Textures
    // g_ColorAtlas.Create(1024, g_PixelsPerUnit);
    g_Pixel =   Renderer::GetTextureRegistry().Add("res\\textures\\White.png");
    g_Bob =     Renderer::GetTextureRegistry().Add("res\\textures\\Bob.png", "res\\textures\\Bob_Emission.png");
    g_Bobby =   Renderer::GetTextureRegistry().Add("res\\textures\\Bobby.png");
    g_Blaster = Renderer::GetTextureRegistry().Add("res\\textures\\Blaster.png");

    // Particle System
    g_ParticleSystem.CreateFromTexture(g_Bobby);
    ParticleSystemSettings defaultSettings = ParticleSystemSettings();
    defaultSettings.VelocityMultiplier = 10.0f;
    defaultSettings.GravityMultiplier = 0.05f;
    defaultSettings.StartVelocity = { 0.0f, 0.2f };
    g_ParticleSystem.SetSettings(defaultSettings);

    // Postprocessing
    g_BloomRenderer.Init(g_Width, g_Height);
    g_Tonemapper.Init(g_Width, g_Height);
    g_Framebuffer = Framebuffer(g_Width, g_Height, true);
    
    Renderer::SetPixelTexture(g_Pixel);
}

void AppUpdate(float ts)
{
    static glm::vec3 cameraPosition = glm::vec3();
    static glm::vec3 bobPosition = glm::vec3(-1.0f, 0.0f, 0.0f);
    static glm::vec3 pixelPosition = glm::vec3();
    static glm::vec3 blasterPosition = glm::vec3();
    g_View = glm::translate(glm::mat4(1.0f), -cameraPosition);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, g_Width, g_Height);
    UpdateProjection();

    // Bind what we need
    g_Framebuffer.Bind();

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Render Scene
    Renderer::BeginBatch(g_Proj);
    
    // Particles
    static glm::vec2 particleSystemPosition = { 0.0f, 0.0f };
    static bool play = false;
    if (play)
        g_ParticleSystem.Update(ts);
    g_ParticleSystem.Render(particleSystemPosition);

    // Quads
    static float bobEmission = 1.0f;
    Renderer::DrawQuad(bobPosition, g_Bob, bobEmission);

    Renderer::EndBatch();
    Renderer::Flush();

    // Postprocessing
    static bool bloom = true;
    static float threshold = 1.0f;
    static float filterRadius = 0.003f;
    if (bloom)
    {
        g_BloomRenderer.RenderBloomTexture(g_Framebuffer.GetTextureID(), threshold, filterRadius);
        g_Framebuffer.DrawTexture(g_BloomRenderer.BloomTexture());
    }

    static bool tonemap = true;
    if (tonemap)
    {
        g_Tonemapper.RenderTonemap(g_Framebuffer.GetTextureID());
        g_Framebuffer.DrawTexture(g_Tonemapper.TonemappedTexture());
    }

    g_Framebuffer.DrawToScreen();

    // ImGui
    static bool showWindow = true;
    if (showWindow)
    {
        ImGui::Begin("Controls", &showWindow);

        // Stats
        std::string timeStr = std::format("{}", ts);
        std::string fpsStr = std::format("{}", 1.0f / ts);
        ImGui::LabelText("Timestep", timeStr.c_str());
        ImGui::LabelText("FPS", fpsStr.c_str());

        // Object GUI
        ImGui::DragFloat3("Camera", &cameraPosition.x, 0.01f);
        ImGui::DragFloat("Zoom", &g_CameraZoom, 0.01f);
        ImGui::DragFloat3("Bob", &bobPosition.x, 0.01f);
        ImGui::DragFloat("Bob Emission", &bobEmission, 0.01f);
        ImGui::DragFloat3("Particle System", &particleSystemPosition.x, 0.01f);

        // Particle GUI
        ParticleSystemSettings settings = g_ParticleSystem.GetSettings();

        ImGui::DragFloat("Speed", &settings.Speed, 0.01f);
        ImGui::DragFloat("Gravity Multiplier", &settings.GravityMultiplier, 0.01f);
        ImGui::DragFloat("Air Friction", &settings.AirFriction, 0.01f);
        ImGui::SliderFloat("Velocity Randomness", &settings.VelocityRandomness, 0.0f, 1.0f);
        ImGui::DragFloat2("Start Velocity", &settings.StartVelocity.x, 0.1f);
        ImGui::DragFloat("Velocity Multiplier", &settings.VelocityMultiplier, 0.1f);
        ImGui::Checkbox("Pixel Perfect", &settings.PixelPerfect);

        g_ParticleSystem.SetSettings(settings);
        if (ImGui::Button("Reset"))
        {
            g_ParticleSystem.Reset();
            play = false;
        }
        if (ImGui::Button("Start"))
            play = true;
        if (ImGui::Button("Stop"))
            play = false;

        // Postprocessing GUI
        ImGui::Checkbox("Bloom", &bloom);
        ImGui::DragFloat("Threshold", &threshold, 0.1f);
        ImGui::SliderFloat("Filter radius", &filterRadius, 0.0f, 0.1f);
        ImGui::Checkbox("Tonemap", &tonemap);

        float bloomImageSize = 0.3f;
        ImGui::Image(g_BloomRenderer.BloomTexture(), ImVec2(g_Width * bloomImageSize, g_Height * bloomImageSize));

        float tonemapImageSize = 0.3f;
        ImGui::Image(g_Tonemapper.TonemappedTexture(), ImVec2(g_Width * bloomImageSize, g_Height * bloomImageSize));

        // float textureAtlasImageSize = 10.0f;
        // ImGui::Image(g_ColorAtlas.GetAtlasTexture(), ImVec2(g_Width * textureAtlasImageSize, g_Height * textureAtlasImageSize));

        ImGui::End();
    }
}

void AppStop()
{
    Renderer::Shutdown();
}

void OnWindowResize(int width, int height)
{
    g_Width = width;
    g_Height = height;

    // Resize buffers
    g_BloomRenderer.Resize(width, height);
    g_Framebuffer.Resize(width, height);

    glViewport(0, 0, g_Width, g_Height);

    // Projection
    UpdateProjection();
}