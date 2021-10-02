#pragma once

#include <vector>

#include "Traits.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Point
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	struct Point
	{
		T x, y;

		template<typename U>
		constexpr explicit operator Point<U>() const noexcept
		{
			return { static_cast<U>(x), static_cast<U>(y) };
		}

		constexpr void translate(T dx, T dy) noexcept
		{
			x += dx;
			y += dy;
		}

		constexpr auto dot_product(Point p)
		{
			return x * p.x + y * p.y;
		}

		constexpr auto operator==(Point p) const noexcept
		{
			return x == p.x && y == p.y;
		}

		constexpr auto operator+(Point p) const noexcept
		{
			return Point{ x + p.x, y + p.y };
		}
		constexpr auto operator+=(Point p) noexcept
		{
			return *this = *this + p;
		}

		constexpr auto operator-(Point p) const noexcept
		{
			return Point{ x - p.x, y - p.y };
		}
		constexpr auto operator-=(Point p) noexcept
		{
			return *this = *this - p;
		}

		constexpr auto operator*(T s) const noexcept
		{
			return Point{ x * s, y * s };
		}
		constexpr auto operator*=(T s) noexcept
		{
			return *this = *this * s;
		}

		constexpr auto operator/(T s) const noexcept
		{
			return Point{ x / s, y / s };
		}
		constexpr auto operator/=(T s) noexcept
		{
			return *this = *this / s;
		}
	};

	// -----------------------------------------------------------------------------
	// Dimension
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	struct Dim
	{
		T w, h;

		template<typename U>
		constexpr explicit operator Dim<U>() const noexcept
		{
			return { static_cast<U>(w), static_cast<U>(h) };
		}

		/**
		 * @brief Get the area of the dimensions
		 * @return Area
		 */
		constexpr auto area() const noexcept -> T
		{
			return w * h;
		}

		constexpr auto operator*(T s) const noexcept
		{
			return Dim{ w * s, h * s };
		}

		constexpr auto operator/(T s) const noexcept
		{
			return Dim{ w / s, h / s };
		}

		constexpr auto operator==(const Dim &d) const noexcept -> bool
		{
			return w == d.w && h == d.h;
		}
	};

	// -----------------------------------------------------------------------------
	// Rectangle
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	struct Rect
	{
		T x, y, w, h;

		template<typename U>
		constexpr explicit operator Rect<U>() noexcept
		{
			return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(w), static_cast<U>(h) };
		}

		constexpr auto pos() const -> mth::Point<T>
		{
			return { x, y };
		}
		constexpr void pos(mth::Point<T> p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr auto dim() const noexcept -> mth::Dim<T>
		{
			return { w, h };
		}
		constexpr void dim(mth::Dim<T> d) noexcept
		{
			w = d.w;
			h = d.h;
		}

		constexpr void translate(T dx, T dy) noexcept
		{
			x += dx;
			y += dy;
		}

		constexpr void resize(T dw, T dh) noexcept
		{
			w += dw;
			h += dh;
		}
	};

	// -----------------------------------------------------------------------------
	// Circle
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	struct Circle
	{
		template<typename U>
		constexpr explicit operator Circle<U>() noexcept
		{
			return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(r) };
		}

		constexpr auto pos() const noexcept
		{
			return mth::Point<T>{ x, y };
		}
		constexpr void pos(mth::Point<T> p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr void translate(int dx, int dy) noexcept
		{
			x += dx;
			y += dy;
		}

		constexpr void resize(T dr) noexcept
		{
			r += dr;
		}

		T x, y, r;
	};

	/**
	 * @brief Generate a filled circle. Optimal for small circles.
	 *
	 * @param r circle radius
	 * @return points of a filled circle
	 */
	inline auto gen_circle_filled(const int r) noexcept -> std::vector<mth::Point<int>>
	{
		std::vector<mth::Point<int>> circle;

		for (auto x = -r; x <= +r; ++x)
			for (auto y = -r; y <= +r; ++y)
				if (y * y + x * x < r * r)
					circle.push_back({ x, y });

		return circle;
	}

	inline auto gen_diamond(const int r) noexcept -> std::vector<mth::Point<int>>
	{
		std::vector<mth::Point<int>> d;

		for (auto x = -r; x <= +r; ++x)
			for (auto y = -r; y <= +r; ++y)
				if (std::abs(y) + std::abs(x) == r)
					d.push_back({ x, y });

		return d;
	}

	/**
	 * @brief Generate a circle shape. Optimal for larger circles.
	 *
	 * @param r circle radius
	 * @return a array of points outlining the circle
	 */
	inline auto midpoint_circle(int r) noexcept -> std::vector<mth::Point<int>>
	{
		std::vector<mth::Point<int>> cache;

		const auto d = r * 2;

		mth::Point<int> p{ r - 1, 0 };
		mth::Point<int> tp{ 1, 1 };

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

	// -----------------------------------------------------------------------------
	// Line
	// -----------------------------------------------------------------------------

	template<arithmetic T>
	struct Line
	{
		static auto from(mth::Point<T> p1, mth::Point<T> p2) -> Line
		{
			return { p1.x, p1.y, p2.x, p2.y };
		}

		template<typename U>
		constexpr explicit operator Line<U>() noexcept
		{
			return { static_cast<U>(x1), static_cast<U>(y1), static_cast<U>(x2), static_cast<U>(y2) };
		}

		constexpr auto pos1() const noexcept -> mth::Point<T>
		{
			return { x1, y1 };
		}
		constexpr void pos1(mth::Point<T> p) noexcept
		{
			x1 = p.x;
			y1 = p.y;
		}

		constexpr auto pos2() const noexcept -> mth::Point<T>
		{
			return { x2, y2 };
		}
		constexpr void pos2(mth::Point<T> p) noexcept
		{
			x2 = p.x;
			y2 = p.y;
		}

		constexpr auto abs_rect() const -> mth::Rect<T>
		{
			const auto min_x = std::min(x1, x2);
			const auto min_y = std::min(y1, y2);

			const auto diff_x = std::abs(x1 - x2);
			const auto diff_y = std::abs(y1 - y2);

			return { min_x, min_y, diff_x, diff_y };
		}

		/**
		 * @brief Move the whole line by a difference in length
		 *
		 * @param delta1 Difference of point 1
		 * @param delta2 Difference of point 2
		 */
		constexpr void translate(int dx1, int dy1, int dx2, int dy2) noexcept
		{
			x1 += dx1;
			y1 += dy1;
			x2 += dx2;
			y2 += dy2;
		}

		constexpr auto delta_x() const noexcept -> T
		{
			return x2 - x1;
		}
		constexpr auto delta_y() const noexcept -> T
		{
			return y2 - y1;
		}

		T x1, y1, x2, y2;
	};

} // namespace ctl::mth
