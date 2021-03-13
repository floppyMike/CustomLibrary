#pragma once

#include <SDL2/SDL.h>

#include "../Traits.h"
#include "../Rect.h"

#include <cassert>

namespace ctl::sdl
{
	// clang-format off
	template<typename T>
	concept uses_sdl_shape = requires(T t)
	{
		{ t.shape() } -> same_as<mth::Rect<int, int> &, const mth::Rect<int, int> &>;
		t.shape(mth::Rect<int, int>());
	};

	template<typename T>
	concept uses_sdl_shape_const = requires(T t)
	{
		{ t.shape() } -> std::same_as<const mth::Rect<int, int> &>;
		t.shape(mth::Rect<int, int>());
	};

	template<typename T>
	concept uses_sdl_renderer = requires(T t)
	{
		{ t.renderer()->get() } -> std::same_as<SDL_Renderer *>;
	};
	// clang-format on

} // namespace ctl::sdl
