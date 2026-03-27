#include <pch.h>
#include "App.h"

#include <pxr/pxr.h>

#include <fwd.hpp>
#include <glm.hpp>
#include <GL/glew.h>

#include <cstdint>
#include <algorithm>

namespace fs = std::filesystem;

uint32_t g_Width = 1920;
uint32_t g_Height = 1080;
uint32_t g_PixelsPerUnit = 16;

pxr::Texture g_Bob;
pxr::Texture g_Bobby;
pxr::Texture g_Blaster;
pxr::Texture g_PerplexLogo;

pxr::ParticleSystem g_ParticleSystem;

pxr::BloomRenderer g_BloomRenderer;
pxr::Tonemapper g_Tonemapper;

pxr::Framebuffer g_Framebuffer;

glm::mat4 g_Model = glm::mat4(1.0f);
glm::mat4 g_View = glm::mat4(1.0f);
glm::mat4 g_Proj = glm::mat4(1.0f);

float g_CameraZoom = 2.0f;

static void UpdateProjection()
{
    float ratio = std::min(2.0 / (float)g_Width, 1.5 / (float)g_Height);
    float semiWidth = pxr::RoundToNearestFraction((float)g_Width * ratio * g_CameraZoom, g_PixelsPerUnit);
    float semiHeight = pxr::RoundToNearestFraction((float)g_Height * ratio * g_CameraZoom, g_PixelsPerUnit);

    g_Proj = glm::ortho(-semiWidth, semiWidth, -semiHeight, semiHeight, -1.0f, 1.0f);
}

void AppStart()
{
    glViewport(0, 0, g_Width, g_Height);

    pxr::Renderer::Init(g_PixelsPerUnit);

    // Textures
    // g_ColorAtlas.Create(1024, g_PixelsPerUnit);
    g_Bob =         pxr::Renderer::GetTextureRegistry().Add("res\\textures\\Bob.png", "res\\textures\\Bob_Emission.png");
    g_Bobby =       pxr::Renderer::GetTextureRegistry().Add("res\\textures\\Bobby.png");
    g_Blaster =     pxr::Renderer::GetTextureRegistry().Add("res\\textures\\Blaster.png");
    g_PerplexLogo = pxr::Renderer::GetTextureRegistry().Add("res\\textures\\Perplex.png", "res\\textures\\Perplex_Emission.png");

    // Particle System
    g_ParticleSystem.CreateEmissive();
    pxr::ParticleSystemSettings defaultSettings = pxr::ParticleSystemSettings();
    defaultSettings.ParticlesPerSecond = 40;
    defaultSettings.SpawnBounds = { 10.0f, 1.0f };
    defaultSettings.VelocityMultiplier = 10.0f;
    defaultSettings.GravityMultiplier = 0.05f;
    defaultSettings.StartVelocity = { -0.2f, -0.5f };
    defaultSettings.StartColor = { 0.5f, 0.55f, 1.0f, 1.0f };
    defaultSettings.StartEmission = 0.5f;
    defaultSettings.PixelPerfect = false;
    g_ParticleSystem.SetSettings(defaultSettings);

    // Postprocessing
    g_BloomRenderer.Init(g_Width, g_Height);
    g_Tonemapper.Init(g_Width, g_Height);
    g_Framebuffer.Create(g_Width, g_Height, true);
}

void AppUpdate(float ts)
{
    static glm::vec3 cameraPosition = glm::vec3();
    static glm::vec3 perplexPosition = glm::vec3(-1.0f, 0.0f, 0.0f);
    static glm::vec3 pixelPosition = glm::vec3();
    static glm::vec3 blasterPosition = glm::vec3();
    g_View = glm::translate(glm::mat4(1.0f), -cameraPosition);

    UpdateProjection();

    // Bind what we need
    g_Framebuffer.Bind();

    // Render Scene
    pxr::Renderer::BeginBatch(g_Proj);
    
    // Particles
    static glm::vec2 particleSystemPosition = { 0.0f, 4.0f };
    static bool play = false;
    if (play)
        g_ParticleSystem.Update(ts);
    g_ParticleSystem.Render(particleSystemPosition);

    // Quads

    static float perplexEmission = 1.0f;
    pxr::Renderer::DrawQuad(perplexPosition, g_PerplexLogo, perplexEmission);

    pxr::Renderer::EndBatch();
    pxr::Renderer::Flush();

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
        ImGui::DragFloat3("Bob", &perplexPosition.x, 0.01f);
        ImGui::DragFloat("Perplex Emission", &perplexEmission, 0.01f);

        // Postprocessing GUI
        ImGui::Checkbox("Bloom", &bloom);
        ImGui::DragFloat("Threshold", &threshold, 0.01f);
        ImGui::SliderFloat("Filter radius", &filterRadius, 0.0f, 0.01f);
        ImGui::Checkbox("Tonemap", &tonemap);

        float bloomImageSize = 0.3f;
        ImGui::Image(g_BloomRenderer.BloomTexture(), ImVec2(g_Width * bloomImageSize, g_Height * bloomImageSize));

        float tonemapImageSize = 0.3f;
        ImGui::Image(g_Tonemapper.TonemappedTexture(), ImVec2(g_Width * bloomImageSize, g_Height * bloomImageSize));

        ImGui::End();
    }

    static bool showParticleSystemSettings = true;
    if (showParticleSystemSettings)
    {
        ImGui::Begin("Particle System Settings", &showParticleSystemSettings);

        pxr::ParticleSystemSettings settings = g_ParticleSystem.GetSettings();

        ImGui::DragFloat3("Position", &particleSystemPosition.x, 0.01f);
        ImGui::DragInt("Rate", &settings.ParticlesPerSecond);
        ImGui::DragFloat("Start Lifetime", &settings.StartLifetime, 0.01f);
        ImGui::DragFloat2("Bounds", &settings.SpawnBounds.x);
        ImGui::DragFloat("Speed", &settings.Speed, 0.01f);

        ImGui::DragFloat("Gravity Multiplier", &settings.GravityMultiplier, 0.01f);
        ImGui::DragFloat("Air Friction", &settings.AirFriction, 0.01f);
        ImGui::SliderFloat("Velocity Randomness", &settings.VelocityRandomness, 0.0f, 1.0f);
        ImGui::DragFloat2("Start Velocity", &settings.StartVelocity.x, 0.1f);
        ImGui::DragFloat("Velocity Multiplier", &settings.VelocityMultiplier, 0.01f);
        
        ImGui::DragFloat4("Color", &settings.StartColor.x, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Emission", &settings.StartEmission, 0.01f, 0.0f);
        ImGui::DragFloat("Emission Multiplier", &settings.EmissionMultiplier, 0.01f);
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

        ImGui::End();
    }
}

void AppStop()
{
    pxr::Renderer::Shutdown();
}

void OnWindowResize(int width, int height)
{
    g_Width = width;
    g_Height = height;

    // Resize buffers
    g_BloomRenderer.Resize(width, height);
    g_Tonemapper.Resize(width, height);
    g_Framebuffer.Resize(width, height);

    glViewport(0, 0, g_Width, g_Height);

    // Projection
    UpdateProjection();
}