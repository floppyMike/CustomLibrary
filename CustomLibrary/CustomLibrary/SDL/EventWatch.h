#pragma once

#include <SDL.h>
#include <bitset>

namespace ctl
{
	namespace sdl
	{
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