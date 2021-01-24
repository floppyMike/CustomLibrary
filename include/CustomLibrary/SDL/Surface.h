#if not defined _CTL_SDL2_SURFACE_
#define _CTL_SDL2_SURFACE_

#include "SDL_init.h"

#include "../Error.h"

#include <memory>
#include <cassert>
#include <stdexcept>

namespace ctl::sdl
{
	/**
	 * @brief Manages a Surface texture
	 */
	class Surface
	{
		struct Unique_Destructor
		{
			void operator()(SDL_Surface *s) { SDL_FreeSurface(s); }
		};

	public:
		Surface() noexcept			 = default;
		Surface(Surface &&) noexcept = default;
		auto operator=(Surface &&) noexcept -> Surface & = default;

		Surface(const Surface &) noexcept = delete;
		auto operator=(const Surface &) noexcept = delete;

		/**
		 * @brief Adapts surface from ptr
		 * @param s New surface to use
		 */
		explicit Surface(SDL_Surface *s) noexcept
			: m_surface(s)
		{
		}

		/**
		 * @brief Gets the SDL_Surface ptr
		 * @return SDL_Surface*
		 */
		[[nodiscard]] auto surface() const noexcept { return m_surface.get(); }

		/**
		 * @brief Adapts surface from ptr
		 * @param s New surface to use
		 */
		auto surface(SDL_Surface *s) noexcept -> void { m_surface.reset(s); }

		/**
		 * @brief Changes the color key for the surface
		 * The color key defines what pixels are treated as transparent.
		 * @param key Color to use
		 * @param enabled If transparent
		 */
		auto color_key(Uint32 key, SDL_bool enabled = SDL_TRUE) -> void
		{
			const auto r = SDL_SetColorKey(m_surface.get(), enabled, key);
			ASSERT(r == 0, SDL_GetError());
		}

		/**
		 * @brief Resizes the surface to a new dim
		 *
		 * @param x new x dim -> nullptr if should scale
		 * @param y new y dim -> nullptr if should scale
		 */
		auto resize(const int *const x, const int *const y) -> void
		{
			if (x == nullptr && y == nullptr)
				return;

			SDL_Surface *temp = SDL_CreateRGBSurface(
				0, x == nullptr ? static_cast<int>(static_cast<double>(*y) / m_surface->h * m_surface->w) : *x,
				y == nullptr ? static_cast<int>(static_cast<double>(*x) / m_surface->w * m_surface->h) : *y,
				m_surface->format->BitsPerPixel, m_surface->format->Rmask, m_surface->format->Gmask,
				m_surface->format->Bmask, m_surface->format->Amask);

			SDL_BlitScaled(m_surface.get(), nullptr, temp, nullptr);

			m_surface.reset(temp);
		}

	private:
		std::unique_ptr<SDL_Surface, Unique_Destructor> m_surface;
	};

} // namespace ctl::sdl

#endif