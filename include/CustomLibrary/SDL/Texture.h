#if not defined _CTL_SDL2_TEXTURE_ and defined _SDL2IMG_
#define _CTL_SDL2_TEXTURE_

#include "../Dim.h"

#include "Surface.h"
#include "Renderer.h"
#include "TypeTraits.h"
#include "Geometry.h"

namespace ctl::sdl
{
	/**
	 * @brief Container class for managing Textures
	 */
	class Texture : public RectFrame
	{
		/**
		 * @brief Handles deallocation of the Texture
		 */
		struct Unique_Destructor
		{
			void operator()(SDL_Texture *t) { SDL_DestroyTexture(t); }
		};

	public:
		using base = RectFrame;

		using RectFrame::RectFrame;

		Texture() = default;

		/**
		 * @brief Gives the Texture
		 * @return Texture ptr
		 */
		[[nodiscard]] auto texture() const noexcept
		{
			assert(m_texture && "SDL_Texture is a nullptr.");
			return m_texture.get();
		}
		/**
		 * @brief Sets the Texture
		 * @param tex New Texture
		 */
		auto texture(SDL_Texture *tex) noexcept -> void
		{
			assert(tex && "SDL_Texture is a nullptr.");

			m_texture.reset(tex);
			reset_shape();
		}

		/**
		 * @brief Resets the width and height to default
		 */
		auto reset_shape() -> void
		{
			if (SDL_QueryTexture(texture(), nullptr, nullptr, &this->shape().w, &this->shape().h) != 0)
				throw std::runtime_error(SDL_GetError());
		}

		/**
		 * @brief Sets the color modulation affecting render operations
		 * Color modulation affects the coloration of the outputted texture. Color / 255. * TextureColor = Output.
		 * @param c Color to modulate with
		 */
		auto color_mod(const SDL_Color &c) const -> void
		{
			const auto re = SDL_SetTextureColorMod(texture(), c.r, c.g, c.b) + SDL_SetTextureAlphaMod(texture(), c.a);
			ASSERT(re == 0, SDL_GetError());
		}
		/**
		 * @brief Gets the color modulation used
		 * @return SDL_Color
		 */
		[[nodiscard]] auto color_mod() const
		{
			SDL_Color c;

			const auto r =
				SDL_GetTextureColorMod(texture(), &c.r, &c.g, &c.b) + SDL_GetTextureAlphaMod(texture(), &c.a);
			ASSERT(r == 0, SDL_GetError());

			return c;
		}

		/**
		 * @brief Sets the blending used while rendering
		 * Blending indicates how the colors are displayed on the screen.
		 * @param b Blend mode used. https://wiki.libsdl.org/SDL_SetTextureBlendMode#blendMode
		 */
		auto blend_mode(const SDL_BlendMode &b) const -> void
		{
			const auto r = SDL_SetTextureBlendMode(texture(), b);
			ASSERT(r == 0, SDL_GetError());
		}
		/**
		 * @brief Gets the blending used
		 * @return SDL_BlendMode
		 */
		[[nodiscard]] auto blend_mode() const
		{
			SDL_BlendMode b;

			const auto r = SDL_GetTextureBlendMode(texture(), &b);
			ASSERT(r == 0, SDL_GetError());

			return b;
		}

	private:
		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture = nullptr;
	};

	template<template<typename> class... Ex>
	using ETexture = typename MixBuilder<Texture, Ex...>::type;

} // namespace ctl::sdl

#endif // !_CTL_SDL2_TEXTURE_

//	class ModdedTexture : public BaseTexture
//	{
//	public:
//		ModdedTexture(SDLWindow *engine)
//			: BaseTexture(engine) {}
//		ModdedTexture(ModdedTexture&& x) = default;
//		ModdedTexture(const ModdedTexture& x) = default;
//		ModdedTexture& operator=(ModdedTexture&& x) = default;
//		ModdedTexture& operator=(const ModdedTexture& x) = default;
//
//		//Exception: Fail on surface convert or texture creation
//		auto& load(SDL_Surface* s)
//		{
//			SDL_Surface* f_s = SDL_ConvertSurfaceFormat(s, SDL_GetWindowPixelFormat(m_win->window()), 0);
//			if (f_s == nullptr)
//				throw ctl::std::runtime_error(SDL_GetError(), Log::Sev::ERR0R);
//
//			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_GetWindowPixelFormat(m_win->window()),
// SDL_TEXTUREACCESS_STREAMING, f_s->w, f_s->h)); 			if (!m_texture) 				throw
// ctl::std::runtime_error(SDL_GetError(), Log::Sev::ERR0R);
//
//			lock();
//			memcpy(m_pixels, f_s->pixels, f_s->pitch * f_s->h);
//			unlock();
//
//			m_pixels = nullptr;
//
//			m_dim = { f_s->w, f_s->h };
//
//			SDL_FreeSurface(f_s);
//			SDL_FreeSurface(s);
//
//			return *this;
//		}
//
//		auto& load(const mth::Dim &wh, const SDL_TextureAccess &a)
//		{
//			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_PIXELFORMAT_RGBA8888, a, wh.w, wh.h));
//			if (!m_texture)
//				throw ctl::std::runtime_error(SDL_GetError(), Log::Sev::ERR0R);
//
//			m_dim = wh;
//
//			return *this;
//		}
//
//		void lock()
//		{
//			if (m_pixels != nullptr)
//				Log::note("Texture is already locked.", Log::Sev::WARNING);
//
//			if (SDL_LockTexture(m_texture.get(), nullptr, &m_pixels, &m_pitch) != 0)
//				throw ctl::std::runtime_error(SDL_GetError(), Log::Sev::ERR0R);
//		}
//
//		void unlock()
//		{
//			if (m_pixels == nullptr)
//				Log::note("Texture is already unlocked.", Log::Sev::WARNING);
//
//			SDL_UnlockTexture(m_texture.get());
//			m_pixels = nullptr;
//			m_pitch = 0;
//		}
//
//		auto& target()
//		{
//			if (SDL_SetRenderTarget(m_win->renderer(), m_texture.get()) != 0)
//				throw ctl::std::runtime_error(SDL_GetError(), Log::Sev::ERR0R);
//
//			return *this;
//		}
//
//		auto& cpyPixel(void* p)
//		{
//			if (m_pixels != nullptr)
//				memcpy(m_pixels, p, m_pitch * m_dim.h);
//
//			return *this;
//		}
//
//		template<typename T = void*>
//		constexpr auto pixels() { return static_cast<T>(m_pixels); }
//		template<typename T = void*>
//		constexpr auto pixel(const NumVec<Uint32, 2> &xy)
//		{
//			const auto &p = static_cast<T>(m_pixels);
//			return p[(xy[0] * m_pitch / 4) + xy[1]];
//		}
//
//		constexpr const auto& pitch() const { return m_pitch; }
//		constexpr auto& pitch() { return m_pitch; }
//		constexpr auto pixelCount() const { return m_pitch / 4 * m_dim.h; }
//
//	private:
//		void *m_pixels = nullptr;
//		int m_pitch = 0;
//	};
//}