#pragma once

#include "BasicTypes.h"

namespace ctl
{
	namespace sdl
	{
		class Window
		{
		public:
			Window(const char* name, const Dim<int>& dim, Uint32 windowFlags = SDL_WINDOW_SHOWN);

			~Window()
			{
				destroy();
			}

			void destroy() noexcept;

			constexpr auto* get() noexcept { return m_window; }
			constexpr auto ID() const noexcept { return m_id; }
			auto dim() const noexcept 
			{
				Dim<int> size;
				SDL_GetWindowSize(m_window, &size.w, &size.h);
				return size;
			}

		private:
			SDL_Window* m_window = nullptr;
			Uint32 m_id;
		};


		//----------------------------------------------
		//Implementation
		//----------------------------------------------

		inline Window::Window(const char* name, const Dim<int>& dim, Uint32 windowFlags)
		{
			m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				dim.w, dim.h, windowFlags);
			if (m_window == nullptr)
				throw ctl::Log(SDL_GetError());

			m_id = SDL_GetWindowID(m_window);
		}

		inline void Window::destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window),
				m_window = nullptr;
		}
	}
}