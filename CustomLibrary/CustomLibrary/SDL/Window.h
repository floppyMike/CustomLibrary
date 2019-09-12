#pragma once

#include <CustomLibrary/Vector.h>

#include "State.h"
#include "BasicTypes.h"

namespace ctl
{
	namespace sdl
	{
		class Window
		{
		public:
			Window(const char* name, const SDLDim<int>& dim, Uint32 windowFlags = SDL_WINDOW_SHOWN)
				: m_dim(dim)
			{
				m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					m_dim.w, m_dim.h, windowFlags);
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
					SDL_DestroyWindow(m_window);
			}

			void update(const SDL_Event& e) noexcept
			{
				m_dim.w = e.window.data1;
				m_dim.h = e.window.data2;
			}

			constexpr auto* get() noexcept { return m_window; }
			constexpr auto ID() const noexcept { return m_id; }
			constexpr const auto& dim() const noexcept { return m_dim; }

		private:
			SDL_Window* m_window = nullptr;
			Uint32 m_id;
			SDLDim<int> m_dim;
		};

		class EventWatch
		{
		public:
			enum Focus { MOUSE, KEYBOARD, FULLSCREEN, MINIMIZED, SHOWN, CLOSE };

			void listen(const SDL_Event& e) noexcept;
			bool state(size_t foc) const noexcept
			{
				return m_focus[foc];
			}

		private:
			std::bitset<6> m_focus = std::bitset<6>(0b010011);
		};


		//----------------------------------------------
		//Implementation
		//----------------------------------------------

		inline void EventWatch::listen(const SDL_Event& e) noexcept
		{
			if (e.type == SDL_WINDOWEVENT)
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_ENTER:		   m_focus[MOUSE] = true; break;
				case SDL_WINDOWEVENT_LEAVE:		   m_focus[MOUSE] = false; break;
				case SDL_WINDOWEVENT_FOCUS_GAINED: m_focus[KEYBOARD] = true; break;
				case SDL_WINDOWEVENT_FOCUS_LOST:   m_focus[KEYBOARD] = false; break;
				case SDL_WINDOWEVENT_MINIMIZED:    m_focus[MINIMIZED] = true; break;
				case SDL_WINDOWEVENT_MAXIMIZED:
				case SDL_WINDOWEVENT_RESTORED:	   m_focus[MINIMIZED] = false; break;

				default: break;
				}
		}

	}
}