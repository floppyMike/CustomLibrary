#include "Window.h"

ctl::WindowBase::WindowBase(const std::string_view& name, const SDLDim<int>& dim, const Uint32& windowFlags)
	: m_dim(dim)
	, m_focus(0b010011)
{
	m_window = SDL_CreateWindow(name.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		m_dim.w, m_dim.h, windowFlags);
	if (m_window == nullptr)
		throw ctl::Log(SDL_GetError());

	m_id = SDL_GetWindowID(m_window);
}

void ctl::WindowBase::destroy() noexcept
{
	if (m_window != nullptr)
		SDL_DestroyWindow(m_window);
}

auto ctl::WindowBase::removState() noexcept -> WindowBase&
{
	m_state.reset();
	return *this;
}

void ctl::WindowBase::update(const SDL_Event& e)
{
	if (e.window.windowID == m_id) //Only events from this window go through here
	{
		if (e.type == SDL_WINDOWEVENT)
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				m_dim.w = e.window.data1;
				m_dim.h = e.window.data2;
				break;

			case SDL_WINDOWEVENT_ENTER:		   m_focus[MOUSE] = true; break;
			case SDL_WINDOWEVENT_LEAVE:		   m_focus[MOUSE] = false; break;
			case SDL_WINDOWEVENT_FOCUS_GAINED: m_focus[KEYBOARD] = true; break;
			case SDL_WINDOWEVENT_FOCUS_LOST:   m_focus[KEYBOARD] = false; break;
			case SDL_WINDOWEVENT_MINIMIZED:    m_focus[MINIMIZED] = true; break;
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:	   m_focus[MINIMIZED] = false; break;

			case SDL_WINDOWEVENT_CLOSE:
				m_focus[SHOWN] = false;
				m_focus[CLOSE] = true;
				destroy();
				break;

			default: break;
			}

		m_state->input(e);
		m_state->event(e);
	}
}
