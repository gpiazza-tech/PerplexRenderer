#pragma once

namespace pxr
{
	class Shader
	{
	public:
		Shader() = default;
		Shader(const std::filesystem::path& vertexSrc, const std::filesystem::path& fragmentSrc);
		~Shader();

		void Use();
		void EndUse();

		void SetUniformInt(const char* name, int value);
		void SetUniformInt2(const char* name, int val1, int val2);
		void SetUniformInt3(const char* name, int val1, int val2, int val3);
		void SetUniformInt4(const char* name, int val1, int val2, int val3, int val4);
		void SetUniformIntArray(const char* name, int count, int* array);
		void SetUniformFloat(const char* name, float val);
		void SetUniformFloat(const char* name, float* ptr);
		void SetUniformFloat2(const char* name, float val1, float val2);
		void SetUniformFloat3(const char* name, float val1, float val2, float val3);
		void SetUniformFloat4(const char* name, float val1, float val2, float val3, float val4);
		void SetUniformMat4(const char* name, float* val);
	private:
		uint32_t m_RendererID;
	};
}