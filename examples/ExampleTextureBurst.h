#pragma once

#include "Example.h"

class ExampleTextureBurst : public Example
{
public:
	const char* Name() override { return "Burst"; }

	void Enter() override
	{
		m_TextureRegistry.Create(1024, 16);
		m_Bob = m_TextureRegistry.Add("res\\textures\\Kablooey.png", "res\\textures\\Kablooey_Emission.png");
		m_ParticleSystem.CreateBurstFromTexture(m_Bob);

		m_ParticleSettings.ParticlesPerPixel = 1;
		m_ParticleSettings.GravityMultiplier = 0.05f;
		m_ParticleSettings.VelocityMultiplier = 10.0f;
		m_ParticleSettings.EmissionMultiplier = 0.7f;
	}
	 
	void Update(float ts) override
	{
		// Render
		pxr::Renderer::UseTextureRegistry(m_TextureRegistry);
		pxr::Renderer::BeginBatch(m_Proj);

		static glm::vec3 bobPosition = glm::vec3(-3.5f, 0.6f, 0.0f);
		static float bobEmission = 0.6f;

		static bool play = false;
		if (play)
			m_ParticleSystem.Update(ts);
		m_ParticleSystem.Render(bobPosition);

		pxr::Renderer::EndBatch();
		pxr::Renderer::Flush();

		// ImGui
		static bool showWindow = true;
		if (showWindow)
		{
			ImGui::Begin("Burst", &showWindow);

			ImGui::DragFloat("Camera Zoom", &m_CameraZoom, 0.01f);
			ImGui::DragFloat3("Position", &bobPosition.x, 0.01f);
			ImGui::DragFloat("Emission", &bobEmission, 0.01f);

			if (play && ImGui::Button("Stop"))
				play = false;
			else if (!play && ImGui::Button("Play"))
				play = true;

			if (ImGui::Button("Reset"))
			{
				play = false;
				m_ParticleSystem.Reset();
			}

			ImGui::Dummy({ 0, 10 });
			ImGui::Text("Particle Settings");
			ImGui::DragFloat("Speed", &m_ParticleSettings.Speed, 0.01f);
			ImGui::DragInt("Particles Per Pixel", &m_ParticleSettings.ParticlesPerPixel);
			ImGui::Dummy({ 0, 5 });
			ImGui::DragFloat("Gravity Multiplier", &m_ParticleSettings.GravityMultiplier, 0.01f);
			ImGui::DragFloat("Air Friction", &m_ParticleSettings.AirFriction, 0.01f);
			ImGui::DragFloat2("Start Velocity", &m_ParticleSettings.StartVelocity.x, 0.01f);
			ImGui::DragFloat("Velocity Multiplier", &m_ParticleSettings.VelocityMultiplier, 0.01f);
			ImGui::DragFloat("Velocity Randomness", &m_ParticleSettings.VelocityRandomness, 0.01f);
			ImGui::Dummy({ 0, 5 });
			ImGui::DragFloat4("Start Color", &m_ParticleSettings.StartColor.x, 0.01f);
			ImGui::DragFloat("Start Emission", &m_ParticleSettings.StartEmission, 0.01f);
			ImGui::DragFloat("Emission Multiplier", &m_ParticleSettings.EmissionMultiplier, 0.01f);
			ImGui::Checkbox("Pixel Perfect", &m_ParticleSettings.PixelPerfect);

			m_ParticleSystem.SetSettings(m_ParticleSettings);

			ImGui::Dummy({ 0, 30 });
			ImGui::Text("Texture Preview");
			ImGui::Image(m_TextureRegistry.GetAtlasGroups()[0].ColorAtlas.GetAtlasTexture(), { 1080, 1080 });
			ImGui::Image(m_TextureRegistry.GetAtlasGroups()[0].EmissionAtlas.GetAtlasTexture(), { 1080, 1080 });

			ImGui::End();
		}

		UpdateProjection();
	}

	void Exit() override
	{
		m_TextureRegistry.Destroy();
		m_ParticleSystem.Destroy();
	}
private:
	pxr::TextureRegistry m_TextureRegistry;
	pxr::Texture m_Bob;

	pxr::ParticleSystemSettings m_ParticleSettings;
	pxr::ParticleSystem m_ParticleSystem;
};