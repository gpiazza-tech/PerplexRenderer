#include <pxr/pch.h>
#include <pxr/postprocessing/Pixelator.h>

#include <pxr/backends/Shader.h>
#include <pxr/backends/RenderCommands.h>

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
		glDisable(GL_BLEND);

		constexpr uint32_t textureUnit{ 0 };
		glBindTextureUnit(textureUnit, srcTexture);

		m_PixelatorShader.SetUniformInt("u_Texture", textureUnit);
		m_PixelatorShader.SetUniformFloat2("u_TargetResolution", targetResolution.x, targetResolution.y);

		m_PixelatorShader.Use(); 
		RenderCommands::DrawScreen();
		m_PixelatorShader.EndUse();

		glBindTextureUnit(textureUnit, 0);
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