#include <pch.h>
#include "Tonemapper.h"

#include "Renderer/RenderCommands.h"
#include "Renderer/Shader.h"
#include "Util.h"

void Tonemapper::Init(int width, int height)
{
	m_Width = width;
	m_Height = height;

	// Framebuffer
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// Colorbuffer texture
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach colorbuffer to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

	// check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: Framebuffer is not complete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Shader
	m_TonemapShader = new Shader("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\TonemapFragment.glsl");
}

void Tonemapper::Destroy()
{
	glDeleteTextures(1, &m_Texture);

	delete m_TonemapShader;
}

void Tonemapper::RenderTonemap(uint32_t srcTexture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	m_TonemapShader->Use();
	m_TonemapShader->SetUniformInt("u_Texture", 1);

	RenderCommands::DrawScreen();

	m_TonemapShader->EndUse();

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t Tonemapper::TonemappedTexture()
{
	return m_Texture;
}