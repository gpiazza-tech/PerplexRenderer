#include <pch.h>;
#include "Pixelator.h"

#include <rendering/Shader.h>
#include <util/RenderCommands.h>

namespace pxr
{
	void Pixelator::Init(int outWidth, int outHeight)
	{
		m_Width = outWidth;
		m_Height = outHeight;

		m_PixelatorShader = new Shader("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\postprocessing\\PixelateFragment.glsl");
		m_FBO.Create(outWidth, outHeight);
	}

	void Pixelator::Destroy()
	{
		delete m_PixelatorShader;
		m_FBO.Destroy();
	}

	void Pixelator::RenderPixelator(uint32_t srcTexture)
	{
		m_FBO.Bind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		m_PixelatorShader->Use();
		m_PixelatorShader->SetUniformInt("u_Texture", 1);

		RenderCommands::DrawScreen();

		m_PixelatorShader->EndUse();

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