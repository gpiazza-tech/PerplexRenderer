#pragma once

#include <glm.hpp>

struct Particle
{
	glm::vec2 StartPosition = glm::vec2(0.0f);
	glm::vec2 Position = glm::vec2(0.0f);
	glm::vec2 Velocity = glm::vec2(0.0f);
	glm::vec4 Color = glm::vec4(1.0f);
};

struct Texture;

class ParticleSystem
{
public:
	ParticleSystem() = default;
	~ParticleSystem() {};

	void Create(size_t particleCount);
	void CreateFromTexture(const Texture& texture);
	void Restart();
	void Update(float speed);
	void Render();
private:
	std::vector<Particle> m_Particles;
	glm::vec2 m_Center;
};