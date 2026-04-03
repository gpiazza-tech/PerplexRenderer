#pragma once

#include <glm.hpp>
#include <fwd.hpp>

namespace pxr
{
	enum class ParticleSystemType
	{
		None = 0,
		Emissive,
		Sprite,
		Burst,
	};

	struct ParticleSystemSettings
	{
		// System
		union
		{
			int ParticlesPerSecond = 0;	// ParticleSystemType::Emissive
			int ParticlesPerPixel;		// ParticleSystemType::Sprite
			int ParticleCount;			// ParticleSystemType::Burst
		};
		bool PixelPerfect = false;

		float Speed = 1.0f;
		float GravityMultiplier = 1.0f;
		float AirFriction = 0.05f;

		glm::vec2 SpawnBounds = { 1.0f, 1.0f };

		// Particles
		float StartLifetime = 1.0f; // in seconds
		float StartLifetimeRandomness = 0.0f;

		// Physics : used in ParticleSystem::Update()
		glm::vec2 StartVelocity = { 0.0f, 0.0f };
		float VelocityMultiplier = 1.0f;
		float VelocityRandomness = 0.1f;

		// Graphics : used in ParticleSystem::Render()
		glm::vec4 StartColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float StartColorRandomness = 0.0f;
		glm::vec4 EndColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float EndColorRandomness = 0.0f;

		float StartEmission = 0.0f;
		float StartEmissionRandomness = 0.0f;
		float EndEmission = 0.0f;
		float EndEmissionRandomness = 0.0f;

		float EmissionMultiplier = 1.0f;
	};
}