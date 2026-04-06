#include <pch.h>
#include "SpriteParticleSystem.h"

#include "Particle.h"
#include <texture/Texture.h>
#include <texture/TextureRegistry.h>
#include <rendering/Renderer.h>

#include <fwd.hpp>

#include <cstdlib>

namespace pxr
{
	void SpriteParticleSystem::Create(const Texture& sprite)
	{
        m_SpriteParticles.clear();
        m_SpriteParticles.reserve(sprite.PixelWidth * sprite.PixelHeight);

        glm::u8vec4* colorPixels = sprite.TextureRegistry->FetchColorPixels(sprite);
        glm::u8vec4* emissionPixels = sprite.TextureRegistry->FetchEmissionPixels(sprite);

        float pixelsPerUnit = 16.0f;
        m_Center = glm::vec2((float)sprite.PixelWidth / pixelsPerUnit / 2.0f, (float)sprite.PixelHeight / pixelsPerUnit / 2.0f);

        glm::vec2 positionOffset = {  };
        for (size_t i = 0; i < sprite.PixelWidth * sprite.PixelHeight; i++)
        {
            float aColor = colorPixels[i].a / (float)255;
            if (aColor == 0.0f)
                continue;

            float rColor = colorPixels[i].r / (float)255;
            float gColor = colorPixels[i].g / (float)255;
            float bColor = colorPixels[i].b / (float)255;

            float rEmit = emissionPixels[i].r / (float)255;
            float gEmit = emissionPixels[i].g / (float)255;
            float bEmit = emissionPixels[i].b / (float)255;
            float aEmit = emissionPixels[i].a / (float)255;

            int pixelX = i % sprite.PixelWidth;
            int pixelY = i / sprite.PixelWidth;

            // Particle
            Particle particle = Particle();
            particle.Color = { rColor, gColor, bColor, aColor };
            particle.Position = { i % sprite.PixelWidth / pixelsPerUnit, i / sprite.PixelWidth / pixelsPerUnit };
            particle.Emission = rEmit; // only red channel contributes to emission

            m_SpriteParticles.emplace_back(particle);
        }

        delete[] colorPixels;
        delete[] emissionPixels;

        InitParticles();
	}

	void SpriteParticleSystem::Update(float ts)
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

    void SpriteParticleSystem::Render(glm::vec2 position)
    {
        for (auto& particle : m_Particles)
        {
            Renderer::DrawPixel(particle.Position + position, particle.Color, particle.Emission * m_Settings->EmissionMultiplier, m_Settings->PixelPerfect);
        }
    }

    void SpriteParticleSystem::InitParticles()
    {
        m_Particles.clear();
        m_Particles.reserve(m_SpriteParticles.size() * m_Settings->ParticlesPerPixel);

        for (const auto& particle : m_SpriteParticles)
        {
            for (int i = 0; i < m_Settings->ParticlesPerPixel; i++)
            {
                Particle newParticle = Particle(particle);

                newParticle.Velocity = glm::vec2(newParticle.Position - m_Center) + m_Settings->StartVelocity;
                newParticle.Velocity.x += ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings->VelocityRandomness;
                newParticle.Velocity.y += ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings->VelocityRandomness;

                m_Particles.emplace_back(newParticle);
            }
        }
    }
}