#include <pch.h>
#include "BloomFBO.h"

bool BloomFBO::Init(uint32_t windowWidth, uint32_t windowHeight, uint32_t mipChainLength)
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
	glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);

	m_MipChain.reserve(mipChainLength);
	
	for (size_t i = 0; i < mipChainLength; i++)
	{
		BloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.Size = mipSize;
		mip.IntSize = mipIntSize;

		glGenTextures(1, &mip.Texture);
		glBindTexture(GL_TEXTURE_2D, mip.Texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_MipChain.emplace_back(mip);
	}

	// attach first mip texture to color attachment 0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MipChain[0].Texture, 0);

	// define color attachment 0 as the output of fragment shaders
	uint32_t attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// check if complete
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "bloom FBO error, status: " << status << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void BloomFBO::Destroy()
{
	for (size_t i = 0; i < m_MipChain.size(); i++)
	{
		glDeleteTextures(1, &m_MipChain[i].Texture);
		m_MipChain[i].Texture = 0;
	}
	glDeleteFramebuffers(1, &m_FBO);
	m_FBO = 0;
}

void BloomFBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}