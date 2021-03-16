#pragma once

#include "Point.h"
#include "Traits.h"

#include <vector>

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

	/**
	 * @brief Generate a circle
	 *
	 * @param s circle shape
	 * @return a array of point outlining the circle
	 */
	auto generate_circle(uint32_t r) noexcept -> std::vector<mth::Point<int>>
	{
		std::vector<mth::Point<int>> cache;

		const auto d = r * 2;

		mth::Point<int> p(r - 1, 0);
		mth::Point<int> tp(1, 1);

		int err = tp.x - d;

		while (p.x >= p.y)
		{
			cache.insert(cache.end(),
						 { mth::Point<int>{ +p.x, +p.y },
						   { -p.x, +p.y },
						   { +p.x, -p.y },
						   { -p.x, -p.y },
						   { +p.y, +p.x },
						   { -p.y, +p.x },
						   { +p.y, -p.x },
						   { -p.y, -p.x } });

			if (err <= 0)
			{
				++p.y;
				err += tp.y;
				tp.y += 2;
			}
			else
			{
				--p.x;
				tp.x += 2;
				err += tp.x - d;
			}
		}

		return cache;
	}
} // namespace ctl::mth