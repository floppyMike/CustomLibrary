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

		e.user.type		= SDL_RegisterEvents(1);
		e.user.windowID = win_id;

		return e;
	}

	/**
	 * @brief Create a user event
	 * @param win_id window id to use
	 * @param type user event type
	 * @return window event
	 */
	inline auto create_event(Uint32 win_id, int type) noexcept
	{
		SDL_Event e;
		SDL_zero(e);

		e.user.type		= type;
		e.user.windowID = win_id;

		return e;
	}

	/**
	 * @brief Create a event and push it
	 *
	 * @param win_id window id to use
	 * @param type user event type
	 */
	inline void push_event(Uint32 win_id, int type) noexcept
	{
		auto e = create_event(win_id, type);
		SDL_PushEvent(&e);
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
