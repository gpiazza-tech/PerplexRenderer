#pragma once

#include "Example.h"

#include <pxr/pxr.h>

class ExampleLogo : public Example
{
public:
	ExampleLogo(const pxr::Camera* camera)
		: m_Camera(camera)
	{
	}

	const char* Name() override { return "Logo"; }

	void Enter() override
	{
		m_Logo = pxr::SpriteRegistry::GetSprite("textures\\Perplex.png");
		m_LogoEmission = pxr::SpriteRegistry::GetSprite("textures\\Perplex_Emission.png");
	}

	void Update(float ts) override
	{
		// Render
		pxr::Renderer::BeginFrame();
		pxr::Renderer::BeginBatch(m_Camera->GetProjection());

		static glm::vec3 perplexPosition = glm::vec3(-1.5f, -1.0f, 0.0f);
		static float perplexEmission = 0.6f;
		pxr::Renderer::DrawQuad(perplexPosition, glm::vec2(1.0f), m_Logo, m_LogoEmission, glm::vec4(1.0f), perplexEmission, true);

		pxr::Renderer::EndBatch();
		pxr::Renderer::Flush();

		// ImGui
		static bool showWindow = true;
		if (showWindow)
		{
			ImGui::Begin("Logo", &showWindow);

			ImGui::DragFloat2("Position", &perplexPosition.x, 0.01f);
			ImGui::DragFloat("Emission", &perplexEmission, 0.01f);

			ImGui::End();
		}
	}

	void Exit() override
	{
		
	}
private:
	pxr::Sprite m_Logo;
	pxr::Sprite m_LogoEmission;

	const pxr::Camera* m_Camera;
};