#pragma once

class Example
{
public:
	Example() {};

	virtual const char* Name() = 0;

	virtual void Enter() = 0;
	virtual void Update(float ts) = 0;
	virtual void Exit() = 0;

	virtual void Resize(int width, int height)
	{
		m_Width = width;
		m_Height = height;
	}

	void UpdateProjection()
	{
		float ratio = std::min(2.0 / (float)m_Width, 1.5 / (float)m_Height);
		float semiWidth = pxr::RoundToNearestFraction((float)m_Width * ratio * m_CameraZoom, m_PixelsPerUnit);
		float semiHeight = pxr::RoundToNearestFraction((float)m_Height * ratio * m_CameraZoom, m_PixelsPerUnit);

		m_Proj = glm::ortho(-semiWidth, semiWidth, -semiHeight, semiHeight, -1.0f, 1.0f);
	}
protected:
	uint32_t m_Width = 1920;
	uint32_t m_Height = 1080;
	uint32_t m_PixelsPerUnit = 16;

	glm::mat4 m_Proj = glm::mat4(1.0f);
	float m_CameraZoom = 2.0f;
};