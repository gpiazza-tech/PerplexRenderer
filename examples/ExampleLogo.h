#pragma once

#include "Example.h"

class ExampleLogo : public Example
{
public:
	const char* Name() override { return "Logo"; }

	void Enter() override
	{
		m_TextureRegistry.Create(1024, 16);
		m_Logo = m_TextureRegistry.Add("res\\textures\\Perplex.png", "res\\textures\\Perplex_Emission.png");
	}

	void Update(float ts) override
	{
		// Render
		pxr::Renderer::UseTextureRegistry(m_TextureRegistry);
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

			ImGui::Image(m_TextureRegistry.GetAtlasGroups()[0].ColorAtlas.GetAtlasTexture(), { 1080, 1080 });
			ImGui::Image(m_TextureRegistry.GetAtlasGroups()[0].EmissionAtlas.GetAtlasTexture(), { 1080, 1080 });

			ImGui::End();
		}

		UpdateProjection();
	}

	void Exit() override
	{
		m_TextureRegistry.Destroy();
	}
private:
	pxr::TextureRegistry m_TextureRegistry;
	pxr::Texture m_Logo;
};