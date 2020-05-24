#pragma once

#include "Tags.h"

namespace ctl::mth
{
	template<typename T>
	class Point
	{
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	public:
		using num_t1 = T;
		using tag	 = tag::isPoint;

		constexpr Point()			   = default;
		constexpr Point(const Point &) = default;

		constexpr Point &operator=(const Point &) = default;

		/**
		 * Copy constructs from coords.
		 * @param [in] x x-coord
		 * @param [in] y y-coord
		 */
		constexpr Point(const T &x, const T &y)
			: x(x)
			, y(y)
		{
		}

		template<typename U>
		operator Point<U>() const noexcept;

#ifdef SDL_h_
		operator SDL_Point() const noexcept;
		operator SDL_FPoint() const noexcept;

		const SDL_Point *point_ptr() const noexcept
		{
			static_assert(std::is_same_v<T, int>, "Type must be int.");
			return reinterpret_cast<const SDL_Point *const>(this);
		}
		SDL_Point *point_ptr() noexcept
		{
			static_assert(std::is_same_v<T, int>, "Type must be int.");
			return reinterpret_cast<SDL_Point *>(this);
		}
#endif // SDL_h_

		constexpr void translate(const Point<T> &delta) noexcept { *this += delta; }

		constexpr void pos(const Point<T> &p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		/**
		 * @summary assignment operators
		 */
		constexpr auto &operator+=(const Point &p) noexcept;
		constexpr auto &operator-=(const Point &p) noexcept;

		/**
		 * @summary arithmitic operators
		 */
		constexpr auto operator+(const Point &p) const noexcept;
		constexpr auto operator-(const Point &p) const noexcept;

		/**
		 * @summary relational operators
		 */
		constexpr auto operator==(const Point &p) const noexcept;

		T x, y;
	};

	//----------------------------------------------
	// Implementation
	//----------------------------------------------

#ifdef SDL_h_
	template<typename T>
	inline Point<T>::operator SDL_Point() const noexcept
	{
		return { static_cast<int>(x), static_cast<int>(y) };
	}

	template<typename T>
	inline Point<T>::operator SDL_FPoint() const noexcept
	{
		return { static_cast<float>(x), static_cast<float>(y) };
	}
#endif // SDL_h_

	//--------------------------------------------------------

	template<typename T>
	inline constexpr auto &Point<T>::operator+=(const Point &p) noexcept
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	template<typename T>
	inline constexpr auto &Point<T>::operator-=(const Point &p) noexcept
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}

	template<typename T>
	inline constexpr auto Point<T>::operator+(const Point &p) const noexcept
	{
		return Point(x + p.x, y + p.y);
	}

	template<typename T>
	inline constexpr auto Point<T>::operator-(const Point &p) const noexcept
	{
		return Point(x - p.x, y - p.y);
	}

	template<typename T>
	inline constexpr auto Point<T>::operator==(const Point &p) const noexcept
	{
		return x == p.x && y == p.y;
	}

	template<typename T>
	template<typename U>
	inline Point<T>::operator Point<U>() const noexcept
	{
		return { static_cast<U>(x), static_cast<U>(y) };
	}
} // namespace ctl::mth

