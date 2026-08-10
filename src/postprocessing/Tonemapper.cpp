#include <pxr/pch.h>
#include <pxr/postprocessing/Tonemapper.h>

#include <pxr/backends/RenderCommands.h>
#include <pxr/backends/Shader.h>

// TODO: move glew functions
#include <GL/glew.h>

#include <cstdint>

namespace pxr
{
	void Tonemapper::Init(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		m_TonemapShader.Create("shaders\\ScreenVertex.glsl", "shaders\\postprocessing\\TonemapFragment.glsl");
		m_FBO = new Framebuffer(width, height);
	}

	void Tonemapper::Destroy()
	{
		m_TonemapShader.Destroy();
		delete m_FBO;
	}

	void Tonemapper::RenderTonemap(uint32_t srcTexture)
	{
		m_FBO->Bind();
		
		constexpr uint32_t textureUnit{ 0 };
		glBindTextureUnit(textureUnit, srcTexture);

		m_TonemapShader.Use();
		m_TonemapShader.SetUniformInt("u_Texture", textureUnit);

		RenderCommands::DrawScreen();

		m_TonemapShader.EndUse();

		glBindTextureUnit(textureUnit, 0);
	}

	uint32_t Tonemapper::TonemappedTexture()
	{
		return m_FBO->GetTextureID();
	}

	void Tonemapper::Resize(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		m_FBO->Resize(width, height);
	}
}