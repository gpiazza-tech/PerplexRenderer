#pragma once

#include "../TextureBuffer.h"

#include <vector>
#include <cstdint>

class BloomFBO
{
public:
	BloomFBO() = default;
	~BloomFBO() {}

	bool Init(int windowWidth, int windowHeight, int mipChainLength);
	void Destroy();
	void Bind();
	inline std::vector<TextureBuffer>& MipChain() { return m_MipChain; }

	void Resize(int width, int height);
private:
	uint32_t m_FBO = 0;
	std::vector<TextureBuffer> m_MipChain;
};