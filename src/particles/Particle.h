#pragma once

#include <fwd.hpp>
#include <glm.hpp>

namespace pxr
{
	struct Particle
	{
		glm::vec2 Position = { 0.0f, 0.0f };
		glm::vec2 Velocity = { 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Emission = 0.0f;

		float Lifetime = 0.0f; // Seconds
	};
}