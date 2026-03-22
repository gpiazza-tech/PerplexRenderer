#include <pch.h>
#include "ParticleSystem.h"

#include "Renderer.h"
#include "Util.h"

void ParticleSystem::Create(size_t particleCount)
{
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

void ParticleSystem::CreateFromTexture(const Texture& texture)
{
    m_StartParticles.reserve(texture.PixelWidth * texture.PixelHeight);

    uint8_t* channels = (uint8_t*)malloc(texture.PixelWidth * texture.PixelHeight * 4);
    glGetTextureSubImage(texture.AtlasTexture, 0,
        texture.PixelX, texture.PixelY, 0, 
        texture.PixelWidth, texture.PixelHeight, 1, 
        GL_RGBA, GL_UNSIGNED_BYTE, 
        texture.PixelWidth * texture.PixelHeight * 4, 
        channels);

    float pixelsPerUnit = 16.0f; 
    m_Center = glm::vec2((float)texture.PixelWidth / pixelsPerUnit / 2.0f, (float)texture.PixelHeight / pixelsPerUnit / 2.0f);

    ASSERT(channels != nullptr);

    for (size_t i = 0; i < texture.PixelWidth * texture.PixelHeight * 4; i += 4)
    {
        uint8_t a = channels[i + 3];
        if (a == 0)
            continue;

        uint8_t r = channels[i];
        uint8_t g = channels[i + 1];
        uint8_t b = channels[i + 2];
        
        int pixelIndex = (int)i / 4;
        int pixelX = pixelIndex % texture.PixelWidth;
        int pixelY = pixelIndex / texture.PixelHeight;

        // Particle
        Particle particle = Particle();
        particle.Color = glm::vec4(r / (float)255, g / (float)255, b / (float)255, a / (float)255);
        particle.Position = glm::vec2(pixelX / 16.0f, pixelY / 16.0f);

        // Particle Velocity
        particle.Velocity = glm::vec2(particle.Position - m_Center) + m_Settings.StartVelocity;
        float xRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.VelocityRandomness;
        float yRand = ((float)std::rand() / RAND_MAX - 0.5f) * m_Settings.VelocityRandomness;
        particle.Velocity.x += xRand;
        particle.Velocity.y += yRand;

        m_StartParticles.emplace_back(particle);
    }

    m_Particles = m_StartParticles;
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
    for (auto& particle : m_Particles)
    {
        particle.Position += particle.Velocity * m_Settings.VelocityMultiplier * m_Settings.Speed * ts;
        particle.Velocity.y -= m_Settings.GravityMultiplier * 9.8f * m_Settings.Speed * ts;
        particle.Velocity *= 1.0f - m_Settings.AirFriction * m_Settings.Speed * ts;
    }
}

void ParticleSystem::Render()
{
    for (auto& particle : m_Particles)
    {
        Renderer::DrawPixel(particle.Position, particle.Color, m_Settings.PixelPerfect);
    }
}
