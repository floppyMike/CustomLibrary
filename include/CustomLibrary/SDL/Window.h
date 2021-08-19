#pragma once

#include <SDL.h>

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
