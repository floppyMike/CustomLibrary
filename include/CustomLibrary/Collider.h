#pragma once

#include "BasicTypes.h"
#include "Traits.h"
#include <cmath>
#include <utility>

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Colliders for Point, Rectangle, Circle
	// -----------------------------------------------------------------------------

	template<mth::simple_point T1, mth::simple_point T2>
	constexpr auto collision(const T1 &d1, const T2 &d2) noexcept
	{
		return d1 == d2;
	}

	template<mth::simple_point T1, mth::simple_rect T2>
	constexpr auto collision(const T1 &d, const T2 &r) noexcept
	{
		return !(d.x < r.x || d.x > r.x + r.w || d.y < r.y || d.y > r.y + r.h);
	}

	template<mth::simple_point T1, mth::simple_circle T2>
	constexpr auto collision(const T1 &d, const T2 &c) noexcept
	{
		const auto dx = std::abs(d.x - c.x);
		const auto dy = std::abs(d.y - c.y);

		if (dx > c.r || dy > c.r) return false;

		return dx + dy <= c.r || std::pow(dx, 2) + std::pow(dy, 2) <= std::pow(c.r, 2);
	}

	template<mth::simple_rect T1, mth::simple_point T2>
	constexpr auto collision(const T1 &r, const T2 &d) noexcept
	{
		return collision(d, r);
	}

	template<mth::simple_rect T1, mth::simple_rect T2>
	constexpr auto collision(const T1 &r1, const T2 &r2) noexcept
	{
		return !(r1.y + r1.h <= r2.y || r1.y >= r2.y + r2.h || r1.x + r1.w <= r2.x || r1.x >= r2.x + r2.w);
	}

	template<mth::simple_rect T1, mth::simple_circle T2>
	constexpr auto collision(const T1 &r, const T2 &c) noexcept
	{
		const auto halfWidth = r.w / 2;
		const auto halfHight = r.h / 2;
		const auto distanceX = std::abs(r.x + halfWidth - c.x);
		const auto distanceY = std::abs(r.y + halfHight - c.y);

		if (distanceX > (halfWidth + c.r)) return false;
		if (distanceY > (halfHight + c.r)) return false;

		if (distanceX <= halfWidth) return true;
		if (distanceY <= halfHight) return true;

		return power2(distanceX - halfWidth) + power2(distanceY - halfHight) <= power2(c.r);
	}

	template<mth::simple_circle T1, mth::simple_point T2>
	constexpr auto collision(const T1 &c, const T2 &d) noexcept
	{
		return collision(d, c);
	}

	template<mth::simple_circle T1, mth::simple_rect T2>
	constexpr auto collision(const T1 &c, const T2 &r) noexcept
	{
		return collision(r, c);
	}

	template<mth::simple_circle T1, mth::simple_circle T2>
	constexpr auto collision(const T1 &c1, const T2 &c2) noexcept
	{
		return power2(c1.x - c2.x) + power2(c1.y - c2.y) < power2(c1.r + c2.r);
	}

} // namespace ctl::mth