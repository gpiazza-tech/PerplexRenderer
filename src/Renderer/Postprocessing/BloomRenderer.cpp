#include <pch.h>
#include "BloomRenderer.h"

#include "Renderer/Shader.h"
#include "Renderer/RenderCommands.h"

namespace fs = std::filesystem;

bool BloomRenderer::Init(uint32_t windowWidth, uint32_t windowHeight)
{
	m_SrcViewportSize = glm::ivec2(windowWidth, windowHeight);
	m_SrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

	// Framebuffer
	const uint32_t numBloomMips = 5;
	bool status = m_FBO.Init(windowWidth, windowHeight, numBloomMips);
	if (!status)
	{
		std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
		return false;
	}

	// Shaders
	m_DownsampleShader = new Shader("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\DownsampleFragment.glsl");
	m_UpsampleShader = new Shader("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\UpsampleFragment.glsl");

	m_DownsampleShader->Use();
	m_DownsampleShader->SetUniformInt("u_Texture", 0);
	m_DownsampleShader->EndUse();

	m_UpsampleShader->Use();
	m_UpsampleShader->SetUniformInt("u_Texture", 0);
	m_UpsampleShader->EndUse();

	return true;
}

void BloomRenderer::Destroy()
{
	m_FBO.Destroy();

	delete m_DownsampleShader;
	delete m_UpsampleShader;
}

void BloomRenderer::RenderBloomTexture(uint32_t srcTexture, float filterRadius)
{
	m_FBO.Bind();

	this->RenderDownsamples(srcTexture);
	this->RenderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_SrcViewportSize.x, m_SrcViewportSize.y);
}

uint32_t BloomRenderer::BloomTexture()
{
	return m_FBO.MipChain()[0].Texture;
}

void BloomRenderer::RenderDownsamples(uint32_t srcTexture)
{
	glDisable(GL_BLEND);

	const std::vector<BloomMip>& mipChain = m_FBO.MipChain();

	m_DownsampleShader->Use();
	m_DownsampleShader->SetUniformFloat2("u_Resolution", m_SrcViewportSizeFloat.x, m_SrcViewportSizeFloat.y);

	// Bind srcTexture as the initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	for (size_t i = 0; i < m_FBO.MipChain().size(); i++)
	{
		const BloomMip& mip = mipChain[i];
		glViewport(0, 0, mip.Size.x, mip.Size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.Texture, 0);

		RenderCommands::DrawScreen();

		// Setup next iteration
		m_DownsampleShader->SetUniformFloat2("u_Resolution", mip.Size.x, mip.Size.y);
		glBindTexture(GL_TEXTURE_2D, mip.Texture);
	}

	m_DownsampleShader->EndUse();
}

void BloomRenderer::RenderUpsamples(float filterRadius)
{
	const std::vector<BloomMip>& mipChain = m_FBO.MipChain();

	m_UpsampleShader->Use();
	m_UpsampleShader->SetUniformFloat("u_FilterRadius", filterRadius);

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (size_t i = mipChain.size() - 1; i > 0; i--)
	{
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		// Set read texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.Texture);

		// Set write texture
		glViewport(0, 0, nextMip.Size.x, nextMip.Size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.Texture, 0);

		RenderCommands::DrawScreen();
	}

	glDisable(GL_BLEND);

	m_UpsampleShader->EndUse();
}