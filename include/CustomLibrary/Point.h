#pragma once

#include "Traits.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Concept
	// -----------------------------------------------------------------------------

	template<typename T>
	concept simple_point = requires(const T p)
	{
		{
			p.x
		}
		->arithmetic;
		{
			p.y
		}
		->arithmetic;
	};

	// -----------------------------------------------------------------------------
	// Point
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	class Point
	{
	public:
		constexpr Point() = default;

		constexpr Point(T x, T y) noexcept
			: x(x)
			, y(y)
		{
		}

		template<arithmetic U>
		constexpr Point(U x, U y) noexcept
			: x(x)
			, y(y)
		{
		}

		template<arithmetic U>
		constexpr Point(const Point<U> &p) noexcept
			: Point(p.x, p.y)
		{
		}

		template<arithmetic U>
		constexpr auto operator=(const Point<U> &p) -> Point &
		{
			x = p.x;
			y = p.y;
		}

		template<typename U>
		operator Point<U>() const noexcept
		{
			return { static_cast<U>(x), static_cast<U>(y) };
		}

		constexpr void translate(Point delta) noexcept { *this += delta; }

		constexpr void pos(Point p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		template<arithmetic U>
		constexpr auto operator+=(Point<U>) noexcept -> Point &;
		template<arithmetic U>
		constexpr auto operator-=(Point<U>) noexcept -> Point &;

		template<arithmetic U>
		constexpr auto operator+(Point<U>) const noexcept;
		template<arithmetic U>
		constexpr auto operator-(Point<U>) const noexcept;

		template<arithmetic U>
		constexpr auto operator*(U) const noexcept;

		template<arithmetic U>
		constexpr auto operator/(U) const noexcept;

		constexpr auto operator==(Point p) const noexcept;

		T x, y;
	};

	//----------------------------------------------
	// Implementation
	//----------------------------------------------

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Point<T>::operator+=(Point<U> p) noexcept -> Point &
	{
		x += p.x;
		y += p.y;

		return *this;
	}

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Point<T>::operator-=(Point<U> p) noexcept -> Point &
	{
		x -= p.x;
		y -= p.y;

		return *this;
	}

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Point<T>::operator+(Point<U> p) const noexcept
	{
		return Point<std::common_type_t<T, U>>(x + p.x, y + p.y);
	}

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Point<T>::operator-(Point<U> p) const noexcept
	{
		return Point<std::common_type_t<T, U>>(x - p.x, y - p.y);
	}

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Point<T>::operator*(U a) const noexcept
	{
		return Point(x * a, y * a);
	}

	template<arithmetic T>
	template<arithmetic U>
	inline constexpr auto Point<T>::operator/(U a) const noexcept
	{
		return Point(x / a, y / a);
	}

	template<arithmetic T>
	inline constexpr auto Point<T>::operator==(Point p) const noexcept
	{
		return x == p.x && y == p.y;
	}

} // namespace ctl::mth
