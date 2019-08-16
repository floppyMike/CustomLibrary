#include "SDLWindow.h"

ctl::SDLWindow::SDLWindow(const std::string& name, 
	const SDLDim<Uint32>& dim, 
	const Uint32& windowFlags, 
	const Uint32& rendererFlags)
	: WindowBase(name, dim, windowFlags)
	, cam(m_dim)
{
	if ((m_renderer = SDL_CreateRenderer(m_window, -1, rendererFlags)) == nullptr)
		throw ctl::Log(SDL_GetError());

	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void ctl::SDLWindow::destroy()
{
	if (m_renderer != nullptr)
		SDL_DestroyRenderer(m_renderer);

	WindowBase::destroy();
}

void ctl::SDLWindow::clear()
{
	SDL_SetRenderDrawColor(m_renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(m_renderer);
}

void ctl::SDLWindow::_render_()
{
	clear();
	m_state->render();
	render();
}

constexpr ctl::Camera2D& ctl::Camera2D::mov(const float& deltaX, const float& deltaY) noexcept
{
	m_camLoc.x += deltaX;
	m_camLoc.y += deltaY;

	return *this;
}
