#include <pch.h>
#include "SpriteParticleSystem.h"

#include "Particle.h"
#include <sprite/Sprite.h>
#include <sprite/SpriteRegistry.h>
#include <rendering/Renderer.h>

#include <fwd.hpp>

#include <cstdlib>

namespace pxr
{
	void SpriteParticleSystem::Create(const Sprite& colorSprite, const Sprite& emissionSprite)
	{
        m_SpriteParticles.clear();
        m_SpriteParticles.reserve(colorSprite.PixelWidth * colorSprite.PixelHeight);

        glm::u8vec4* colorPixels = new glm::u8vec4[colorSprite.PixelWidth * colorSprite.PixelHeight];
        SpriteRegistry::FetchPixels(colorSprite, colorPixels);
        glm::u8vec4* emissionPixels = new glm::u8vec4[emissionSprite.PixelWidth * emissionSprite.PixelHeight];
        SpriteRegistry::FetchPixels(emissionSprite, emissionPixels);

        float pixelsPerUnit = 16.0f;
        m_Center = glm::vec2((float)colorSprite.PixelWidth / pixelsPerUnit / 2.0f, (float)colorSprite.PixelHeight / pixelsPerUnit / 2.0f);

        glm::vec2 positionOffset = {  };
        for (size_t i = 0; i < colorSprite.PixelWidth * colorSprite.PixelHeight; i++)
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

            int pixelX = i % colorSprite.PixelWidth;
            int pixelY = i / colorSprite.PixelWidth;

            // Particle
            Particle particle = Particle();
            particle.Color = { rColor, gColor, bColor, aColor };
            particle.Position = { i % colorSprite.PixelWidth / pixelsPerUnit, i / colorSprite.PixelWidth / pixelsPerUnit };
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