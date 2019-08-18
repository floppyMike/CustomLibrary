#pragma once

#include <SDL_image.h>

#include <array>
#include <sstream>

#include "SDLWindow.h"
#include "Texture.h"

namespace ctl
{
	class Sprite : public FixedTexture
	{
	public:
		Sprite() = default;
		Sprite(const Sprite&) = delete;
		Sprite(Sprite&&) = default;

		Sprite& operator=(const Sprite &) = delete;
		Sprite& operator=(Sprite &&) = default;

		/**
		* @summary constructs empty Sprite
		* @param "e" SDLWindow ptr
		*/
		Sprite(SDLWindow* e)
			: FixedTexture(e)
		{
		}

		/**
		* @summary loads texture from SDL_Surface
		* @param "surface" SDL_Surface to load from
		* @param "blit" array of const int* for resizing texture
		* @param "key" colour key
		*/
		Sprite& load(SDL_Surface* surface, const std::array<const int* const, 2>& blit = { nullptr },
			const SDL_Color* const key = nullptr);

		/**
		* @summary loads texture from specified path
		* @param "path" path to load texture from
		* @param "blit" array of const int* for resizing texture
		* @param "key" colour key
		*/
		Sprite& load(const char* path, const std::array<const int* const, 2> &blit = { nullptr },
			const SDL_Color* const key = nullptr);

		/**
		* @summary loads texture from SDL_Surface
		* @param "src" void* of src array
		* @param "size" size of array
		* @param "blit" array of const int* for resizing texture
		* @param "key" colour key
		*/
		Sprite& load(void* src, const int& size, const std::array<const int* const, 2>& blit = { nullptr },
			const SDL_Color* const key = nullptr);

	private:
		/**
		* @summary resizes the surface
		* @param "surface" SDL_Surface to resize
		* @param "blit" new size parameters
		* @remarks frees previous surface
		* @returns changed surface
		*/
		SDL_Surface* _resizeSurfaceY_(SDL_Surface *surface, const std::array<const int* const, 2> &blit);
	};
}