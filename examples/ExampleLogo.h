#pragma once

#include <pxr/pxr.h>
#include "Example.h"

class ExampleLogo : public Example
{
public:
	ExampleLogo(int width, int height, int pixelsPerUnit, const pxr::Camera* camera)
		: m_Width(width), m_Height(height), m_PixelsPerUnit(pixelsPerUnit), m_Camera(camera)
	{
	}

	const char* Name() override { return "Logo"; }

	void Enter() override
	{
		m_Logo = pxr::SpriteRegistry::Add("res\\textures\\Perplex.png", "res\\textures\\Perplex_Emission.png");
	}

	void Update(float ts) override
	{
		// Render
		pxr::Renderer::BeginFrame();
		pxr::Renderer::BeginBatch(m_Camera->GetProjection());

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

			ImGui::DragFloat2("Position", &perplexPosition.x, 0.01f);
			ImGui::DragFloat("Emission", &perplexEmission, 0.01f);

			ImGui::End();
		}
	}

	void Exit() override
	{
		
	}

	void Resize(int width, int height) override
	{
		m_Width = width;
		m_Height = height;
	}
private:
	int m_Width;
	int m_Height;
	int m_PixelsPerUnit;
	const pxr::Camera* m_Camera;

	pxr::Sprite m_Logo;
};