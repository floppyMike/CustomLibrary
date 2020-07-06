#ifndef _CTL_SDL2_TEXTURE_
#	define _CTL_SDL2_TEXTURE_

#	include <SDL2/SDL.h>
#	include <SDL2/SDL_image.h>

#	include "Surface.h"
#	include "Renderer.h"

#	include "../Traits.h"

#	include "TypeTraits.h"
#	include "Geometry.h"
#	include "Drawable.h"
#	include "Loader.h"

namespace ctl::sdl
{
	class Texture : public RectFrame
	{
		struct Unique_Destructor
		{
			void operator()(SDL_Texture *t) { SDL_DestroyTexture(t); }
		};

	public:
		using tag_t	 = tag::isTexture;
		using base_t = RectFrame;

		using RectFrame::RectFrame;
		Texture() = default;

		[[nodiscard]] auto texture() const noexcept -> SDL_Texture * { return m_texture.get(); }

		auto reset_shape() -> auto &
		{
			if (SDL_QueryTexture(m_texture.get(), nullptr, nullptr, &this->shape().w, &this->shape().h) != 0)
				throw std::runtime_error(SDL_GetError());

			return *this;
		}

		auto texture(SDL_Texture *tex) noexcept -> auto &
		{
			assert(tex && "SDL_Texture is a nullptr.");

			m_texture.reset(tex);
			reset_shape();

			return *this;
		}

		auto load(Renderer *r) { return LoadR<std::decay_t<decltype(*this)>>(this, r); }
		auto option() { return Option<std::decay_t<decltype(*this)>>(this); }
		auto draw(Renderer *r) { return Draw<std::decay_t<decltype(*this)>>(this, r); }

	private:
		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
	};

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
//SDL_TEXTUREACCESS_STREAMING, f_s->w, f_s->h)); 			if (!m_texture) 				throw ctl::std::runtime_error(SDL_GetError(),
//Log::Sev::ERR0R);
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