#pragma once

#include <SDL.h>

#include <CustomLibrary/Error.h>

namespace ctl
{
	class UserEvent
	{
	public:
		/**
		* @summary construct SDL_Event and register type
		* @exception "Log" failure at registering event
		*/
		UserEvent();

		/**
		* @summary pushes event onto SDL event queue
		*/
		void pushEvent() noexcept;

		/**
		* @summary const access for event type
		*/
		constexpr const auto& type() const noexcept { return m_event.type; }

		/**
		* @summary access for code of event
		*/
		constexpr const auto& code() const noexcept { return m_event.user.code; }
		constexpr auto& code(const Sint32& code) noexcept
		{
			m_event.user.code = code;
			return *this;
		}

		/**
		* @summary access for data pointer of event
		*/
		constexpr const auto& data() const noexcept { return std::make_pair(m_event.user.data1, m_event.user.data2); }
		constexpr auto& data(void* data1, void* data2) noexcept
		{
			m_event.user.data1 = data1;
			m_event.user.data2 = data2;
			return *this;
		}

	private:
		SDL_Event m_event;
	};
}
