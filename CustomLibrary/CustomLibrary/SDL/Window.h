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

			void update(const SDL_Event& e) noexcept;

			constexpr auto* get() noexcept { return m_window; }
			constexpr auto ID() const noexcept { return m_id; }
			constexpr const auto& dim() const noexcept { return m_dim; }

		private:
			SDL_Window* m_window = nullptr;
			Uint32 m_id;
			Dim<int> m_dim;
		};


		//----------------------------------------------
		//Implementation
		//----------------------------------------------

		inline Window::Window(const char* name, const Dim<int>& dim, Uint32 windowFlags)
			: m_dim(dim)
		{
			m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				m_dim.w, m_dim.h, windowFlags);
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

		inline void Window::update(const SDL_Event& e) noexcept
		{
			m_dim.w = e.window.data1;
			m_dim.h = e.window.data2;
		}
	}
}