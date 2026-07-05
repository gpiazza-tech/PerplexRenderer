#pragma once

#include <pxr/util/Log.h>

#include <vector>
#include <exception>

#include <glm/fwd.hpp>

#include <algorithm>
#include <utility>

namespace pxr
{
	class out_of_range_exception : public std::exception {};

	template<typename T>
	void ResizeFlattenedArray(std::vector<T>& array, size_t& width, size_t& height, int left, int right, int up, int down, const T& defaultVal = T())
	{
		size_t newWidth = width + left + right;
		size_t newHeight = height + up + down;

		std::vector<T> newArray{ newWidth * newHeight, defaultVal };

		for (size_t y{}; y < height; ++y)
		{
			for (size_t x{}; x < width; ++x)
			{
				size_t newX = left + x;
				size_t newY = up + y;

				if (newX < 0 || newX >= newWidth || newY < 0 || newY >= newHeight)
					continue;

				newArray[newY * newWidth + newX] = array[y * width + x];
			}
		}

		array = std::move(newArray);
		width = newWidth;
		height = newHeight;
	}

	class ImageBuffer
	{
	public:
		ImageBuffer()
			: m_Width(0), m_Height(0), m_Buffer(0)
		{ }

		ImageBuffer(size_t width, size_t height)
			: m_Width(width), m_Height(height), m_Buffer(width * height)
		{ }

		glm::i8vec4& At(size_t x, size_t y)
		{
			if (x >= m_Width)
				throw std::exception("X value is out of range!");
			else if (y >= m_Height)
				throw std::exception("Y value is out of range!");
			return m_Buffer[y * m_Width + x];
		}

		const glm::i8vec4& At(size_t x, size_t y) const
		{
			if (x >= m_Width)
				throw std::exception("X value is out of range!");
			else if (y >= m_Height)
				throw std::exception("Y value is out of range!");
			return m_Buffer[y * m_Width + x];
		}

		size_t GetWidth() const { return m_Width; }
		size_t GetHeight() const { return m_Height; }

		glm::i8vec4* Data() { return m_Buffer.data(); }

		void Fill(const glm::i8vec4* data)
		{
			for (size_t i{}; i < m_Buffer.size(); ++i)
				m_Buffer[i] = data[i];
		}

		void Resize(int left, int right, int up, int down, const glm::i8vec4& defaultVal = glm::i8vec4())
		{
			ResizeFlattenedArray(m_Buffer, m_Width, m_Height, left, right, up, down, defaultVal);
		}
	private:
		size_t m_Width, m_Height;
		std::vector<glm::i8vec4> m_Buffer;
	};
}