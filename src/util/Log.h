#pragma once

#define PXR_ENABLE_LOGGING
#ifdef PXR_ENABLE_LOGGING

#include <spdlog/spdlog.h>
#include <intrin.h>

#define PXR_INFO(...) spdlog::info(__VA_ARGS__)
#define PXR_WARN(...) spdlog::warn(__VA_ARGS__)
#define PXR_ERROR(...) spdlog::error(__VA_ARGS__)

#define PXR_ASSERT(x, ...) { if(!(x)) { PXR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#else

#define PXR_INFO(...)
#define PXR_WARN(...)
#define PXR_ERROR(...)

#define PXR_ASSERT(x, ...)

#endif