#pragma once

#include <SDL.h>

#include <CustomLibrary/Error.h>

namespace ctl::sdl
{
	class UserEvent
	{
	public:
		/**
		* @summary construct SDL_Event and register type
		* @exception "Log" failure at registering event
		*/
		UserEvent()
		{
			SDL_zero(m_event);

			m_event.type = SDL_RegisterEvents(1);
			if (m_event.type == std::numeric_limits<Uint32>::max())
				throw err::Log("UserEvent: event not registered.");
		}

		/**
		* @summary pushes event onto SDL event queue
		*/
		void pushEvent() noexcept
		{
			if (SDL_PushEvent(&m_event) < 0)
				Log::logWrite(SDL_GetError(), Log::Sev::WARNING);
		}

		/**
		* @summary const access for event type
		*/
		constexpr Uint32 type() const noexcept { return m_event.type; }

		constexpr Uint32 userType() const noexcept
		{
			return m_event.user.type;
		}
		constexpr auto& userType(Uint32 t) noexcept
		{
			m_event.user.type = t;
			return *this;
		}

		/**
		* @summary access for code of event
		*/
		constexpr Sint32 code() const noexcept { return m_event.user.code; }
		constexpr UserEvent& code(const Sint32& code) noexcept
		{
			m_event.user.code = code;
			return *this;
		}

		/**
		* @summary access for data pointer of event
		*/
		constexpr std::pair<void*, void*> data() const noexcept
		{ 
			return { m_event.user.data1, m_event.user.data2 };
		}
		constexpr UserEvent& data(void* data1, void* data2) noexcept
		{
			m_event.user.data1 = data1;
			m_event.user.data2 = data2;
			return *this;
		}

	private:
		SDL_Event m_event;
	};
}
