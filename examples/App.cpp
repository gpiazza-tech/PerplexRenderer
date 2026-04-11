#include <pch.h>
#include "App.h"

#include <pxr/pxr.h>

#include <imgui.h>

#include <string>
#include <vector>

#include "Example.h"
#include "ExampleLogo.h"
#include "ExampleBurst.h"
#include "ExampleSpriteBurst.h"
#include "ExampleScale.h"
#include "ExampleSmile.h"

int g_Width = 1920;
int g_Height = 1080;

std::vector<Example*> g_Examples = std::vector<Example*>();
Example* g_ActiveExample;

pxr::Framebuffer* g_Framebuffer;

pxr::BloomRenderer g_BloomRenderer;
pxr::Tonemapper g_Tonemapper;
pxr::Pixelator g_Pixelator;

pxr::Camera g_Camera = pxr::Camera(glm::vec2(g_Width, g_Height), 16, 6.0f, pxr::ScalingMode::SmallerSide);

static void PushExamples()
{
    // Add any examples here
    g_Examples.emplace_back(new ExampleLogo(&g_Camera));
    g_Examples.emplace_back(new ExampleBurst(&g_Camera));
	g_Examples.emplace_back(new ExampleSpriteBurst(&g_Camera));
	g_Examples.emplace_back(new ExampleScale(&g_Camera));
	g_Examples.emplace_back(new ExampleSmile(&g_Camera));
}

void AppStart()
{
    pxr::Renderer::Init(16);
    g_Framebuffer = new pxr::Framebuffer(g_Width, g_Height, true);

    g_BloomRenderer.Init(g_Width, g_Height);
    g_Tonemapper.Init(g_Width, g_Height);
    g_Pixelator.Init(g_Width, g_Height);

    // Create examples
    PushExamples();
    g_ActiveExample = g_Examples[0];
    g_ActiveExample->Enter();
}

void AppUpdate(float ts)
{
    g_Framebuffer->Bind();

    // Update Example
    g_ActiveExample->Update(ts);

	// Post processing
	static bool bloom = true;
	static float threshold = 1.0f;
	static float filterRadius = 0.003f;
	if (bloom)
	{
		g_BloomRenderer.RenderBloomTexture(g_Framebuffer->GetTextureID(), threshold, filterRadius);
		g_Framebuffer->DrawTexture(g_BloomRenderer.BloomTexture());
	}

	static bool tonemap = true;
	if (tonemap)
	{
		g_Tonemapper.RenderTonemap(g_Framebuffer->GetTextureID());
        g_Framebuffer->DrawTexture(g_Tonemapper.TonemappedTexture());
	}

	static bool pixelate = true;
	if (pixelate)
	{
		g_Pixelator.RenderPixelator(g_Framebuffer->GetTextureID(), g_Camera.GetPixelResolution());
		g_Framebuffer->DrawTexture(g_Pixelator.PixelatedTexture());
	}
	g_Framebuffer->DrawToScreen();

	// ImGui Stats and Post Processing
	static bool showWindow = true;
	if (showWindow)
	{
		ImGui::Begin("Global Settings", &showWindow);

		ImGui::Text("Examples");
		for (Example* example : g_Examples)
		{
			if (ImGui::Button(example->Name()))
			{
				g_ActiveExample->Exit();
				g_ActiveExample = example;
				g_ActiveExample->Enter();
			}
		}
		ImGui::Dummy({ 0.0f, 20.0f });

		ImGui::Text("Stats");
		std::string timeStr = std::format("{}", ts);
		std::string fpsStr = std::format("{}", 1.0f / ts);
		ImGui::LabelText("Timestep", timeStr.c_str());
		ImGui::LabelText("FPS", fpsStr.c_str());
		const pxr::RenderStats& stats = pxr::Renderer::GetStats();
		std::string quadsStr = std::format("{}", stats.Quads);
		std::string drawCallsStr = std::format("{}", stats.DrawCalls);
		ImGui::LabelText("Quads", quadsStr.c_str());
		ImGui::LabelText("Draw Calls", drawCallsStr.c_str());
		ImGui::Dummy({ 0.0f, 20.0f });

		ImGui::Text("Camera");
		static float camZoom = g_Camera.GetZoom();
		ImGui::DragFloat("Zoom", &camZoom, 0.01f);
		g_Camera.SetZoom(camZoom);
		static const char* scalingModeNames[]{ "Width", "Height", "Larger Side", "Smaller Side" };
		static int scalingMode = 3;
		if (ImGui::Combo("Scaling Mode", &scalingMode, scalingModeNames, IM_ARRAYSIZE(scalingModeNames)))
			g_Camera.SetScalingMode((pxr::ScalingMode)(scalingMode + 1));


		ImGui::Text("Post Processing");
		ImGui::Checkbox("Bloom", &bloom);
		ImGui::DragFloat("Threshold", &threshold, 0.01f);
		ImGui::SliderFloat("Filter radius", &filterRadius, 0.0f, 0.01f);
		ImGui::Checkbox("Tonemap", &tonemap);
		ImGui::Checkbox("Pixelate", &pixelate);

		ImGui::End();
	}
}

void AppStop()
{
    g_ActiveExample->Exit();

    delete g_Framebuffer;
    g_BloomRenderer.Destroy();
    g_Tonemapper.Destroy();
    g_Pixelator.Destroy();
    pxr::Renderer::Shutdown();

    for (Example* example : g_Examples)
        delete example;
}

void OnWindowResize(int width, int height)
{
    pxr::RenderCommands::ResizeViewport(width, height);

    g_Camera.Resize({ width, height });

    g_BloomRenderer.Resize(width, height);
    g_Tonemapper.Resize(width, height);
    g_Framebuffer->Resize(width, height);
    g_Pixelator.Resize(width, height);

    g_Width = width;
    g_Height = height;
}