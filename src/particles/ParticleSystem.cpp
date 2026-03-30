#include <pch.h>
#include "ParticleSystem.h"

#include <rendering/Renderer.h>
#include <texture/Texture.h>

#include <GL/glew.h>
#include <fwd.hpp>

#include <iostream>
#include <malloc.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>

namespace pxr
{
    void ParticleSystem::CreateBurst(size_t particleCount)
    {
        m_Type = ParticleSystemType::Burst;

        m_Particles.reserve(particleCount);
        for (size_t i = 0; i < particleCount; i++)
            m_Particles.emplace_back(Particle());
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        for (auto& particle : m_Particles)
        {
            float randX = (float)std::rand() / RAND_MAX - 0.5f;
            float randY = (float)std::rand() / RAND_MAX - 0.5f;
            float magnitude = std::sqrt(randX * randX + randY * randY);
            randX /= (magnitude > 0.5f) ? magnitude : 1.0f;
            randY /= (magnitude > 0.5f) ? magnitude : 1.0f;
            particle.Velocity = { randX, randY };
        }
    }

    void ParticleSystem::CreateBurstFromTexture(const Texture& texture)
    {
        m_Type = ParticleSystemType::Burst;

        m_StartParticles.reserve(texture.PixelWidth * texture.PixelHeight);

        // Color
        uint8_t* colorChannels = (uint8_t*)malloc(texture.PixelWidth * texture.PixelHeight * 4);
        glGetTextureSubImage(texture.ColorAtlas, 0,
            texture.PixelX, texture.PixelY, 0,
            texture.PixelWidth, texture.PixelHeight, 1,
            GL_RGBA, GL_UNSIGNED_BYTE,
            texture.PixelWidth * texture.PixelHeight * 4,
            colorChannels);

        // Emission
        uint8_t* emitChannels = (uint8_t*)malloc(texture.PixelWidth * texture.PixelHeight * 4);
        glGetTextureSubImage(texture.EmissionAtlas, 0,
            texture.PixelX, texture.PixelY, 0,
            texture.PixelWidth, texture.PixelHeight, 1,
            GL_RGBA, GL_UNSIGNED_BYTE,
            texture.PixelWidth * texture.PixelHeight * 4,
            emitChannels);

        float pixelsPerUnit = 16.0f;
        m_Center = glm::vec2((float)texture.PixelWidth / pixelsPerUnit / 2.0f, (float)texture.PixelHeight / pixelsPerUnit / 2.0f);

        ASSERT(colorChannels != nullptr);

        glm::vec2 positionOffset = {  };
        for (size_t i = 0; i < texture.PixelWidth * texture.PixelHeight * 4; i += 4)
        {
            float aColor = colorChannels[i + 3] / (float)255;
            if (aColor == 0.0f)
                continue;

            float rColor = colorChannels[i + 0] / (float)255;
            float gColor = colorChannels[i + 1] / (float)255;
            float bColor = colorChannels[i + 2] / (float)255;

            float rEmit = emitChannels[i + 0] / (float)255;
            float gEmit = emitChannels[i + 1] / (float)255;
            float bEmit = emitChannels[i + 2] / (float)255;
            float aEmit = emitChannels[i + 3] / (float)255;

            int pixelIndex = (int)i / 4;
            int pixelX = pixelIndex % texture.PixelWidth;
            int pixelY = pixelIndex / texture.PixelWidth;

            // Particle
            Particle particle = Particle();
            particle.Color = glm::vec4(rColor, gColor, bColor, aColor);
            particle.Position = glm::vec2(pixelX / pixelsPerUnit, pixelY / pixelsPerUnit);
            particle.Emission = rEmit; // only red channel contributes to emission

            // Particle Velocity
            particle.Velocity = glm::vec2(particle.Position - m_Center) + m_Settings.StartVelocity;
            float xRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.VelocityRandomness;
            float yRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.VelocityRandomness;
            particle.Velocity.x += xRand;
            particle.Velocity.y += yRand;

            m_StartParticles.emplace_back(particle);
        }

        m_Particles = m_StartParticles;

        free(colorChannels);
        free(emitChannels);
    }

    void ParticleSystem::CreateEmissive()
    {
        m_Type = ParticleSystemType::Emissive;
    }

    void ParticleSystem::Reset()
    {
        for (auto& particle : m_StartParticles)
        {
            particle.Velocity = glm::vec2(particle.Position - m_Center) + m_Settings.StartVelocity;
            float xRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.VelocityRandomness;
            float yRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.VelocityRandomness;
            particle.Velocity.x += xRand;
            particle.Velocity.y += yRand;
        }

        m_Particles = m_StartParticles;
    }

    void ParticleSystem::Update(float ts)
    {
        switch (m_Type)
        {
        case ParticleSystemType::Emissive:
            UpdateEmissive(ts);
            break;
        case ParticleSystemType::Burst:
            UpdateBurst(ts);
            break;
        default:
            std::cout << "ParticleSystemType not supported!" << std::endl;
            break;
        }
    }

    void ParticleSystem::Render(const glm::vec2& position)
    {
        for (auto& particle : m_Particles)
        {
            Renderer::DrawPixel(particle.Position + position, particle.Color, particle.Emission * m_Settings.EmissionMultiplier, m_Settings.PixelPerfect);
        }
    }

    void ParticleSystem::UpdateBurst(float ts)
    {
        for (auto& particle : m_Particles)
        {
            particle.Position += particle.Velocity * m_Settings.VelocityMultiplier * m_Settings.Speed * ts;
            particle.Velocity.y -= m_Settings.GravityMultiplier * 9.8f * m_Settings.Speed * ts;
            particle.Velocity *= 1.0f - m_Settings.AirFriction * m_Settings.Speed * ts;
        }
    }

    void ParticleSystem::UpdateEmissive(float ts)
    {
        // Update Particles
        for (size_t i = 0; i < m_Particles.size(); i++)
        {
            if (m_Particles[i].Lifetime < 0.0f)
            {
                m_Particles.erase(m_Particles.begin() + i);
                continue;
            }

            m_Particles[i].Position += m_Particles[i].Velocity * m_Settings.VelocityMultiplier * m_Settings.Speed * ts;
            m_Particles[i].Velocity.y -= m_Settings.GravityMultiplier * 9.8f * m_Settings.Speed * ts;
            m_Particles[i].Velocity *= 1.0f - m_Settings.AirFriction * m_Settings.Speed * ts;
            m_Particles[i].Lifetime -= m_Settings.Speed * ts;
        }

        // Add Particles
        m_Timer += ts;
        if (m_Timer > 1.0f / m_Settings.ParticlesPerSecond)
        {
            m_Timer = 0.0f;
            Particle newParticle = Particle();
            newParticle.Position = {
                ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.SpawnBounds.x,
                ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.SpawnBounds.y
            };
            newParticle.Velocity = m_Settings.StartVelocity;
            newParticle.Color = m_Settings.StartColor;
            newParticle.Emission = m_Settings.StartEmission;
            newParticle.Lifetime = m_Settings.StartLifetime;
            m_Particles.emplace_back(newParticle);
        }
    }

    void ParticleSystem::Destroy()
    {
        m_Timer = 0.0f;
        m_Settings = ParticleSystemSettings();
        m_Particles.clear();
        m_StartParticles.clear();
        m_Type = ParticleSystemType::None;
    }
}