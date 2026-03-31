#include <pch.h>
#include "BloomFBO.h"

#include <rendering/TextureBuffer.h>

#include <GL/glew.h>
#include <fwd.hpp>
#include <cstdint>
#include <iostream>
#include <ostream>

namespace pxr
{
	bool BloomFBO::Init(int windowWidth, int windowHeight, int mipChainLength)
	{
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
		glm::ivec2 mipIntSize(windowWidth, windowHeight);

		m_MipChain.reserve(mipChainLength);
		for (int i = 0; i < mipChainLength; i++)
		{
			TextureBuffer mip;
			mip.Create(mipIntSize.x, mipIntSize.y, TextureBufferType::HDR, TextureBufferFilterMode::Linear);
			m_MipChain.emplace_back(mip);

			// setup next mip
			mipSize *= 0.5f;
			mipIntSize /= 2;
		}

		// attach first mip texture to color attachment 0
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MipChain[0].GetID(), 0);

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
		for (auto& mip : m_MipChain)
			mip.Destroy();
		m_MipChain.clear();
		
		glDeleteFramebuffers(1, &m_FBO);
	}

	void BloomFBO::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void BloomFBO::Resize(int width, int height)
	{
		for (auto& mip : m_MipChain)
			mip.Resize(width, height);
	}
}