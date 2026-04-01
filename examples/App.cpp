#include <pch.h>
#include "App.h"

#include <pxr/pxr.h>

#include <imgui.h>
#include <fwd.hpp>

#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include "Example.h"
#include "ExampleLogo.h"
#include "ExampleTextureBurst.h"

int g_Width = 1920;
int g_Height = 1080;
int g_PixelsPerUnit = 16;

std::vector<Example*> g_Examples = std::vector<Example*>();
Example* g_ActiveExample;

static void PushExamples()
{
    // Add any examples here
    g_Examples.emplace_back(new ExampleLogo(g_Width, g_Height, g_PixelsPerUnit));
    g_Examples.emplace_back(new ExampleTextureBurst(g_Width, g_Height, g_PixelsPerUnit));
}

void AppStart()
{
    pxr::Renderer::Init(g_PixelsPerUnit);

    // Create examples
    PushExamples();
    g_ActiveExample = g_Examples[0];
    g_ActiveExample->Enter();
    g_ActiveExample->Resize(g_Width, g_Height);
}

void AppUpdate(float ts)
{
    g_ActiveExample->Update(ts);

    // ImGui Examples Panel
    static bool windowOpen = true;
    if (windowOpen)
    {
        ImGui::Begin("Examples", &windowOpen);

        for (Example* example : g_Examples)
        {
            if (ImGui::Button(example->Name()))
            {
                g_ActiveExample->Exit();
                g_ActiveExample = example;
                g_ActiveExample->Enter();
                g_ActiveExample->Resize(g_Width, g_Height);
            }
        }

        ImGui::End();
    }
}

void AppStop()
{
    g_ActiveExample->Exit();
    pxr::Renderer::Shutdown();

    for (Example* example : g_Examples)
        delete example;
}

void OnWindowResize(int width, int height)
{
    pxr::RenderCommands::ResizeViewport(width, height);

    g_ActiveExample->Resize(width, height);

    g_Width = width;
    g_Height = height;
}