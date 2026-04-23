#pragma once

#include <glm/fwd.hpp>

namespace pxr
{
	enum class ScalingMode 
	{
		None = 0, 
		Width, 
		Height, 
		LargerSide,
		SmallerSide
	};

	class Camera
	{
	public:
		Camera()
			: m_Resolution(1.0f), m_PixelsPerUnit(16), m_Zoom(1.0f), m_ScalingMode(ScalingMode::SmallerSide)
		{
			CalculateProjection();
		}
		Camera(const glm::vec2& screenResolution, int pixelsPerUnit, float zoom, ScalingMode scalingMode = ScalingMode::SmallerSide)
			: m_Resolution(screenResolution), m_PixelsPerUnit(pixelsPerUnit), m_Zoom(zoom), m_ScalingMode(scalingMode)
		{
			CalculateProjection();
		}
		~Camera() {}

		inline int GetPixelsPerUnit() const { return m_PixelsPerUnit; }
		inline const glm::mat4& GetProjection() const { return m_Projection; };

		inline void SetPixelPerfect(bool pixelPerfect) { m_PixelPerfect = pixelPerfect; }
		inline const glm::vec2& GetPixelResolution() { return m_PixelResolution; }

		inline float GetZoom() const { return m_Zoom; }
		void SetZoom(float zoom);

		inline ScalingMode GetScalingMode() { return m_ScalingMode; }
		void SetScalingMode(ScalingMode scalingMode);

		void Resize(const glm::vec2& resolution);
	protected:
		void CalculateProjection();
	private:
		float m_PixelsPerUnit; // number of pixels in one unit
		float m_Zoom; // how many units the screen width (or height or smallest) takes up
		float m_AspectRatio; // Width / Height

		glm::mat4 m_Projection;

		bool m_PixelPerfect;
		glm::vec2 m_Resolution;
		glm::vec2 m_PixelResolution;

		ScalingMode m_ScalingMode;
	};
}