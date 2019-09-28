#pragma once

#include "SDLTraits.h"
#include "Surface.h"
#include "Renderer.h"

#include <CustomLibrary/Traits.h>

#include <SDL_image.h>

namespace ctl::sdl
{
	template<typename ImplRend, template<typename> class... Func>
	class basicTexture : Renderable<ImplRend, basicTexture<ImplRend, Func...>>, 
		public Func<basicTexture<ImplRend, Func...>>...
	{
		struct Unique_Destructor { void operator()(SDL_Texture* t) { SDL_DestroyTexture(t); } };

		using base1 = Renderable<ImplRend, basicTexture<ImplRend, Func...>>;

	public:
		using base1::base1;

		basicTexture() noexcept = default;
		basicTexture(basicTexture&& t) noexcept = default;
		basicTexture& operator=(basicTexture&& t) noexcept = default;

		SDL_Texture* get() const noexcept
		{
			return m_texture.get();
		}

		basicTexture& replace(SDL_Texture* tex) noexcept
		{
			m_texture.reset(tex);
			return *this;
		}

		using base1::renderer;

	private:
		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
	};

	template<template<typename> class... Func>
	using Texture = basicTexture<Renderer, Func...>;


	template<typename ImplTex>
	class TexRend : public Shapeable<Rect<int, int>, ImplTex>, public crtp<ImplTex, TexRend>
	{
		using base1 = Shapeable<Rect<int, int>, ImplTex>;

	public:
		ImplTex& resetSize()
		{
			if (SDL_QueryTexture(this->_().get(), nullptr, nullptr, &this->m_shape.w, &this->m_shape.h) != 0)
				throw Log(SDL_GetError());

			return this->_();
		}

		void draw() const
		{
			if (SDL_RenderCopy(this->_().renderer()->get(), this->_().get(), nullptr, this->m_shape.rectPtr()) < 0)
				throw Log(SDL_GetError());
		}

		using base1::shape;
	};


	template<typename ImplTex>
	class TexLoad : public crtp<ImplTex, TexLoad>
	{
		template<typename T>
		using has_resetSize = decltype(std::declval<T&>().resetSize());

	public:
		template<typename ImplSur>
		ImplTex& load(ImplSur&& surface)
		{
			auto* tex = SDL_CreateTextureFromSurface(this->_().renderer()->get(), surface);
			if (!tex)
				throw Log(SDL_GetError());

			return this->_().replace(tex);
		}

		ImplTex& load(const char* path)
		{
			static_assert(is_detected_v<has_resetSize, ImplTex>, "resetSize is required for setting the renderer size.");

			auto* data = IMG_LoadTexture(this->_().renderer()->get(), path);
			if (data == nullptr)
				throw Log(SDL_GetError());

			this->_().replace(data);
			this->_().resetSize();

			return this->_();
		}

		ImplTex& load(void* src, int size)
		{
			return this->_().replace(IMG_LoadTexture_RW(this->_().renderer()->get(), SDL_RWFromMem(src, size), 1));
		}
		
	};


	template<typename ImplTex>
	class TexAttrib : public crtp<ImplTex, TexAttrib>
	{
	public:
		auto& colourMod(Uint8 r, Uint8 g, Uint8 b)
		{
			if (SDL_SetTextureColorMod(this->_().get(), r, g, b) != 0)
				throw Log(SDL_GetError());

			return *this;
		}
		auto colourMod() const
		{
			std::tuple<Uint8, Uint8, Uint8> c;

			if (SDL_GetTextureColorMod(this->_().get(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
				throw Log(SDL_GetError());

			return c;
		}

		auto& blendMode(const SDL_BlendMode& b)
		{
			if (SDL_SetTextureBlendMode(this->_().get(), b) != 0)
				throw Log(SDL_GetError());

			return *this;
		}
		SDL_BlendMode blendMode() const
		{
			SDL_BlendMode b;

			if (SDL_GetTextureBlendMode(this->_().get(), &b) != 0)
				throw Log(SDL_GetError());

			return b;
		}

		auto& alphaMod(const Uint8& a)
		{
			if (SDL_SetTextureAlphaMod(this->_().get(), a) != 0)
				throw Log(SDL_GetError());

			return *this;
		}
		Uint8 alphaMod() const
		{
			Uint8 a;

			if (SDL_GetTextureAlphaMod(this->_().get(), &a) == -1)
				throw Log(SDL_GetError());

			return a;
		}
	};

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