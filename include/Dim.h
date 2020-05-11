#pragma once

#include "Tags.h"
#include <SDL.h>

namespace ctl::mth
{
	template<typename T>
	class Dim
	{
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	public:
		using num_t1 = T;
		using tag = tag::isDim;

		constexpr Dim() = default;
		constexpr Dim(const Dim&) = default;

		constexpr Dim& operator=(const Dim&) = default;

		/**
		* @summary construct from width and height
		*/
		constexpr Dim(const T& pw, const T& ph) noexcept
			: w(pw), h(ph)
		{
		}

		T w, h;
	};
}