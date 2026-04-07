#pragma once

#include "Particle.h"
#include "ParticleSystemSettings.h"
#include <sprite/Sprite.h>

#include <fwd.hpp>
#include <glm.hpp>

#include <vector>
#include <memory>

namespace pxr
{
	class SpriteParticleSystem
	{
	public:
		SpriteParticleSystem() = default;
		SpriteParticleSystem(std::shared_ptr<ParticleSystemSettings> settings)
			: m_Settings(settings)
		{ }
		~SpriteParticleSystem() {}

		void Create(const Sprite& sprite);

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
	private:
		std::vector<Particle> m_SpriteParticles;
		std::vector<Particle> m_Particles;
		glm::vec2 m_Center = { 0.0f, 0.0f };

		std::shared_ptr<ParticleSystemSettings> m_Settings;

		bool m_Playing = false;
		float m_Timer = 0.0f;
	};
}