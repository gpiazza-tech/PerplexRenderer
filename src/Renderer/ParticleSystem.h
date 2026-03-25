#pragma once

struct Particle
{
	glm::vec2 Position = { 0.0f, 0.0f };
	glm::vec2 Velocity = { 0.0f, 0.0f };
	glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct ParticleSystemSettings
{
	float Speed = 1.0f;

	float GravityMultiplier = 1.0f;
	float AirFriction = 0.05f;

	glm::vec2 StartVelocity = { 0.0f, 0.0f };
	float VelocityMultiplier = 1.0f;
	float VelocityRandomness = 0.1f; // range: 0.0f to 1.0f

	bool PixelPerfect = true;
};

struct Texture;

class ParticleSystem
{
public:
	ParticleSystem() = default;
	~ParticleSystem() {};

	void Create(size_t particleCount);
	void CreateFromTexture(const Texture& texture);

	void Reset();

	void Update(float speed);
	void Render(const glm::vec2& position);

	inline const ParticleSystemSettings& GetSettings() { return m_Settings; }
	inline void SetSettings(const ParticleSystemSettings& settings) { m_Settings = settings; }
private:
	std::vector<Particle> m_StartParticles;
	std::vector<Particle> m_Particles;
	glm::vec2 m_Center = { 0.0f, 0.0f };
	ParticleSystemSettings m_Settings;
};