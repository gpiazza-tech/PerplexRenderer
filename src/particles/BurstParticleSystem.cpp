#include <pch.h>
#include "BurstParticleSystem.h"

#include <rendering/Renderer.h>

#include <fwd.hpp>
#include <glm.hpp>

#include <vector>
#include <memory>

namespace pxr
{
    void BurstParticleSystem::Update(float ts)
    {
        if (!m_Playing)
            return;

        for (auto& particle : m_Particles)
        {
            particle.Position += particle.Velocity * m_Settings->VelocityMultiplier * m_Settings->Speed * ts;
            particle.Velocity.y -= m_Settings->GravityMultiplier * 9.8f * m_Settings->Speed * ts;
            particle.Velocity *= 1.0f - m_Settings->AirFriction * m_Settings->Speed * ts;
        }
    }

    void BurstParticleSystem::Render(glm::vec2 position)
    {
        for (auto& particle : m_Particles)
        {
            Renderer::DrawPixel(particle.Position + position, particle.Color, particle.Emission * m_Settings->EmissionMultiplier, m_Settings->PixelPerfect);
        }
    }


	void BurstParticleSystem::InitParticles()
	{
        m_Particles.clear();
        m_Particles.reserve(m_Settings->ParticleCount);

        for (int i = 0; i < m_Settings->ParticleCount; i++)
        {
            Particle& particle = m_Particles.emplace_back();
            InitParticle(particle);
        }
	}

	void BurstParticleSystem::InitParticle(Particle& particle)
	{
        particle.Position = { 0.0f, 0.0f };
        particle.Color = m_Settings->StartColor;
        particle.Emission = m_Settings->StartEmission;
        particle.Lifetime = m_Settings->StartLifetime;

        particle.Velocity = m_Settings->StartVelocity;
        float xRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings->VelocityRandomness;
        float yRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings->VelocityRandomness;
        particle.Velocity.x += xRand;
        particle.Velocity.y += yRand;
	}
}