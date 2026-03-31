#include <pch.h>;
#include "Pixelator.h"

#include <rendering/Shader.h>
#include <util/RenderCommands.h>

#include <fwd.hpp>
#include <GL/glew.h>

#include <cstdint>

namespace pxr
{
	void Pixelator::Init(int outWidth, int outHeight)
	{
		m_Width = outWidth;
		m_Height = outHeight;

		m_PixelatorShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\postprocessing\\PixelateFragment.glsl");
		m_FBO.Create(outWidth, outHeight);
	}

	void Pixelator::Destroy()
	{
		m_PixelatorShader.Destroy();
		m_FBO.Destroy();
	}

	void Pixelator::RenderPixelator(uint32_t srcTexture, const glm::vec2& targetResolution)
	{
		m_FBO.Bind();

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
		return m_FBO.GetTextureID();
	}

	void Pixelator::Resize(int outWidth, int outHeight)
	{
		m_Width = outWidth;
		m_Height = outHeight;

		m_FBO.Resize(outWidth, outHeight);
	}
}