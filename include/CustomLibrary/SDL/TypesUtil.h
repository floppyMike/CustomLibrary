#pragma once

#include "SDL_init.h"

#include "../Traits.h"
#include "../Geometry.h"

namespace ctl::sdl
{
	// clang-format off
	template<typename T>
	concept is_valid_rect = same_as_one_of<std::remove_cv_t<T>, mth::Rect<int>, SDL_Rect>;

	template<typename T>
	concept is_valid_frect = same_as_one_of<std::remove_cv_t<T>, mth::Rect<float>, SDL_FRect>;

	template<typename T>
	concept is_valid_point = same_as_one_of<std::remove_cv_t<T>, mth::Point<int>, SDL_Point>;

	template<typename T>
	concept is_valid_fpoint = same_as_one_of<std::remove_cv_t<T>, mth::Point<float>, SDL_FPoint>;
	// clang-format on

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	inline auto to_rect(const mth::Rect<int> &r) -> const SDL_Rect &
	{
		return *(const SDL_Rect *)&r;
	}

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	inline auto to_rect(const mth::Rect<float> &r) -> const SDL_FRect &
	{
		return *(const SDL_FRect *)&r;
	}

	/**
	 * @brief Overload when using SDL_Rect
	 */
	inline auto to_rect(const SDL_Rect &r) -> const SDL_Rect &
	{
		return r;
	}

	/**
	 * @brief Overload when using SDL_FRect
	 */
	inline auto to_rect(const SDL_FRect &r) -> const SDL_FRect &
	{
		return r;
	}

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	inline auto to_point(const mth::Point<int> &r) -> const SDL_Point &
	{
		return *(const SDL_Point *)&r;
	}

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	inline auto to_point(const mth::Point<float> &r) -> const SDL_FPoint &
	{
		return *(const SDL_FPoint *)&r;
	}

	/**
	 * @brief Overload when using SDL_Point
	 */
	inline auto to_point(const SDL_Point &r) -> const SDL_Point &
	{
		return r;
	}

	/**
	 * @brief Overload when using SDL_FPoint
	 */
	inline auto to_point(const SDL_FPoint &r) -> const SDL_FPoint &
	{
		return r;
	}

} // namespace ctl::sdl
