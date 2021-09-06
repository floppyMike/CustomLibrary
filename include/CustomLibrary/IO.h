#pragma once

#include <cstdio>

namespace ctl
{
	// -----------------------------------------------------------------------------
	// Formatters
	// -----------------------------------------------------------------------------

	template<typename T>
	T format(const T &arg)
	{
		return arg;
	}

	// -----------------------------------------------------------------------------
	// Print function
	// -----------------------------------------------------------------------------

	template<typename... T>
	void print(const char *fmt, const T &...param)
	{
		std::printf(fmt, format(param)...);
	}

	inline void print(const char *str)
	{
		std::puts(str);
	}

} // namespace ctl
