#pragma once

#include "SDLTraits.h"
#include "Surface.h"

#include <SDL_image.h>

namespace ctl
{
	namespace sdl
	{
		class Texture
		{
			struct Unique_Destructor { void operator()(SDL_Texture* t) { SDL_DestroyTexture(t); } };

		public:
			Texture() noexcept = default;
			Texture(Texture&& t) noexcept = default;
			Texture& operator=(Texture&& t) noexcept = default;

			SDL_Texture* get() noexcept
			{
				return m_texture.get();
			}

			Texture& replace(SDL_Texture* tex)
			{
				m_texture.reset(tex);
				return *this;
			}

			Texture& colourMod(const SDL_Color& c)
			{
				if (SDL_SetTextureColorMod(m_texture.get(), c.r, c.g, c.b) != 0)
					throw Log(SDL_GetError());

				return *this;
			}
			auto colourMod() const
			{
				std::tuple<Uint8, Uint8, Uint8> c;

				if (SDL_GetTextureColorMod(m_texture.get(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
					throw Log(SDL_GetError());

				return c;
			}

			Texture& blendMode(const SDL_BlendMode& b)
			{
				if (SDL_SetTextureBlendMode(m_texture.get(), b) != 0)
					throw Log(SDL_GetError());

				return *this;
			}
			SDL_BlendMode blendMode() const
			{
				SDL_BlendMode b;

				if (SDL_GetTextureBlendMode(m_texture.get(), &b) != 0)
					throw Log(SDL_GetError());

				return b;
			}

			Texture& alphaMod(const Uint8& a)
			{
				if (SDL_SetTextureAlphaMod(m_texture.get(), a) != 0)
					throw Log(SDL_GetError());

				return *this;
			}
			Uint8 alphaMod() const
			{
				Uint8 a;

				if (SDL_GetTextureAlphaMod(m_texture.get(), &a) == -1)
					throw Log(SDL_GetError());

				return a;
			}

		private:
			std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
		};


		template<typename ImplTex = Texture, typename ImplRend = Renderer>
		class TextureRenderer : Object<ImplRend, Rect<int, int>>, ReliesOn<ImplTex>
		{
			using base1 = Object<ImplRend, Rect<int, int>>;
			using base2 = ReliesOn<ImplTex>;

		public:
			using base1::base1;

			TextureRenderer& resetSize()
			{
				if (SDL_QueryTexture(this->get<ImplTex>()->get(), nullptr, nullptr, &this->m_shape.w, &this->m_shape.h) != 0)
					throw Log(SDL_GetError());

				return *this;
			}

			TextureRenderer& set(ImplTex* tex)
			{
				base2::set(tex);
				return resetSize();
			}

			void draw() const
			{
				if (SDL_RenderCopy(this->m_rend->get(), this->get<ImplTex>()->get(), nullptr, this->m_shape.rectPtr()) < 0)
					throw Log(SDL_GetError());
			}

			using base1::renderer;
			using base1::shape;
		};


		template<typename ImplTex = Texture, typename ImplRend = Renderer>
		class TextureLoader : Renderable<ImplRend>, ReliesOn<ImplTex>
		{
			using base1 = Renderable<ImplRend>;
			using base2 = ReliesOn<ImplTex>;

		public:
			using base1::base1;

			template<typename ImplSur>
			ImplTex& load(ImplSur* surface)
			{
				auto* tex = SDL_CreateTextureFromSurface(this->m_rend->get(), surface);
				if (!tex)
					throw Log(SDL_GetError());

				return this->get<ImplTex>()->replace(tex);
			}

			ImplTex& load(const char* path)
			{
				return this->get<ImplTex>()->replace(IMG_LoadTexture(this->m_rend->get(), path));
			}
		
			using base1::renderer;
			using base2::set;
		};


		//class BaseTexture : public Object<Rect<short, int>>
		//{

		//public:
		//	BaseTexture() = default;
		//	BaseTexture(BaseTexture&& x) = default;

		//	BaseTexture& operator=(BaseTexture&& x) = default;

		//	/**
		//	* @summary construct empty Texture
		//	* @param "win" window ptr
		//	*/
		//	BaseTexture(SDLWindow* win)
		//		: Object(win)
		//	{
		//	}

		//	/**
		//	* @summary puts texture into renderer
		//	* @param "angle" angle at which to rotate the texture
		//	* @param "flip" determains if texture should be flipped
		//	* @param "center" determains the center of object (if nullptr then middle is taken)
		//	* @param "clip" clip of part that should only be rendered
		//	* @exception "Log" at fail
		//	*/
		//	void render(const double& angle, const SDL_RendererFlip& flip,
		//		const SDL_Point* const center = nullptr, const SDL_Rect* const clip = nullptr) const
		//	{
		//		SDL_Rect renderQuad = m_shape;

		//		if (clip != nullptr)
		//			renderQuad.w = clip->w,
		//			renderQuad.h = clip->h;

		//		if (SDL_RenderCopyEx(m_win->renderer(), m_texture.get(), clip, &renderQuad, angle, center, flip) < 0)
		//			throw Log(SDL_GetError());
		//	}

		//	/**
		//	* @summary puts texture into renderer
		//	* @param "clip" clip of part that should only be rendered
		//	* @exception "Log" at fail
		//	*/
		//	void render(const SDL_Rect* const clip = nullptr) const
		//	{
		//		SDL_Rect renderQuad = m_shape;

		//		if (clip != nullptr)
		//			renderQuad.w = clip->w,
		//			renderQuad.h = clip->h;

		//		if (SDL_RenderCopy(m_win->renderer(), m_texture.get(), clip, &renderQuad) < 0)
		//			throw Log(SDL_GetError());
		//	}

		//	/**
		//	* @summary colour modifiers
		//	*/


		//	constexpr const auto& dim() const { return m_dim; }

		//protected:
		//	ctl::Dim<int> m_dim;
		//};

		//class FixedTexture : public BaseTexture
		//{
		//public:
		//	FixedTexture() = default;
		//	FixedTexture(FixedTexture&& x) = default;

		//	FixedTexture& operator=(FixedTexture&& x) = default;

		//	FixedTexture(SDLWindow* engine)
		//		: BaseTexture(engine)
		//	{
		//	}

		//	/**
		//	* @summary loads texture from SDL_Surface
		//	* @param "surface" SDL_Surface to load from
		//	* @param "key" colour key
		//	* @exception "Log" at fail
		//	*/
		//	FixedTexture& load(SDL_Surface* surface, const SDL_Color* const key = nullptr)
		//	{
		//		if (key != nullptr)
		//			SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, key->r, key->g, key->b));

		//		m_dim = { surface->w, surface->h };
		//		m_shape.dim(m_dim);

		//		m_texture.reset(SDL_CreateTextureFromSurface(m_win->renderer(), surface));
		//		if (!m_texture)
		//			throw Log(SDL_GetError());

		//		SDL_FreeSurface(surface);

		//		return *this;
		//	}
		//};

	}
}























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
//				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);
//
//			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_GetWindowPixelFormat(m_win->window()), SDL_TEXTUREACCESS_STREAMING, f_s->w, f_s->h));
//			if (!m_texture)
//				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);
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
//		auto& load(const Dim &wh, const SDL_TextureAccess &a)
//		{
//			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_PIXELFORMAT_RGBA8888, a, wh.w, wh.h));
//			if (!m_texture)
//				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);
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
//				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);
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
//				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R); 
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