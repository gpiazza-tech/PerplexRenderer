#include <pch.h>
#include "Tonemapper.h"

#include <util/RenderCommands.h>
#include <rendering/Shader.h>

#include <GL/glew.h>

#include <cstdint>

namespace pxr
{
	void Tonemapper::Init(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		m_TonemapShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\postprocessing\\TonemapFragment.glsl");
		m_FBO.Create(width, height);
	}

	void Tonemapper::Destroy()
	{
		m_TonemapShader.Destroy();
		m_FBO.Destroy();
	}

	void Tonemapper::RenderTonemap(uint32_t srcTexture)
	{
		m_FBO.Bind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		m_TonemapShader.Use();
		m_TonemapShader.SetUniformInt("u_Texture", 1);

		RenderCommands::DrawScreen();

		m_TonemapShader.EndUse();

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t Tonemapper::TonemappedTexture()
	{
		return m_FBO.GetTextureID();
	}

	void Tonemapper::Resize(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		m_FBO.Resize(width, height);
	}
}