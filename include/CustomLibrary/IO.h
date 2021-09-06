#pragma once

#include <stdio.h>

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
		printf(fmt, format(param)...);
	}

} // namespace ctl
