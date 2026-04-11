#pragma once

#include "Example.h"

#include <pxr/pxr.h>

class ExampleSmile : public Example
{
public:
	ExampleSmile(const pxr::Camera* camera)
		: m_Camera(camera)
	{
	}

	const char* Name() override { return "Smile"; }

	void Enter() override
	{
		 
	}

	void Update(float ts) override
	{
		pxr::Renderer::BeginFrame();
		pxr::Renderer::BeginBatch(m_Camera->GetProjection());

		pxr::Renderer::DrawPixel({ -0.0625f, 0.0625f });
		pxr::Renderer::DrawPixel({ 0.0625f, 0.0625f });
		pxr::Renderer::DrawPixel({ -0.125f, -0.0625f });
		pxr::Renderer::DrawPixel({ -0.0625f, -0.125f });
		pxr::Renderer::DrawPixel({ 0.0f, -0.125f });
		pxr::Renderer::DrawPixel({ 0.0625f, -0.125f });
		pxr::Renderer::DrawPixel({ 0.125f, -0.0625f });

		pxr::Renderer::EndBatch();
		pxr::Renderer::Flush();
	}

	void Exit() override
	{

	}
private:
	const pxr::Camera* m_Camera;
};