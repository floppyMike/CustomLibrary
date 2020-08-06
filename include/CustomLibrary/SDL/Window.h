#pragma once

#include <SDL2/SDL.h>

#include "../BasicTypes.h"
#include "../Error.h"

#include <cassert>

namespace ctl::sdl
{
	/**
	 * @brief Manages a window instance
	 */
	class Window
	{
		constexpr void _win_exist_() const noexcept { assert(m_window != nullptr && "Window isn't loaded."); }

	public:
		/**
		 * @brief Initializes a new window
		 * 
		 * @param name Window title
		 * @param dim Dimensions of the window
		 * @param window_flags Window creation flags
		 */
		Window(const char *name, const mth::Dim<int> &dim, Uint32 window_flags = SDL_WINDOW_SHOWN)
		{
			m_window =
				SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dim.w, dim.h, window_flags);
			if (m_window == nullptr)
				throw std::runtime_error(SDL_GetError());
		}

		~Window() { destroy(); }

		/**
		 * @brief Kills and deallocated the window 
		 */
		void destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window), m_window = nullptr;
		}

		/**
		 * @brief Get the window ptr
		 * @return SDL_Window ptr
		 */
		constexpr auto get() noexcept
		{
			_win_exist_();
			return m_window;
		}
		/**
		 * @brief Get the window ID registered by the OS
		 * @return unsigned int
		 */
		[[nodiscard]] auto ID() const noexcept
		{
			_win_exist_();
			return SDL_GetWindowID(m_window);
		}
		/**
		 * @brief Get the current window dimension
		 * @return Dimension
		 */
		[[nodiscard]] auto dim() const noexcept
		{
			_win_exist_();
			mth::Dim<int> size;
			SDL_GetWindowSize(m_window, &size.w, &size.h);
			return size;
		}

	private:
		SDL_Window *m_window = nullptr;
	};

} // namespace ctl::sdl