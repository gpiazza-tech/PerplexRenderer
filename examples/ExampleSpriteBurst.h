#pragma once

#include "Example.h"

class ExampleSpriteBurst : public Example
{
public:
	ExampleSpriteBurst(const pxr::Camera* camera)
		: m_Camera(camera), 
		m_ParticleSettings(new pxr::ParticleSystemSettings()), 
		m_ParticleSystem(m_ParticleSettings)
	{
	}

	const char* Name() override { return "Sprite Burst"; }

	void Enter() override
	{
		m_Kablooey = pxr::SpriteRegistry::GetSprite("textures\\Kablooey.png");
		m_KablooeyEmission = pxr::SpriteRegistry::GetSprite("textures\\Kablooey_Emission.png");

		m_ParticleSettings->GravityMultiplier = 0.05f;
		m_ParticleSettings->VelocityMultiplier = 10.0f;
		m_ParticleSettings->EmissionMultiplier = 0.7f;
		m_ParticleSettings->ParticlesPerPixel = 1;
		m_ParticleSystem.Create(m_Kablooey, m_KablooeyEmission);
	}
	 
	void Update(float ts) override
	{
		// Render
		pxr::Renderer::BeginFrame();
		pxr::Renderer::BeginBatch(m_Camera->GetProjection());

		static glm::vec3 bobPosition = glm::vec3(-3.5f, 0.6f, 0.0f);
		static float bobEmission = 0.6f;

		m_ParticleSystem.Update(ts);
		m_ParticleSystem.Render(bobPosition);

		pxr::Renderer::EndBatch();
		pxr::Renderer::Flush();

		// ImGui
		static bool showWindow = true;
		if (showWindow)
		{
			ImGui::Begin("Sprite Burst", &showWindow);

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

			ImGui::End();
		}
	}

	void Exit() override
	{

	}
private:
	const pxr::Camera* m_Camera;

	pxr::Sprite m_Kablooey;
	pxr::Sprite m_KablooeyEmission;

	std::shared_ptr<pxr::ParticleSystemSettings> m_ParticleSettings;
	pxr::SpriteParticleSystem m_ParticleSystem;
};