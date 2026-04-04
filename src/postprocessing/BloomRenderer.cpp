#include <pch.h>
#include "BloomRenderer.h"

#include <backends/Shader.h>
#include <backends/Framebuffer.h>
#include <backends/TextureBuffer.h>
#include <backends/RenderCommands.h>

// TODO: move glew functions
#include <GL/glew.h>

#include <fwd.hpp>
#include <cstdint>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace pxr
{
	bool BloomRenderer::Init(uint32_t windowWidth, uint32_t windowHeight)
	{
		m_SrcViewportSize = glm::ivec2(windowWidth, windowHeight);
		m_SrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

		// Framebuffer
		const uint32_t numBloomMips = 6;
		bool status = m_FBO.Init(windowWidth, windowHeight, numBloomMips);
		if (!status)
		{
			std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
			return false;
		}

		m_PrefilterFBO.Create(windowWidth, windowHeight, true);

		// Shaders
		m_PrefilterShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\postprocessing\\PrefilterFragment.glsl");
		m_DownsampleShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\postprocessing\\DownsampleFragment.glsl");
		m_UpsampleShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\postprocessing\\UpsampleFragment.glsl");
		m_ScreenShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\ScreenFragment.glsl");

		m_PrefilterShader.Use();
		m_PrefilterShader.SetUniformInt("u_Texture", 0);
		m_PrefilterShader.EndUse();

		m_DownsampleShader.Use();
		m_DownsampleShader.SetUniformInt("u_Texture", 0);
		m_DownsampleShader.EndUse();

		m_UpsampleShader.Use();
		m_UpsampleShader.SetUniformInt("u_Texture", 0);
		m_UpsampleShader.EndUse();

		m_ScreenShader.Use();
		m_ScreenShader.SetUniformInt("u_Texture", 0);
		m_ScreenShader.EndUse();

		return true;
	}

	void BloomRenderer::Destroy()
	{
		m_FBO.Destroy();
		m_PrefilterFBO.Destroy();

		m_PrefilterShader.Destroy();
		m_DownsampleShader.Destroy();
		m_UpsampleShader.Destroy();
		m_ScreenShader.Destroy();
	}

	void BloomRenderer::RenderBloomTexture(uint32_t srcTexture, float threshold, float filterRadius)
	{
		Prefilter(srcTexture, threshold);
		RenderDownsamples(m_PrefilterFBO.GetTextureID());
		RenderUpsamples(filterRadius);
		Combine(srcTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_SrcViewportSize.x, m_SrcViewportSize.y);
	}

	uint32_t BloomRenderer::BloomTexture()
	{
		return m_FBO.MipChain()[0].GetID();
	}

	void BloomRenderer::Prefilter(uint32_t srcTexture, float threshold)
	{
		glDisable(GL_BLEND);
		glViewport(0, 0, m_SrcViewportSize.x, m_SrcViewportSize.y);

		// input texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		// output texture
		m_PrefilterFBO.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_PrefilterShader.Use();
		m_PrefilterShader.SetUniformFloat("u_Threshold", threshold);
		RenderCommands::DrawScreen();
		m_PrefilterShader.EndUse();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void BloomRenderer::RenderDownsamples(uint32_t srcTexture)
	{
		glDisable(GL_BLEND);

		const std::vector<TextureBuffer>& mipChain = m_FBO.MipChain();

		m_DownsampleShader.Use();
		m_DownsampleShader.SetUniformFloat2("u_Resolution", m_SrcViewportSizeFloat.x, m_SrcViewportSizeFloat.y);

		// Bind srcTexture as the initial texture input
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		m_FBO.Bind();

		for (size_t i = 0; i < m_FBO.MipChain().size(); i++)
		{
			const TextureBuffer& mip = mipChain[i];
			glViewport(0, 0, (GLsizei)mip.GetWidth(), (GLsizei)mip.GetHeight());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.GetID(), 0);

			RenderCommands::DrawScreen();

			// Setup next iteration
			m_DownsampleShader.SetUniformFloat2("u_Resolution", (float)mip.GetWidth(), (float)mip.GetHeight());
			glBindTexture(GL_TEXTURE_2D, mip.GetID());
		}

		m_DownsampleShader.EndUse();
	}

	void BloomRenderer::RenderUpsamples(float filterRadius)
	{
		const std::vector<TextureBuffer>& mipChain = m_FBO.MipChain();

		m_UpsampleShader.Use();
		m_UpsampleShader.SetUniformFloat("u_FilterRadius", filterRadius);

		// Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		m_FBO.Bind();

		for (size_t i = mipChain.size() - 1; i > 0; i--)
		{
			const TextureBuffer& mip = mipChain[i];
			const TextureBuffer& nextMip = mipChain[i - 1];

			// Set read texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mip.GetID());

			// Set write texture
			glViewport(0, 0, (GLsizei)nextMip.GetWidth(), (GLsizei)nextMip.GetHeight());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.GetID(), 0);

			RenderCommands::DrawScreen();
		}

		glDisable(GL_BLEND);
		m_UpsampleShader.EndUse();
	}

	void BloomRenderer::Combine(uint32_t srcTexture)
	{
		glBindTextureUnit(0, srcTexture);
		m_FBO.Bind();

		m_ScreenShader.Use();
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		RenderCommands::DrawScreen();

		glDisable(GL_BLEND);
		m_ScreenShader.EndUse();
	}

	void BloomRenderer::Resize(uint32_t width, uint32_t height)
	{
		m_SrcViewportSize.x = width;
		m_SrcViewportSize.y = height;

		m_SrcViewportSizeFloat.x = (float)width;
		m_SrcViewportSizeFloat.y = (float)height;

		m_PrefilterFBO.Resize(width, height);
		m_FBO.Resize(width, height);
	}
}