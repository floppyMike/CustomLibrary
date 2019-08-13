#include "Event.h"

ctl::UserEvent::UserEvent()
{
	SDL_zero(m_event);

	m_event.type = SDL_RegisterEvents(1);
	if (m_event.type == std::numeric_limits<Uint32>::max())
		throw Log("UserEvent: event not registered.");
}

void ctl::UserEvent::pushEvent() noexcept
{
	if (SDL_PushEvent(&m_event) < 0)
		Log::logWrite(SDL_GetError(), Log::Sev::WARNING);
}
