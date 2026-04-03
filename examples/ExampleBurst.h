#pragma once

#include "Example.h"

class ExampleBurst : public Example
{
public:
	ExampleBurst(int width, int height, int pixelsPerUnit)
		: m_Width(width), m_Height(height), m_PixelsPerUnit(pixelsPerUnit), 
		m_Camera({ width, height }, pixelsPerUnit, 6.0f),
		m_ParticleSettings(new pxr::ParticleSystemSettings()), m_ParticleSystem(m_ParticleSettings)
	{
	}

	const char* Name() override { return "Burst"; }

	void Enter() override
	{
		m_TextureRegistry.Create(3, 16);

		m_ParticleSettings->ParticleCount = 20;
		m_ParticleSettings->GravityMultiplier = 0.05f;
		m_ParticleSettings->VelocityMultiplier = 10.0f;
		m_ParticleSettings->VelocityRandomness = 1.0f;
		m_ParticleSettings->StartEmission = 0.6f;
		m_ParticleSystem.Create();

		m_Framebuffer.Create(m_Width, m_Height, true);

		m_BloomRenderer.Init(m_Width, m_Height);
		m_Tonemapper.Init(m_Width, m_Height);
		m_Pixelator.Init(m_Width, m_Height);
	}

	void Update(float ts) override
	{
		m_Framebuffer.Bind();

		// Render
		m_TextureRegistry.Bind();
		pxr::Renderer::BeginBatch(m_Camera.GetProjection());

		static glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

		m_ParticleSystem.Update(ts);
		m_ParticleSystem.Render(position);

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
			ImGui::Begin("Texture Burst", &showWindow);

			ImGui::Text("Stats");
			std::string timeStr = std::format("{}", ts);
			std::string fpsStr = std::format("{}", 1.0f / ts);
			ImGui::LabelText("Timestep", timeStr.c_str());
			ImGui::LabelText("FPS", fpsStr.c_str());

			ImGui::Text("Camera");
			static float camZoom = m_Camera.GetZoom();
			ImGui::DragFloat("Zoom", &camZoom, 0.01f);
			m_Camera.SetZoom(camZoom);
			static const char* scalingModeNames[]{ "Width", "Height", "Larger Side", "Smaller Side" };
			static int scalingMode = 0;
			if (ImGui::Combo("Scaling Mode", &scalingMode, scalingModeNames, IM_ARRAYSIZE(scalingModeNames)))
				m_Camera.SetScalingMode((pxr::ScalingMode)(scalingMode + 1));

			ImGui::Text("Particle System");
			if (m_ParticleSystem.Playing() && ImGui::Button("Pause"))
				m_ParticleSystem.Pause();
			else if (!m_ParticleSystem.Playing() && ImGui::Button("Play"))
				m_ParticleSystem.Resume();

			if (ImGui::Button("Reset"))
			{
				m_ParticleSystem.Pause();
				m_ParticleSystem.Reset();
			}

			ImGui::Dummy({ 0, 10 });
			ImGui::Text("Particle Settings");
			ImGui::DragFloat("Speed", &m_ParticleSettings->Speed, 0.01f);
			ImGui::Dummy({ 0, 5 });
			ImGui::DragFloat("Gravity Multiplier", &m_ParticleSettings->GravityMultiplier, 0.01f);
			ImGui::DragFloat("Air Friction", &m_ParticleSettings->AirFriction, 0.01f);
			ImGui::DragFloat2("Start Velocity", &m_ParticleSettings->StartVelocity.x, 0.01f);
			ImGui::DragFloat("Velocity Multiplier", &m_ParticleSettings->VelocityMultiplier, 0.01f);
			ImGui::DragFloat("Velocity Randomness", &m_ParticleSettings->VelocityRandomness, 0.01f);
			ImGui::Dummy({ 0, 5 });
			ImGui::DragFloat4("Start Color", &m_ParticleSettings->StartColor.x, 0.01f);
			ImGui::DragFloat("Start Emission", &m_ParticleSettings->StartEmission, 0.01f);
			ImGui::Checkbox("Pixel Perfect", &m_ParticleSettings->PixelPerfect);

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

		m_BloomRenderer.Resize(width, height);
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

	std::shared_ptr<pxr::ParticleSystemSettings> m_ParticleSettings;
	pxr::BurstParticleSystem m_ParticleSystem;

	pxr::Framebuffer m_Framebuffer;

	// Post processing
	pxr::BloomRenderer m_BloomRenderer;
	pxr::Tonemapper m_Tonemapper;
	pxr::Pixelator m_Pixelator;
};