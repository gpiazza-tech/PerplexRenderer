#include <pch.h>
#include "App.h"

#include <pxr/pxr.h>

#include <fwd.hpp>
#include <glm.hpp>
#include <GL/glew.h>

#include <cstdint>
#include <algorithm>

#include "Example.h"
#include "ExampleLogo.h"

namespace fs = std::filesystem;

uint32_t g_Width = 1920;
uint32_t g_Height = 1080;
uint32_t g_PixelsPerUnit = 16;

pxr::BloomRenderer g_BloomRenderer;
pxr::Tonemapper g_Tonemapper;

pxr::Framebuffer g_Framebuffer;

glm::mat4 g_Model = glm::mat4(1.0f);
glm::mat4 g_View = glm::mat4(1.0f);
glm::mat4 g_Proj = glm::mat4(1.0f);

float g_CameraZoom = 2.0f;

std::vector<Example*> g_Examples = std::vector<Example*>();
Example* g_ActiveExample;

static void PushExamples()
{
    // Add any examples here
    g_Examples.emplace_back(new ExampleLogo());
}

void AppStart()
{
    // Initialize renderer
    glViewport(0, 0, g_Width, g_Height);
    pxr::Renderer::Init(g_PixelsPerUnit);

    // Create examples
    PushExamples();
    g_ActiveExample = g_Examples[0];
    g_ActiveExample->Enter();

    // Postprocessing
    g_BloomRenderer.Init(g_Width, g_Height);
    g_Tonemapper.Init(g_Width, g_Height);
    g_Framebuffer.Create(g_Width, g_Height, true);
}

void AppUpdate(float ts)
{
    // Bind what we need
    g_Framebuffer.Bind();

    g_ActiveExample->Update(ts);

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
        ImGui::Begin("Global Settings", &showWindow);

        ImGui::Text("Stats");
        std::string timeStr = std::format("{}", ts);
        std::string fpsStr = std::format("{}", 1.0f / ts);
        ImGui::LabelText("Timestep", timeStr.c_str());
        ImGui::LabelText("FPS", fpsStr.c_str());

        ImGui::Text("Post Processing");
        ImGui::Checkbox("Bloom", &bloom);
        ImGui::DragFloat("Threshold", &threshold, 0.01f);
        ImGui::SliderFloat("Filter radius", &filterRadius, 0.0f, 0.01f);
        ImGui::Checkbox("Tonemap", &tonemap);

        ImGui::Text("Examples");
        for (auto& example : g_Examples)
        {
            if (ImGui::Button(example->Name()))
            {
                g_ActiveExample->Exit();
                g_ActiveExample = example;
                g_ActiveExample->Enter();
            }
        }

        ImGui::End();
    }
}

void AppStop()
{
    g_ActiveExample->Exit();

    pxr::Renderer::Shutdown();
}

void OnWindowResize(int width, int height)
{
    g_Width = width;
    g_Height = height;

    // resizing bloom renderer breaks everything for some reason
    // g_BloomRenderer.Resize(width, height);
    g_Tonemapper.Resize(width, height);
    g_Framebuffer.Resize(width, height);

    glViewport(0, 0, g_Width, g_Height);

    g_ActiveExample->Resize(width, height);
}