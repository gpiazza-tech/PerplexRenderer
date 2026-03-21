#pragma once

#include <glm.hpp>

struct BloomMip
{
	glm::vec2 Size;
	glm::ivec2 IntSize;
	uint32_t Texture;
};

class BloomFBO
{
public:
	BloomFBO() = default;
	~BloomFBO() {}

	bool Init(uint32_t windowWidth, uint32_t windowHeight, uint32_t mipChainLength);
	void Destroy();
	void Bind();
	inline const std::vector<BloomMip>& MipChain() const { return m_MipChain; }
private:
	uint32_t m_FBO = 0;
	std::vector<BloomMip> m_MipChain;
};