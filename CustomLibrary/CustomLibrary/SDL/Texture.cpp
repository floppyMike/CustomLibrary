#include "Texture.h"

void ctl::BaseTexture::render(const double& angle, const SDL_RendererFlip& flip, 
	const SDL_Point* const center, const SDL_Rect* const clip) const
{
	SDL_Rect renderQuad = m_pos;

	if (clip != nullptr)
		renderQuad.w = clip->w,
		renderQuad.h = clip->h;

	if (SDL_RenderCopyEx(m_win->renderer(), m_texture.get(), clip, &renderQuad, angle, center, flip) < 0)
		throw Log(SDL_GetError());
}

void ctl::BaseTexture::render(const SDL_Rect* const clip) const
{
	SDL_Rect renderQuad = m_pos;

	if (clip != nullptr)
		renderQuad.w = clip->w,
		renderQuad.h = clip->h;

	if (SDL_RenderCopy(m_win->renderer(), m_texture.get(), clip, &renderQuad) < 0)
		throw Log(SDL_GetError());
}

ctl::BaseTexture& ctl::BaseTexture::colourMod(const SDL_Color& c)
{
	if (SDL_SetTextureColorMod(m_texture.get(), c.r, c.g, c.b) != 0)
		throw Log(SDL_GetError());

	return *this;
}

ctl::NumVec<Uint8, 3> ctl::BaseTexture::colourMod() const
{
	NumVec<Uint8, 3> c;

	if (SDL_GetTextureColorMod(m_texture.get(), &c[0], &c[1], &c[2]) != 0)
		throw Log(SDL_GetError());

	return c;
}

ctl::BaseTexture& ctl::BaseTexture::blendMode(const SDL_BlendMode& b)
{
	if (SDL_SetTextureBlendMode(m_texture.get(), b) != 0)
		throw Log(SDL_GetError());

	return *this;
}

SDL_BlendMode ctl::BaseTexture::blendMode() const
{
	SDL_BlendMode b;

	if (SDL_GetTextureBlendMode(m_texture.get(), &b) != 0)
		throw Log(SDL_GetError());

	return b;
}

ctl::BaseTexture& ctl::BaseTexture::alphaMod(const Uint8& a)
{
	if (SDL_SetTextureAlphaMod(m_texture.get(), a) != 0)
		throw Log(SDL_GetError());

	return *this;
}

Uint8 ctl::BaseTexture::alphaMod() const
{
	Uint8 a;

	if (SDL_GetTextureAlphaMod(m_texture.get(), &a) == -1)
		throw Log(SDL_GetError());

	return a;
}

ctl::FixedTexture& ctl::FixedTexture::load(SDL_Surface* surface, const SDL_Color* const key)
{
	if (key != nullptr)
		SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, key->r, key->g, key->b));

	m_pos.dim() = { surface->w, surface->h };

	m_texture.reset(SDL_CreateTextureFromSurface(m_win->renderer(), surface));
	if (!m_texture)
		throw Log(SDL_GetError());

	SDL_FreeSurface(surface);

	return *this;
}
