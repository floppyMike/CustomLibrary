#pragma once

#include <SDL.h>
#include "../Rect.h"
#include "../Traits.h"

namespace ctl::sdl
{

	// -----------------------------------------------------------------------------
	// Uniform Defines
	// -----------------------------------------------------------------------------

#if defined SDL_h_
#define _SDL2_
#endif

#if defined SDL_IMAGE_H_
#define _SDL2IMG_
#endif

#if defined SDL_MIXER_H_
#define _SDL2MIX_
#endif

#if defined SDL_TTF_H_
#define _SDL2TTF_
#endif

	// -----------------------------------------------------------------------------
	// Templated Types
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename T>
		struct Point;

		template<>
		struct Point<int>
		{
			using type = SDL_Point;
		};

		template<>
		struct Point<float>
		{
			using type = SDL_FPoint;
		};

		template<typename T>
		struct Rect;

		template<>
		struct Rect<int>
		{
			using type = SDL_Rect;
		};

		template<>
		struct Rect<float>
		{
			using type = SDL_FRect;
		};

	} // namespace detail

	template<typename T>
	using Rect = typename detail::Rect<T>::type;

	template<typename T>
	using Point = typename detail::Point<T>::type;

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	auto to_rect(const mth::Rect<int, int> &r) -> const SDL_Rect & { return *(const SDL_Rect *)&r; }

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	auto to_rect(const mth::Rect<float, float> &r) -> const SDL_FRect & { return *(const SDL_FRect *)&r; }

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	auto to_point(const mth::Point<int> &r) -> const SDL_Point & { return *(const SDL_Point *)&r; }

	/**
	 * @brief Convert between math shape and sdl shape
	 * @param r shape
	 * @return sdl shape
	 */
	auto to_rect(const mth::Point<float> &r) -> const SDL_FPoint & { return *(const SDL_FPoint *)&r; }

} // namespace ctl::sdl