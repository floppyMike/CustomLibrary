#pragma once

#include <array>
#include <sstream>

#include "SDLWindow.h"
#include "Texture.h"

namespace ctl
{
	class Sprite : public FixedTexture
	{
		void _load_(const std::string& path, const std::array<const int* const, 2> & blit = { nullptr }, const SDL_Color * key = nullptr)
		{
			SDL_Surface* loadedSurface = IMG_Load(path.c_str());
			if (!loadedSurface)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			FixedTexture::load(_resizeSurfaceY_(loadedSurface, blit), key);
		}
		void _load_(std::stringstream& content, const std::array<const int* const, 2> & blit = { nullptr }, const SDL_Color * key = nullptr)
		{
			SDL_Surface* loadedSurface = IMG_Load_RW(SDL_RWFromMem(&content.str()[0], content.str().size()), 1);
			if (!loadedSurface)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			FixedTexture::load(_resizeSurfaceY_(loadedSurface, blit), key);
		}

	public:
		Sprite() = default;
		Sprite(const Sprite&) = delete;
		Sprite(Sprite&&) = default;

		Sprite& operator=(const Sprite &) = delete;
		Sprite& operator=(Sprite &&) = default;

		Sprite(SDLWindow* e)
			: FixedTexture(e)
		{
		}

		Sprite(SDLWindow* e, const std::string& path, const std::array<const int* const, 2> & blit = { nullptr }, const SDL_Color* key = nullptr)
			: FixedTexture(e)
		{
			_load_(path, blit, key);
		}
		Sprite(SDLWindow* e, std::stringstream& content, const std::array<const int* const, 2> & blit = { nullptr }, const SDL_Color * key = nullptr)
			: FixedTexture(e)
		{
			_load_(content, blit, key);
		}

		auto& load(const std::string &path, const std::array<const int* const, 2> &blit = { nullptr }, const SDL_Color* key = nullptr)
		{
			_load_(path, blit, key);
			return *this;
		}
		auto& load(std::stringstream &content, const std::array<const int* const, 2> &blit = { nullptr }, const SDL_Color* key = nullptr)
		{
			_load_(content, blit, key);
			return *this;
		}

	private:
		using FixedTexture::load;

		SDL_Surface* _resizeSurfaceY_(SDL_Surface *surface, const std::array<const int* const, 2> &blit)
		{
			if (blit[0] != nullptr || blit[1] != nullptr)
			{
				SDL_Surface *tempSurface;
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
	};
}