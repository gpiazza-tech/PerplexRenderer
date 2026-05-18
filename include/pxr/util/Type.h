#pragma once

namespace pxr
{
	enum class Type
	{
		None = 0,
		Int,
		Float,
		Byte,
	};

	int SizeOf(Type type);
}