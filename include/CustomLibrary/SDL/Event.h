#pragma once

#include <SDL2/SDL.h>

#include "../Error.h"
#include "../BasicTypes.h"

namespace ctl::sdl
{
	/**
	 * @brief Manages a custom event
	 */
	class UserEvent
	{
	public:
		/**
		 * @brief construct SDL_Event and register type
		 * @exception std::runtime_error failure at registering event
		 */
		UserEvent()
		{
			SDL_zero(m_event);

			m_event.type = SDL_RegisterEvents(1);
			if (m_event.type == std::numeric_limits<Uint32>::max())
				throw std::runtime_error("Event not registered.");
		}

		/**
		 * @brief pushes event onto SDL event queue
		 */
		void push_event() noexcept
		{
			if (SDL_PushEvent(&m_event) < 0)
				std::cerr << SDL_GetError() << std::endl;
		}

		/**
		 * @brief access event type
		 * @return Uint32
		 */
		[[nodiscard]] constexpr auto type() const noexcept { return m_event.type; }

		/**
		 * @brief Get the user type
		 * @return Uint32
		 */
		[[nodiscard]] constexpr auto user_type() const noexcept { return m_event.user.type; }
		/**
		 * @brief Set the user type
		 * @param t new user type
		 */
		constexpr auto user_type(Uint32 t) noexcept { m_event.user.type = t; }

		/**
		 * @brief Get user event code
		 */
		[[nodiscard]] constexpr auto code() const noexcept { return m_event.user.code; }
		/**
		 * @brief Set user event code
		 * @param code Code to use
		 */
		constexpr auto code(const Sint32 &code) noexcept -> void { m_event.user.code = code; }

		/**
		 * @brief Get set user data
		 * @return std::pair<void *, void *>
		 */
		[[nodiscard]] constexpr auto data() const noexcept -> std::pair<void *, void *>
		{
			return { m_event.user.data1, m_event.user.data2 };
		}
		/**
		 * @brief Set user data
		 */
		constexpr auto data(void *data1, void *data2) noexcept
		{
			m_event.user.data1 = data1;
			m_event.user.data2 = data2;
		}

	private:
		SDL_Event m_event;
	};

	/**
	 * @brief Request to mouse position
	 * @return Point
	 */
	auto mouse_position()
	{
		mth::Point<int> pos;
		SDL_GetMouseState(&pos.x, &pos.y);
		return pos;
	}

} // namespace ctl::sdl
