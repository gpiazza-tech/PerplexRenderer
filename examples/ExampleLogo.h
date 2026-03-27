#pragma once

#include "Example.h"

class ExampleLogo : public Example
{
public:
	const char* Name() override { return "Logo"; }

	void Enter() override
	{
		m_Logo = pxr::Renderer::GetTextureRegistry().Add("res\\textures\\Perplex.png", "res\\textures\\Perplex_Emission.png");
	}

	void Update(float ts) override
	{
		// Render
        pxr::Renderer::BeginBatch(m_Proj);

		static glm::vec3 perplexPosition = glm::vec3(-1.5f, -1.0f, 0.0f);
        static float perplexEmission = 0.6f;
        pxr::Renderer::DrawQuad(perplexPosition, m_Logo, perplexEmission);

        pxr::Renderer::EndBatch();
        pxr::Renderer::Flush();

		// ImGui
		static bool showWindow = true;
		if (showWindow)
		{
			ImGui::Begin("Logo", &showWindow);

			ImGui::DragFloat3("Position", &perplexPosition.x, 0.01f);
			ImGui::DragFloat("Emission", &perplexEmission, 0.01f);

			ImGui::End();
		}

		UpdateProjection();
	}

	void Exit() override
	{

	}
private:
	pxr::Texture m_Logo;
};