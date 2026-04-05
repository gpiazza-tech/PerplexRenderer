#include <pch.h>
#include "Type.h"

#include <iostream>

namespace pxr
{
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