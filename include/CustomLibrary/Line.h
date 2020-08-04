#pragma once

#include "Point.h"
#include "Traits.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Concept
	// -----------------------------------------------------------------------------

	template<typename T>
	concept simple_line = requires(const T l)
	{
		{
			l.x1
		}
		->arithmetic;
		{
			l.x2
		}
		->arithmetic;
		{
			l.y1
		}
		->arithmetic;
		{
			l.y2
		}
		->arithmetic;
	};

	// -----------------------------------------------------------------------------
	// Line
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	class Line
	{
	public:
		constexpr Line() noexcept			  = default;
		constexpr Line(const Line &) noexcept = default;

		constexpr auto operator=(const Line &l) noexcept -> Line & = default;

		constexpr Line(const T &x1, const T &y1, const T &x2, const T &y2) noexcept
			: x1(x1)
			, y1(y1)
			, x2(x2)
			, y2(y2)
		{
		}

		constexpr Line(const mth::Point<T> &p1, const mth::Point<T> &p2) noexcept
			: Line(p1.x, p1.y, p2.x, p2.y)
		{
		}

		template<typename U>
		constexpr explicit operator Line<U>() noexcept
		{
			return { static_cast<U>(x1), static_cast<U>(y1), static_cast<U>(x2), static_cast<U>(y2) };
		}

#ifdef SDL_h_
		const SDL_Point *point_ptr() const noexcept requires std::same_as<T, int>
		{
			return reinterpret_cast<const SDL_Point *const>(this);
		}
		SDL_Point *point_ptr() noexcept requires std::same_as<T, int> { return reinterpret_cast<SDL_Point *>(this); }
#endif // SDL_h_

		constexpr auto pos1() const noexcept -> mth::Point<T> { return { x1, y1 }; }
		constexpr void pos1(const mth::Point<T> &p) noexcept
		{
			x1 = p.x;
			y1 = p.y;
		}
		constexpr void pos1(T x, T y) noexcept
		{
			x1 = x;
			y1 = y;
		}

		constexpr auto pos2() const noexcept -> mth::Point<T> { return { x2, y2 }; }
		constexpr void pos2(const mth::Point<T> &p) noexcept
		{
			x2 = p.x;
			y2 = p.y;
		}
		constexpr void pos2(T x, T y) noexcept
		{
			x2 = x;
			y2 = y;
		}

		constexpr void translate(const mth::Point<T> &delta1, const mth::Point<T> &delta2) noexcept
		{
			x1 += delta1.x;
			y1 += delta1.y;
			x2 += delta2.x;
			y2 += delta2.y;
		}

		constexpr void pos(const mth::Point<T> &p1, const mth::Point<T> &p2) noexcept
		{
			x1 = p1.x;
			y1 = p1.y;
			x2 = p2.x;
			y2 = p2.y;
		}

		T x1, y1, x2, y2;
	};

} // namespace ctl::mth
