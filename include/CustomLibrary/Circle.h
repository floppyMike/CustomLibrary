#pragma once

#include "Point.h"
#include "Traits.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Concept
	// -----------------------------------------------------------------------------

	template<typename T>
	concept simple_circle = simple_point<T> &&requires(const T t)
	{
		{
			t.r
		}
		->arithmetic;
	};

	// -----------------------------------------------------------------------------
	// Circle
	// -----------------------------------------------------------------------------

	template<arithmetic T1, arithmetic T2>
	class Circle
	{
	public:
		constexpr Circle() noexcept				  = default;
		constexpr Circle(const Circle &) noexcept = default;

		constexpr auto operator=(const Circle &c) noexcept -> Circle & = default;

		constexpr Circle(const T1 &x1, const T1 &y1, const T2 &r) noexcept
			: x(x1)
			, y(y1)
			, r(r)
		{
		}

		template<simple_point Point>
		constexpr Circle(const Point &p, const T2 &r) noexcept
			: Circle(p.x, p.y, r)
		{
		}

		constexpr auto pos() const noexcept { return mth::Point<T1>(x, y); }

		template<simple_point Point>
		constexpr void pos(const Point &p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr auto dim() const noexcept { return r; }
		constexpr void dim(T2 rad) noexcept { r = rad; }

		template<simple_point Point>
		constexpr void translate(const Point &delta) noexcept
		{
			x += delta.x;
			y += delta.y;
		}

		constexpr void resize(T2 delta) noexcept { r += delta; }

		T1 x, y;
		T2 r;
	};
} // namespace ctl::mth