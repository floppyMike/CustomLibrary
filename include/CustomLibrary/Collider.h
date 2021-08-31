#pragma once

#include <cmath>
#include "Geometry.h"

namespace ctl::mth
{
	/**
	 * @brief Checks for collision between 2 points
	 *
	 * @param d1 point 1
	 * @param d2 point 2
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Point<T> d1, mth::Point<T> d2) noexcept
	{
		return d1 == d2;
	}

	/**
	 * @brief Check for collision between a point and a rectange
	 *
	 * @param d Point
	 * @param r Rectange
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Point<T> d, mth::Rect<T> r) noexcept
	{
		return !(d.x < r.x || d.x > r.x + r.w || d.y < r.y || d.y > r.y + r.h);
	}

	/**
	 * @brief Check for collision between a point and a circle
	 *
	 * @param d Point
	 * @param c Circle
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Point<T> d, mth::Circle<T> c) noexcept
	{
		const auto dx = std::abs(d.x - c.x), dy = std::abs(d.y - c.y);

		if (dx > c.r || dy > c.r)
			return false;

		return dx + dy <= c.r || std::pow(dx, 2) + std::pow(dy, 2) <= std::pow(c.r, 2);
	}

	/**
	 * @brief Check for collision between a rechange and a point
	 *
	 * @param r Rectange
	 * @param d Point
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Rect<T> r, mth::Point<T> d) noexcept
	{
		return collision(d, r);
	}

	/**
	 * @brief Check for collision between 2 rectangles
	 *
	 * @param r1 Rectange 1
	 * @param r2 Rectange 2
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Rect<T> r1, mth::Rect<T> r2) noexcept
	{
		return !(r1.y + r1.h <= r2.y || r1.y >= r2.y + r2.h || r1.x + r1.w <= r2.x || r1.x >= r2.x + r2.w);
	}

	/**
	 * @brief Check for collision between a rectangle and a circle
	 *
	 * @param r Rectangle
	 * @param c Circle
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Rect<T> r, mth::Circle<T> c) noexcept
	{
		const auto halfWidth = r.w / 2;
		const auto halfHight = r.h / 2;
		const auto distanceX = std::abs(r.x + halfWidth - c.x);
		const auto distanceY = std::abs(r.y + halfHight - c.y);

		if (distanceX > (halfWidth + c.r) || distanceY > (halfHight + c.r))
			return false;

		if (distanceX <= halfWidth || distanceY <= halfHight)
			return true;

		return std::pow(distanceX - halfWidth, 2) + std::pow(distanceY - halfHight, 2) <= std::pow(c.r, 2);
	}

	/**
	 * @brief Check for collision between a circle and a point
	 *
	 * @param c Circle
	 * @param d Point
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Circle<T> c, mth::Point<T> d) noexcept
	{
		return collision(d, c);
	}

	/**
	 * @brief Check for collision between a circle and a rectangle
	 *
	 * @param c Circle
	 * @param r Rectange
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Circle<T> c, mth::Rect<int> r) noexcept
	{
		return collision(r, c);
	}

	/**
	 * @brief Check for collision between 2 circles
	 *
	 * @param c1 Circle 1
	 * @param c2 Circle 2
	 * @return if collision occured
	 */
	template<typename T>
	constexpr auto collision(mth::Circle<T> c1, mth::Circle<T> c2) noexcept
	{
		return std::pow(c1.x - c2.x, 2) + std::pow(c1.y - c2.y, 2) < std::pow(c1.r + c2.r, 2);
	}

} // namespace ctl::mth
