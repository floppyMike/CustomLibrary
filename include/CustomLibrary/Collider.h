#pragma once

#include "BasicTypes.h"
#include "Traits.h"
#include <cmath>
#include <utility>

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Collision Detectors
	// -----------------------------------------------------------------------------

	/**
	 * @brief Checks for collision between 2 points
	 *
	 * @tparam _T1 Point type
	 * @tparam _T2 Point type
	 * @param d1 point 1
	 * @param d2 point 2
	 * @return if collision occured
	 */
	template<mth::simple_point _T1, mth::simple_point _T2>
	constexpr auto collision(const _T1 &d1, const _T2 &d2) noexcept
	{
		return d1 == d2;
	}

	/**
	 * @brief Check for collision between a point and a rectange
	 *
	 * @tparam _T1 Point type
	 * @tparam _T2 Rectangle type
	 * @param d Point
	 * @param r Rectange
	 * @return if collision occured
	 */
	template<mth::simple_point _T1, mth::simple_rect _T2>
	constexpr auto collision(const _T1 &d, const _T2 &r) noexcept
	{
		return !(d.x < r.x || d.x > r.x + r.w || d.y < r.y || d.y > r.y + r.h);
	}

	/**
	 * @brief Check for collision between a point and a circle
	 *
	 * @tparam _T1 Point type
	 * @tparam _T2 Circle type
	 * @param d Point
	 * @param c Circle
	 * @return if collision occured
	 */
	template<mth::simple_point _T1, mth::simple_circle _T2>
	constexpr auto collision(const _T1 &d, const _T2 &c) noexcept
	{
		const auto dx = std::abs(d.x - c.x), dy = std::abs(d.y - c.y);

		if (dx > c.r || dy > c.r)
			return false;

		return dx + dy <= c.r || std::pow(dx, 2) + std::pow(dy, 2) <= std::pow(c.r, 2);
	}

	/**
	 * @brief Check for collision between a rechange and a point
	 *
	 * @tparam _T1 Rectange type
	 * @tparam _T2 Point type
	 * @param r Rectange
	 * @param d Point
	 * @return if collision occured
	 */
	template<mth::simple_rect _T1, mth::simple_point _T2>
	constexpr auto collision(const _T1 &r, const _T2 &d) noexcept
	{
		return collision(d, r);
	}

	/**
	 * @brief Check for collision between 2 rectangles
	 *
	 * @tparam _T1 Rectangle type 1
	 * @tparam _T2 Rectangle type 2
	 * @param r1 Rectange 1
	 * @param r2 Rectange 2
	 * @return if collision occured
	 */
	template<mth::simple_rect _T1, mth::simple_rect _T2>
	constexpr auto collision(const _T1 &r1, const _T2 &r2) noexcept
	{
		return !(r1.y + r1.h <= r2.y || r1.y >= r2.y + r2.h || r1.x + r1.w <= r2.x || r1.x >= r2.x + r2.w);
	}

	/**
	 * @brief Check for collision between a rectangle and a circle
	 *
	 * @tparam _T1 Rectange type
	 * @tparam _T2 Circle type
	 * @param r Rectangle
	 * @param c Circle
	 * @return if collision occured
	 */
	template<mth::simple_rect _T1, mth::simple_circle _T2>
	constexpr auto collision(const _T1 &r, const _T2 &c) noexcept
	{
		const auto halfWidth = r.w / 2;
		const auto halfHight = r.h / 2;
		const auto distanceX = std::abs(r.x + halfWidth - c.x);
		const auto distanceY = std::abs(r.y + halfHight - c.y);

		if (distanceX > (halfWidth + c.r))
			return false;
		if (distanceY > (halfHight + c.r))
			return false;

		if (distanceX <= halfWidth)
			return true;
		if (distanceY <= halfHight)
			return true;

		return std::pow(distanceX - halfWidth, 2) + std::pow(distanceY - halfHight, 2) <= std::pow(c.r, 2);
	}

	/**
	 * @brief Check for collision between a circle and a point
	 *
	 * @tparam _T1 Circle type
	 * @tparam _T2 Point type
	 * @param c Circle
	 * @param d Point
	 * @return if collision occured
	 */
	template<mth::simple_circle _T1, mth::simple_point _T2>
	constexpr auto collision(const _T1 &c, const _T2 &d) noexcept
	{
		return collision(d, c);
	}

	/**
	 * @brief Check for collision between a circle and a rectangle
	 *
	 * @tparam _T1 Circle type
	 * @tparam _T2 Rectangle type
	 * @param c Circle
	 * @param r Rectange
	 * @return if collision occured
	 */
	template<mth::simple_circle _T1, mth::simple_rect _T2>
	constexpr auto collision(const _T1 &c, const _T2 &r) noexcept
	{
		return collision(r, c);
	}

	/**
	 * @brief Check for collision between 2 circles
	 *
	 * @tparam _T1 Circle type 1
	 * @tparam _T2 Circle type 2
	 * @param c1 Circle 1
	 * @param c2 Circle 2
	 * @return if collision occured
	 */
	template<mth::simple_circle _T1, mth::simple_circle _T2>
	constexpr auto collision(const _T1 &c1, const _T2 &c2) noexcept
	{
		return power2(c1.x - c2.x) + power2(c1.y - c2.y) < power2(c1.r + c2.r);
	}

} // namespace ctl::mth