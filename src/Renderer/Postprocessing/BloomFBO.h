#pragma once

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

	bool Init(int windowWidth, int windowHeight, int mipChainLength);
	void Destroy();
	void Bind();
	inline const std::vector<BloomMip>& MipChain() const { return m_MipChain; }

	void Resize(int width, int height);
private:
	uint32_t m_FBO = 0;
	std::vector<BloomMip> m_MipChain;
};