#pragma once

#include <fwd.hpp>
#include <vector>

namespace pxr
{
	struct Particle
	{
		glm::vec2 Position = { 0.0f, 0.0f };
		glm::vec2 Velocity = { 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Emission = 0.0f;

		float Lifetime = 0.0f;
	};

	enum class ParticleSystemType
	{
		None = 0,
		Emissive,
		Burst
	};

	enum class ParticleSystemShape
	{
		None = 0,
		Circle,
		Rectangle
	};

	struct ParticleSystemSettings
	{
		union 
		{
			int ParticlesPerSecond;		// ParticleSystemType::Emissive
			int ParticleCount;			// ParticleSystemType::Burst
			int ParticlesPerPixel;		// ParticleSystemType::Burst from texture
		};

		float StartLifetime = 1.0f; // in seconds
		glm::vec2 SpawnBounds = { 1.0f, 1.0f };

		float Speed = 1.0f;

		float GravityMultiplier = 1.0f;
		float AirFriction = 0.05f;

		glm::vec2 StartVelocity = { 0.0f, 0.0f };
		float VelocityMultiplier = 1.0f;
		float VelocityRandomness = 0.1f; // range: 0.0f to 1.0f

		glm::vec4 StartColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float StartEmission = 0.0f;
		float EmissionMultiplier = 1.0f;

		bool PixelPerfect = true;
	};

	struct Texture;

	class ParticleSystem
	{
	public:
		ParticleSystem() = default;
		~ParticleSystem() {};

		void CreateEmissive();
		void CreateBurst(size_t particleCount);
		void CreateBurstFromTexture(const Texture& texture);
		void Destroy();

		void Reset();

		void Update(float ts);
		void Render(const glm::vec2& position);

		inline const ParticleSystemSettings& GetSettings() { return m_Settings; }
		inline void SetSettings(const ParticleSystemSettings& settings) { m_Settings = settings; }
	private:
		void UpdateEmissive(float ts);
		void UpdateBurst(float ts);
	private:
		std::vector<Particle> m_StartParticles;
		std::vector<Particle> m_Particles;
		glm::vec2 m_Center = { 0.0f, 0.0f };

		ParticleSystemSettings m_Settings;
		ParticleSystemType m_Type;

		float m_Timer = 0.0f;
	};
}