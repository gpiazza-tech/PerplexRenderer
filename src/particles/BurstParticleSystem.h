#pragma once

#include "Particle.h"
#include "ParticleSystemSettings.h"

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace pxr
{
	class BurstParticleSystem
	{
	public:
		BurstParticleSystem() = default;
		BurstParticleSystem(std::shared_ptr<ParticleSystemSettings> settings)
			: m_Settings(settings)
		{
		}
		~BurstParticleSystem() {}

		void Create()
		{
			InitParticles();
		}

		void Play()
		{
			InitParticles();
			m_Playing = true;
		}
		void Pause() { m_Playing = false; }
		void Resume() { m_Playing = true; }
		void Reset() { InitParticles(); }

		inline bool Playing() const { return m_Playing; };

		void Update(float ts);
		void Render(glm::vec2 position);
	private:
		void InitParticles();
		void InitParticle(Particle& particle);
	private:
		std::vector<Particle> m_Particles;

		std::shared_ptr<ParticleSystemSettings> m_Settings;

		bool m_Playing = false;
		float m_Timer = 0.0f;
	};
}