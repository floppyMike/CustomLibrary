#pragma once

#include <SDL2/SDL.h>

#include <utility>
#include <functional>

#include "../Collider.h"
#include "Geometry.h"

namespace ctl::sdl
{
	/**
	 * @brief Enables object to be pressable
	 * @tparam T base must have shape() method
	 */
	template<typename T>
	class Pressable : public T
	{
	public:
		using base = T;

		using T::T;

		/**
		 * @brief Handle input
		 * Checks if mouse is inside button and if button was pressed.
		 * @param e SDL_Event
		 */
		void input(const SDL_Event &e)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				if (collision(mth::Point(e.motion.x, e.motion.y), this->shape()))
				{
					if (!m_is_inside)
						m_is_inside = true;
				}
				else if (m_is_inside)
					m_is_inside = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (m_is_inside)
					m_func();
				break;

			default: break;
			}
		}

		/**
		 * @brief Change the function that gets called at button press
		 * @param f New function
		 */
		auto func(std::function<void()> &&f) noexcept -> void { m_func = std::move(f); }

		/**
		 * @brief Check if button is inside the button
		 * @return bool
		 */
		auto is_inside() const noexcept { return m_is_inside; }

	private:
		bool				  m_is_inside = false;
		std::function<void()> m_func;
	};

} // namespace ctl::sdl