#pragma once

#include <pxr/pxr.h>
#include "Example.h"

// #define BLOOM

class ExampleLogo : public Example
{
public:
	ExampleLogo(int width, int height, int pixelsPerUnit)
		: m_Width(width), m_Height(height), m_PixelsPerUnit(pixelsPerUnit), m_Camera({ width, height }, pixelsPerUnit, 6.0f)
	{
	}

	const char* Name() override { return "Logo"; }

	void Enter() override
	{
		m_TextureRegistry.Create(1024, 16);
		m_Logo = m_TextureRegistry.Add("res\\textures\\Perplex.png", "res\\textures\\Perplex_Emission.png");

		m_Framebuffer.Create(m_Width, m_Height, true);

		m_BloomRenderer.Init(m_Width, m_Height);
		m_Tonemapper.Init(m_Width, m_Height);
		m_Pixelator.Init(m_Width, m_Height);
	}

	void Update(float ts) override
	{
		m_Framebuffer.Bind();

		// Render
		pxr::Renderer::UseTextureRegistry(m_TextureRegistry);
		pxr::Renderer::BeginBatch(m_Camera.GetProjection());

		static glm::vec3 perplexPosition = glm::vec3(-1.5f, -1.0f, 0.0f);
		static float perplexEmission = 0.6f;
		pxr::Renderer::DrawQuad(perplexPosition, m_Logo, perplexEmission);

		pxr::Renderer::EndBatch();
		pxr::Renderer::Flush();

		// Post processing
		static bool bloom = true;
		static float threshold = 1.0f;
		static float filterRadius = 0.003f;
		if (bloom)
		{
			m_BloomRenderer.RenderBloomTexture(m_Framebuffer.GetTextureID(), threshold, filterRadius);
			m_Framebuffer.DrawTexture(m_BloomRenderer.BloomTexture());
		}

		static bool tonemap = true;
		if (tonemap)
		{
			m_Tonemapper.RenderTonemap(m_Framebuffer.GetTextureID());
			m_Framebuffer.DrawTexture(m_Tonemapper.TonemappedTexture());
		}

		static bool pixelate = true;
		if (pixelate)
		{
			m_Pixelator.RenderPixelator(m_Framebuffer.GetTextureID(), m_Camera.GetPixelResolution());
			m_Framebuffer.DrawTexture(m_Pixelator.PixelatedTexture());
		}
		m_Framebuffer.DrawToScreen();

		// ImGui
		static bool showWindow = true;
		if (showWindow)
		{
			ImGui::Begin("Logo", &showWindow);
			
			ImGui::Text("Stats");
			std::string timeStr = std::format("{}", ts);
			std::string fpsStr = std::format("{}", 1.0f / ts);
			ImGui::LabelText("Timestep", timeStr.c_str());
			ImGui::LabelText("FPS", fpsStr.c_str());

			ImGui::Text("Camera");
			static float camZoom = m_Camera.GetZoom();
			ImGui::DragFloat("Zoom", &camZoom, 0.01f);
			m_Camera.SetZoom(camZoom);

			ImGui::Text("Logo");
			ImGui::DragFloat3("Position", &perplexPosition.x, 0.01f);
			ImGui::DragFloat("Emission", &perplexEmission, 0.01f);

			ImGui::Text("Post Processing");
			ImGui::Checkbox("Bloom", &bloom);
			ImGui::DragFloat("Threshold", &threshold, 0.01f);
			ImGui::SliderFloat("Filter radius", &filterRadius, 0.0f, 0.01f);
			ImGui::Checkbox("Tonemap", &tonemap);
			ImGui::Checkbox("Pixelate", &pixelate);
			ImGui::End();
		}
	}

	void Exit() override
	{
		m_TextureRegistry.Destroy();

		m_Framebuffer.Destroy();
		m_BloomRenderer.Destroy();
		m_Tonemapper.Destroy();
		m_Pixelator.Destroy();
	}

	void Resize(int width, int height) override
	{
		m_Camera.Resize({ width, height });
		// resizing bloom renderer breaks everything for some reason
		// m_BloomRenderer.Resize(width, height);
		m_Tonemapper.Resize(width, height);
		m_Framebuffer.Resize(width, height);
		m_Pixelator.Resize(width, height);

		m_Width = width;
		m_Height = height;
	}
private:
	int m_Width;
	int m_Height;
	int m_PixelsPerUnit;
	pxr::Camera m_Camera;

	pxr::TextureRegistry m_TextureRegistry;
	pxr::Texture m_Logo;

	pxr::Framebuffer m_Framebuffer;
	// Post processing
	pxr::BloomRenderer m_BloomRenderer;
	pxr::Tonemapper m_Tonemapper;
	pxr::Pixelator m_Pixelator;
};