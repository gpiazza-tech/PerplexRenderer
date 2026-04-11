#pragma once

#pragma once

#include "Example.h"

#include <pxr/pxr.h>

class ExampleScale : public Example
{
public:
	ExampleScale(const pxr::Camera* camera)
		: m_Camera(camera)
	{
	}

	const char* Name() override { return "Scale"; }

	void Enter() override
	{
		m_Scale = pxr::SpriteRegistry::GetSprite("res\\textures\\Scale.png");
	}

	void Update(float ts) override
	{
		// Render
		pxr::Renderer::BeginFrame();
		pxr::Renderer::BeginBatch(m_Camera->GetProjection());

		static glm::vec3 scalePosition = glm::vec3(-2.0f, -2.0f, 0.0f);
		static float scaleEmission = 0.13f;
		pxr::Renderer::DrawQuad(scalePosition, glm::vec2(1.0f), m_Scale, m_Scale, glm::vec4(1.0f), scaleEmission, true);

		pxr::Renderer::EndBatch();
		pxr::Renderer::Flush();

		// ImGui
		static bool showWindow = true;
		if (showWindow)
		{
			ImGui::Begin("Scale", &showWindow);

			ImGui::DragFloat2("Position", &scalePosition.x, 0.01f);
			ImGui::DragFloat("Emission", &scaleEmission, 0.01f);

			ImGui::End();
		}
	}

	void Exit() override
	{

	}
private:
	pxr::Sprite m_Scale;

	const pxr::Camera* m_Camera;
};