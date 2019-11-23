#pragma once

#include "BasicTypes.h"
#include <cassert>

namespace ctl::sdl
{
	class Window
	{
		constexpr void _win_exist_() const noexcept
		{
			assert(m_window != nullptr && "Window isn't loaded.");
		}

	public:
		Window(const char* name, const Dim<int>& dim, Uint32 windowFlags = SDL_WINDOW_SHOWN)
		{
			m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				dim.w, dim.h, windowFlags);
			if (m_window == nullptr)
				throw ctl::Log(SDL_GetError());

			m_id = SDL_GetWindowID(m_window);
		}

		~Window()
		{
			destroy();
		}

		void destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window),
				m_window = nullptr;
		}

		constexpr auto* get() noexcept 
		{ 
			_win_exist_();
			return m_window; 
		}
		constexpr auto ID() const noexcept 
		{
			_win_exist_();
			return m_id;
		}
		auto dim() const noexcept 
		{
			_win_exist_();
			Dim<int> size;
			SDL_GetWindowSize(m_window, &size.w, &size.h);
			return size;
		}

	private:
		SDL_Window* m_window = nullptr;
		Uint32 m_id;
	};

}