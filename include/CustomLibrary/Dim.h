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
		constexpr Dim() = default;

		constexpr Dim(T x, T y) noexcept
			: w(x)
			, h(y)
		{
		}

		template<arithmetic U>
		constexpr Dim(U x, U y) noexcept
			: w(x)
			, h(y)
		{
		}

		template<arithmetic U>
		constexpr Dim(const Dim<U> &p) noexcept
			: Dim(p.w, p.h)
		{
		}

		template<arithmetic U>
		constexpr auto operator=(const Dim<U> &p) -> Dim &
		{
			w = p.x;
			h = p.y;

			return *this;
		}

		template<typename U>
		operator Dim<U>() const noexcept
		{
			return { static_cast<U>(w), static_cast<U>(h) };
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