#include "Sprite.h"

ctl::Sprite& ctl::Sprite::load(SDL_Surface* surface, const std::array<const int* const, 2>& blit, 
	const SDL_Color* const key)
{
	FixedTexture::load(_resizeSurfaceY_(surface, blit), key);
	return *this;
}

ctl::Sprite& ctl::Sprite::load(const char* path, const std::array<const int* const, 2>& blit, 
	const SDL_Color* const key)
{
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (!loadedSurface)
		throw Log(SDL_GetError());

	return load(loadedSurface, blit, key);
}

ctl::Sprite& ctl::Sprite::load(void* src, const int& size, const std::array<const int* const, 2> & blit,
	const SDL_Color* const key)
{
	SDL_Surface* loadedSurface = IMG_Load_RW(SDL_RWFromMem(src, size), 0);
	if (!loadedSurface)
		throw Log(SDL_GetError());

	return load(loadedSurface, blit, key);
}

SDL_Surface* ctl::Sprite::_resizeSurfaceY_(SDL_Surface* surface, const std::array<const int* const, 2>& blit)
{
	if (blit[0] != nullptr || blit[1] != nullptr)
	{
		SDL_Surface* tempSurface;
		if (blit[0] != nullptr && blit[1] != nullptr)
			tempSurface = SDL_CreateRGBSurface(0, *blit[0], *blit[1], surface->format->BitsPerPixel,
				surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
		else
			tempSurface = SDL_CreateRGBSurface(0, blit[0] == nullptr ? static_cast<int>(static_cast<double>(*blit[1]) / surface->h * surface->w) : *blit[0],
				blit[1] == nullptr ? static_cast<int>(static_cast<double>(*blit[0]) / surface->w * surface->h) : *blit[1],
				surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

		SDL_BlitScaled(surface, nullptr, tempSurface, nullptr);

		SDL_FreeSurface(surface);
		surface = tempSurface;
	}

	return surface;
}
