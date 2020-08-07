#pragma once

#include "../BasicTypes.h"

namespace ctl::sdl
{
	/**
	 * @brief Represents the top left camera point
	 */
	class Camera2D : public mth::Point<int>
	{
	public:
		using mth::Point<int>::Point;

		/**
		 * @brief translates screen coord to world coord
		 * @param loc screen coord
		 * @return mth::Point
		 */
		template<typename T>
		constexpr auto screen_world(const mth::Point<T> &loc) const noexcept -> mth::Point<T>
		{
			return loc += *this;
		}

		/**
		 * @brief translates world coord to screen coord
		 * @param loc world coorloc world coord
		 * @return mth::Point
		 */
		template<typename T>
		constexpr auto world_screen(mth::Point<T> loc) const noexcept -> mth::Point<T>
		{
			return loc -= *this;
		}
	};

} // namespace ctl::sdl
