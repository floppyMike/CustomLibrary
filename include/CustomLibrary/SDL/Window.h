#pragma once

#include <SDL2/SDL.h>

#include "../BasicTypes.h"
#include "../Error.h"

#include <cassert>

namespace ctl::sdl
{
	class Window
	{
		constexpr void _win_exist_() const noexcept { assert(m_window != nullptr && "Window isn't loaded."); }

	public:
		Window(const char *name, const mth::Dim<int> &dim, Uint32 windowFlags = SDL_WINDOW_SHOWN)
		{
			m_window =
				SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dim.w, dim.h, windowFlags);
			if (m_window == nullptr)
				throw std::runtime_error(SDL_GetError());
		}

		~Window() { destroy(); }

		void destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window), m_window = nullptr;
		}

		constexpr auto get() noexcept
		{
			_win_exist_();
			return m_window;
		}
		[[nodiscard]] auto ID() const noexcept
		{
			_win_exist_();
			return SDL_GetWindowID(m_window);
		}
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