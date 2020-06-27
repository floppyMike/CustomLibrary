#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <cassert>
#include <exception>

namespace ctl::sdl
{
	class Surface
	{
		struct Unique_Destructor
		{
			void operator()(SDL_Surface *s) { SDL_FreeSurface(s); }
		};

	public:
		Surface() noexcept			 = default;
		Surface(Surface &&) noexcept = default;
		Surface &operator=(Surface &&) noexcept = default;

		Surface(SDL_Surface *s) noexcept
			: m_surface(s)
		{
		}

		SDL_Surface *surface() noexcept { return m_surface.get(); }

		Surface &surface(SDL_Surface *s) noexcept
		{
			m_surface.reset(s);
			return *this;
		}

		Surface &color_key(Uint32 key, SDL_bool enabled = SDL_TRUE)
		{
			if (SDL_SetColorKey(m_surface.get(), enabled, key) != 0)
				throw std::runtime_error(SDL_GetError());
			return *this;
		}

		Surface &resize(const int *const x, const int *const y)
		{
			assert(x == nullptr && y == nullptr && "Both x and y shouldn't be nullptrs.");

			SDL_Surface *temp = SDL_CreateRGBSurface(
				0, x == nullptr ? static_cast<int>(static_cast<double>(*y) / m_surface->h * m_surface->w) : *x,
				y == nullptr ? static_cast<int>(static_cast<double>(*x) / m_surface->w * m_surface->h) : *y,
				m_surface->format->BitsPerPixel, m_surface->format->Rmask, m_surface->format->Gmask,
				m_surface->format->Bmask, m_surface->format->Amask);

			SDL_BlitScaled(m_surface.get(), nullptr, temp, nullptr);

			m_surface.reset(temp);
			return *this;
		}

	private:
		std::unique_ptr<SDL_Surface, Unique_Destructor> m_surface;
	};

} // namespace ctl::sdl