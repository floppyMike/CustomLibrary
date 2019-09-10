#include "SDLWindow.h"

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
