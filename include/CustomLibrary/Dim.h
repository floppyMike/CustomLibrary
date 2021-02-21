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

		template<arithmetic U>
		constexpr auto operator*(U) noexcept;

		template<arithmetic U>
		constexpr auto operator/(U) noexcept;

		constexpr auto operator==(const Dim &d) const noexcept -> bool { return w == d.w && h == d.h; }

		T w, h;
	};

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Dim<T>::operator*(U a) noexcept
	{
		return Dim(w * a, h * a);
	}

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Dim<T>::operator/(U a) noexcept
	{
		return Dim(w / a, h / a);
	}

} // namespace ctl::mth