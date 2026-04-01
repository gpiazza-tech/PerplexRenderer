#include <pch.h>
#include "Camera.h"

#include <util/Util.h>

#include <glm.hpp>
#include <fwd.hpp>

#include <algorithm>
#include <iostream>

namespace pxr
{
	void Camera::CalculateViewProjection()
	{
		float ratio = 0;

		switch (m_ScalingMode)
		{
		case ScalingMode::Width:
			ratio = 1.0f / m_Resolution.x;
			break;
		case ScalingMode::Height:
			ratio = 1.0f / m_Resolution.y;
			break;
		case ScalingMode::SmallerSide:
			ratio = std::max(1.0f / (float)m_Resolution.x, 1.0f / (float)m_Resolution.y);
			break;
		case ScalingMode::LargerSide:
			ratio = std::min(1.0f / (float)m_Resolution.x, 1.0f / (float)m_Resolution.y);
			break;
		default:
			std::cout << "Camera Error: Scalling mode not supported!" << std::endl;
			break;
		}

		float semiWidth = RoundToNearestFraction((float)m_Resolution.x * ratio * m_Zoom / 2.0f, m_PixelsPerUnit);
		float semiHeight = RoundToNearestFraction((float)m_Resolution.y * ratio * m_Zoom / 2.0f, m_PixelsPerUnit);

		m_Projection = glm::ortho(-semiWidth, semiWidth, -semiHeight, semiHeight, -1.0f, 1.0f);
		m_PixelResolution = { m_PixelsPerUnit * semiWidth * 2.0f, m_PixelsPerUnit * semiHeight * 2.0f };
	}

	void Camera::SetZoom(float zoom)
	{ 
		m_Zoom = zoom;
		CalculateViewProjection();
	}

	void Camera::SetScalingMode(ScalingMode scalingMode)
	{
		m_ScalingMode = scalingMode;
		CalculateViewProjection();
	}

	void Camera::Resize(const glm::vec2& resolution)
	{
		m_Resolution = resolution;
		CalculateViewProjection();
	}
}