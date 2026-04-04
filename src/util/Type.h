#pragma once

#include <iostream>

namespace pxr
{
	enum class Type
	{
		None = 0,
		Int,
		Float,
		Byte,
	};

	int SizeOf(Type type)
	{
		switch (type)
		{
		case Type::Int:
			return 4;
		case Type::Float:
			return 4;
		case Type::Byte:
			return 1;
		default:
			std::cout << "pxr::Type not supported!" << std::endl;
			return 0;
		}
	}
}