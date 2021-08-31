#if not defined _CTL_SDL2_EVENT_
#define _CTL_SDL2_EVENT_

#include "SDL_init.h"

namespace ctl::sdl
{

	/**
	 * @brief Register an event (new event)
	 * @param win_id window id to use
	 * @return window event
	 */
	inline auto create_event(Uint32 win_id) noexcept
	{
		SDL_Event e;
		SDL_zero(e);

		e.type = SDL_RegisterEvents(1);
		e.window.windowID = win_id;

		return e;
	}

	/**
	 * @brief Create a exit event object
	 *
	 * @param win_id window id to use
	 * @return window event
	 */
	inline auto create_exit_event(Uint32 win_id) noexcept
	{
		SDL_Event e;
		SDL_zero(e);

		e.type			  = SDL_QUIT;
		e.window.windowID = win_id;

		return e;
	}

	/**
	 * @brief Request to mouse position
	 * @return Point
	 */
	inline auto mouse_position()
	{
		mth::Point<int> pos;
		SDL_GetMouseState(&pos.x, &pos.y);
		return pos;
	}

} // namespace ctl::sdl

#endif
