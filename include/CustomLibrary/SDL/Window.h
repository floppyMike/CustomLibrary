#pragma once

#include <SDL2/SDL.h>

#include "../Dim.h"

#include <cassert>
#include <memory>

namespace ctl::sdl
{
	namespace detail
	{
		struct Window_Destructor
		{
			void operator()(SDL_Window *w) { SDL_DestroyWindow(w); }
		};
	}

	using Window = std::unique_ptr<SDL_Window, detail::Window_Destructor>;

} // namespace ctl::sdl