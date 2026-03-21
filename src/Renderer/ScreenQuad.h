#pragma once

class ScreenQuad
{
public:
	ScreenQuad();
	~ScreenQuad();

	void Draw();
private:
	uint32_t m_Vao;
};