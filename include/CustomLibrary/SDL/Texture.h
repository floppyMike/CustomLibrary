#if not defined _CTL_SDL2_TEXTURE_
#define _CTL_SDL2_TEXTURE_

#include "Surface.h"

namespace ctl::sdl
{
	namespace detail
	{
		/**
		 * @brief Handles deallocation of the Texture
		 */
		struct Texture_Destructor
		{
			void operator()(SDL_Texture *t)
			{
				SDL_DestroyTexture(t);
			}
		};
	}; // namespace detail

	using Texture = std::unique_ptr<SDL_Texture, detail::Texture_Destructor>;

	/**
	 * @brief Load a texture from a bmp file
	 *
	 * @param r SDL_Renderer
	 * @param file Path to bmp file
	 * @return Texture
	 */
	auto texture_from_bmp(SDL_Renderer *r, const char *file) -> Texture
	{
		Surface s(SDL_LoadBMP(file));
		return Texture(SDL_CreateTextureFromSurface(r, s.get()));
	}

	/**
	 * @brief Create a empty streamable texture (render target stays)
	 *
	 * @param r SDL_Renderer
	 * @param w Texture width
	 * @param h Texture height
	 * @param b Blendmode
	 * @return Texture
	 */
	auto create_empty(SDL_Renderer *r, int w, int h, SDL_BlendMode b = SDL_BLENDMODE_BLEND) noexcept -> sdl::Texture
	{
		auto t = sdl::Texture(SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h));

		SDL_SetTextureBlendMode(t.get(), b);

		SDL_SetRenderTarget(r, t.get());
		SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
		SDL_RenderClear(r);

		return t;
	}

	/**
	 * @brief Crop a texture (render target stays)
	 *
	 * @param r SDL_Renderer
	 * @param t Texture to crop
	 * @param box Area to crop out
	 * @return Texture
	 */
	auto crop(SDL_Renderer *r, const sdl::Texture &t, mth::Rect<int, int> box,
			  SDL_BlendMode b = SDL_BLENDMODE_BLEND) noexcept -> sdl::Texture
	{
		auto tex = sdl::Texture(SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, box.w, box.h));

		SDL_SetTextureBlendMode(tex.get(), b);

		SDL_SetRenderTarget(r, tex.get());
		SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
		SDL_RenderClear(r);
		SDL_RenderCopy(r, t.get(), &sdl::to_rect(box), nullptr);

		return tex;
	}

	// /**
	//  * @brief Container class for managing Textures
	//  */
	// class Texture
	// {
	// public:
	// 	using base = RectFrame;

	// 	Texture() = default;

	// 	/**
	// 	 * @brief Gives the Texture
	// 	 * @return Texture ptr
	// 	 */
	// 	[[nodiscard]] auto texture() const noexcept
	// 	{
	// 		assert(m_texture && "SDL_Texture is a nullptr.");
	// 		return m_texture.get();
	// 	}
	// 	/**
	// 	 * @brief Sets the Texture
	// 	 * @param tex New Texture
	// 	 */
	// 	auto texture(SDL_Texture *tex) noexcept -> void
	// 	{
	// 		assert(tex && "SDL_Texture is a nullptr.");

	// 		m_texture.reset(tex);
	// 		reset_shape();
	// 	}

	// 	/**
	// 	 * @brief Resets the width and height to default
	// 	 */
	// 	auto reset_shape() -> void
	// 	{
	// 		if (SDL_QueryTexture(texture(), nullptr, nullptr, &this->shape().w, &this->shape().h) != 0)
	// 			throw std::runtime_error(SDL_GetError());
	// 	}

	// 	/**
	// 	 * @brief Sets the color modulation affecting render operations
	// 	 * Color modulation affects the coloration of the outputted texture. Color / 255. * TextureColor = Output.
	// 	 * @param c Color to modulate with
	// 	 */
	// 	auto color_mod(const SDL_Color &c) const -> void
	// 	{
	// 		const auto re = SDL_SetTextureColorMod(texture(), c.r, c.g, c.b) + SDL_SetTextureAlphaMod(texture(), c.a);
	// 		ASSERT(re == 0, SDL_GetError());
	// 	}
	// 	/**
	// 	 * @brief Gets the color modulation used
	// 	 * @return SDL_Color
	// 	 */
	// 	[[nodiscard]] auto color_mod() const
	// 	{
	// 		SDL_Color c;

	// 		const auto r =
	// 			SDL_GetTextureColorMod(texture(), &c.r, &c.g, &c.b) + SDL_GetTextureAlphaMod(texture(), &c.a);
	// 		ASSERT(r == 0, SDL_GetError());

	// 		return c;
	// 	}

	// 	/**
	// 	 * @brief Sets the blending used while rendering
	// 	 * Blending indicates how the colors are displayed on the screen.
	// 	 * @param b Blend mode used. https://wiki.libsdl.org/SDL_SetTextureBlendMode#blendMode
	// 	 */
	// 	auto blend_mode(const SDL_BlendMode &b) const -> void
	// 	{
	// 		const auto r = SDL_SetTextureBlendMode(texture(), b);
	// 		ASSERT(r == 0, SDL_GetError());
	// 	}
	// 	/**
	// 	 * @brief Gets the blending used
	// 	 * @return SDL_BlendMode
	// 	 */
	// 	[[nodiscard]] auto blend_mode() const
	// 	{
	// 		SDL_BlendMode b;

	// 		const auto r = SDL_GetTextureBlendMode(texture(), &b);
	// 		ASSERT(r == 0, SDL_GetError());

	// 		return b;
	// 	}

	// private:
	// 	std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture = nullptr;
	// };

	// -----------------------------------------------------------------------------
	// Frame Class
	// -----------------------------------------------------------------------------

	// class TextureFrame
	// 	: public RectFrame
	// 	, public Texture
	// {
	// public:
	// 	using RectFrame::RectFrame;
	// 	using Texture::Texture;
	// };

	// -----------------------------------------------------------------------------
	// Procedures
	// -----------------------------------------------------------------------------

	// /**
	//  * @brief Loads texture from a file
	//  *
	//  * @param r renderer
	//  * @param t texture to load to
	//  * @param path path to the file
	//  */
	// void load_file(Renderer &r, Texture &t, const char *path) { t.texture(IMG_LoadTexture(r.get(), path)); }

	// // void load_surface()

	// /**
	//  * @brief Loads Texture from series of bytes
	//  *
	//  * @param r renderer
	//  * @param t texture to load to
	//  * @param src Byte start address
	//  * @param size Byte array size
	//  */
	// void load_bytes(Renderer &r, Texture &t, void *src, int size)
	// {
	// 	t.texture(IMG_Load_Texture_RW(r.get(), SDL_RWFromMem(src, size), 1));
	// }

	// /**
	//  * @brief Draws the texture onto the render buffer
	//  * @param r renderer
	//  * @param t texture to load to
	//  * @param blit Specifies a part of the texture to draw. Default is whole texture.
	//  */
	// void draw(Renderer &r, const TextureFrame &t, mth::Rect<int, int> s,
	// 		  const mth::Rect<int, int> *const blit = nullptr)
	// {
	// 	const auto r = SDL_RenderCopy(r.get(), t.texture(), reinterpret_cast<const SDL_Rect *>(blit),
	// 								  reinterpret_cast<const SDL_Rect *>(&this->obj()->shape()));
	// 	ASSERT(r == 0, SDL_GetError());
	// }

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
