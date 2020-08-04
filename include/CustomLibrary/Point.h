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
		constexpr Point()			   = default;
		constexpr Point(const Point &) = default;

		constexpr auto operator=(const Point &) -> Point & = default;

		constexpr Point(const T &x, const T &y)
			: x(x)
			, y(y)
		{
		}

		template<typename U>
		explicit operator Point<U>() const noexcept
		{
			return { static_cast<U>(x), static_cast<U>(y) };
		}

#ifdef SDL_h_
		operator SDL_Point() const noexcept;
		operator SDL_FPoint() const noexcept;

		const SDL_Point *point_ptr() const noexcept requires std::same_as<T, int>
		{
			return reinterpret_cast<const SDL_Point *const>(this);
		}
		SDL_Point *point_ptr() noexcept requires std::same_as<T, int> { return reinterpret_cast<SDL_Point *>(this); }
#endif // SDL_h_

		constexpr void translate(const Point<T> &delta) noexcept { *this += delta; }

		constexpr void pos(const Point<T> &p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr auto operator+=(const Point &p) noexcept -> auto &;
		constexpr auto operator-=(const Point &p) noexcept -> auto &;

		constexpr auto operator+(const Point &p) const noexcept;
		constexpr auto operator-(const Point &p) const noexcept;

		constexpr auto operator==(const Point &p) const noexcept;

		T x, y;
	};

	//----------------------------------------------
	// Implementation
	//----------------------------------------------

	// #ifdef SDL_h_
	// 	template<typename T>
	// 	inline Point<T>::operator SDL_Point() const noexcept
	// 	{
	// 		return { static_cast<int>(x), static_cast<int>(y) };
	// 	}

	// 	template<typename T>
	// 	inline Point<T>::operator SDL_FPoint() const noexcept
	// 	{
	// 		return { static_cast<float>(x), static_cast<float>(y) };
	// 	}
	// #endif // SDL_h_

	//--------------------------------------------------------

	template<arithmetic T>
	inline constexpr auto Point<T>::operator+=(const Point &p) noexcept -> auto &
	{
		x += p.x;
		y += p.y;

		return *this;
	}

	template<arithmetic T>
	inline constexpr auto Point<T>::operator-=(const Point &p) noexcept -> auto &
	{
		x -= p.x;
		y -= p.y;

		return *this;
	}

	template<arithmetic T>
	inline constexpr auto Point<T>::operator+(const Point &p) const noexcept
	{
		return Point(x + p.x, y + p.y);
	}

	template<arithmetic T>
	inline constexpr auto Point<T>::operator-(const Point &p) const noexcept
	{
		return Point(x - p.x, y - p.y);
	}

	template<arithmetic T>
	inline constexpr auto Point<T>::operator==(const Point &p) const noexcept
	{
		return x == p.x && y == p.y;
	}

} // namespace ctl::mth
