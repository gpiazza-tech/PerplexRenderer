#include <pch.h>;
#include "Pixelator.h"

#include <backends/Shader.h>
#include <backends/RenderCommands.h>

#include <glm/fwd.hpp>

// TODO: move glew functions
#include <GL/glew.h>

#include <cstdint>

namespace pxr
{
	void Pixelator::Init(int outWidth, int outHeight)
	{
		m_Width = outWidth;
		m_Height = outHeight;

		m_PixelatorShader.Create("shaders\\ScreenVertex.glsl", "shaders\\postprocessing\\PixelateFragment.glsl");
		m_FBO = new Framebuffer(outWidth, outHeight);
	}

	void Pixelator::Destroy()
	{
		m_PixelatorShader.Destroy();
		delete m_FBO;
	}

	void Pixelator::RenderPixelator(uint32_t srcTexture, const glm::vec2& targetResolution)
	{
		m_FBO->Bind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		m_PixelatorShader.Use();
		m_PixelatorShader.SetUniformInt("u_Texture", 1);
		m_PixelatorShader.SetUniformFloat2("u_TargetResolution", targetResolution.x, targetResolution.y);

		RenderCommands::DrawScreen();

		m_PixelatorShader.EndUse();

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t Pixelator::PixelatedTexture()
	{
		return m_FBO->GetTextureID();
	}

	void Pixelator::Resize(int outWidth, int outHeight)
	{
		m_Width = outWidth;
		m_Height = outHeight;

		m_FBO->Resize(outWidth, outHeight);
	}
}