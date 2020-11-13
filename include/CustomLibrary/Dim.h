#pragma once

#include "Traits.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Concept
	// -----------------------------------------------------------------------------

	template<typename T>
	concept simple_dim = requires(const T d)
	{
		{
			d.w
		}
		->arithmetic;
		{
			d.h
		}
		->arithmetic;
	};

	// -----------------------------------------------------------------------------
	// Dim
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	class Dim
	{
	public:
		constexpr Dim()			   = default;
		constexpr Dim(const Dim &) = default;

		constexpr auto operator=(const Dim &) -> Dim & = default;

		/**
		 * @summary construct from width and height
		 */
		constexpr Dim(const T &pw, const T &ph) noexcept
			: w(pw)
			, h(ph)
		{
		}

		/**
		 * @brief Get the area of the dimensions
		 * @return Area
		 */
		constexpr auto area() const noexcept -> T { return w * h; }

		T w, h;
	};
} // namespace ctl::mth