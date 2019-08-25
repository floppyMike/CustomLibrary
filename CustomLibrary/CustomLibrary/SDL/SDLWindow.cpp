#include "SDLWindow.h"

ctl::SDLWindow::SDLWindow(const std::string& name, 
	const SDLDim<int>& dim, 
	const Uint32& windowFlags, 
	const Uint32& rendererFlags)
	: WindowBase(name, dim, windowFlags)
	, cam(m_dim)
{
	if ((m_renderer = SDL_CreateRenderer(m_window, -1, rendererFlags)) == nullptr)
		throw ctl::Log(SDL_GetError());

	SDL_RenderSetLogicalSize(m_renderer, m_dim.w, m_dim.h);
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

void ctl::SDLWindow::render()
{
	SDL_RenderPresent(m_renderer);
}

SDL_BlendMode ctl::SDLWindow::blendMode() noexcept
{
	SDL_BlendMode b;
	ASSERT_N(SDL_GetRenderDrawBlendMode(m_renderer, &b), == , 0, SDL_GetError());
	return b;
}

ctl::SDLWindow& ctl::SDLWindow::blendMode(const SDL_BlendMode& m) noexcept
{
	ASSERT_N(SDL_SetRenderDrawBlendMode(m_renderer, m), == , 0, SDL_GetError());
	return *this;
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
