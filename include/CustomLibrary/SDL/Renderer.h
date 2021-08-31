#if not defined _CTL_SDL2_RENDERER_
#define _CTL_SDL2_RENDERER_

#include "SDL_init.h"
#include <memory>

namespace ctl::sdl
{
	namespace detail
	{
		struct Renderer_Destructor
		{
			void operator()(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
		};
	}

	using Renderer = std::unique_ptr<SDL_Renderer, detail::Renderer_Destructor>;

	/**
	 * @brief Fill the buffer with a color
	 * @param col color to use
	 */
	inline void fill(SDL_Renderer *r, const SDL_Color &col)
	{
		SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);
		SDL_RenderClear(r);
	}

} // namespace ctl::sdl

#endif
