#pragma once

#include "SDLTraits.h"
#include "Surface.h"
#include "Renderer.h"

#include <CustomLibrary/Traits.h>
#include "Geometry.h"

#include <SDL_image.h>

namespace ctl::sdl
{
	template<template<typename> class... Ex>
	class TextureFrame : RectFrame<>, public Ex<TextureFrame<Ex...>>...
	{
		struct Unique_Destructor { void operator()(SDL_Texture* t) { SDL_DestroyTexture(t); } };

	public:
		using RectFrame<>::Frame;
		TextureFrame() = default;

		SDL_Texture* texture() const noexcept
		{
			return m_texture.get();
		}

		auto& texture(SDL_Texture* tex) noexcept
		{
			m_texture.reset(tex);
			return *this;
		}
		
		using RectFrame<>::renderer;
		using RectFrame<>::shape;
		using RectFrame<>::translate;
		using RectFrame<>::pos;

	private:
		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
	};


	template<typename ImplTex>
	class ETexRend : public crtp<ImplTex, ETexRend>
	{
	public:
		void draw() const
		{
			if (SDL_RenderCopy(this->_().renderer()->get(), this->_().texture(), nullptr, this->_().shape().rectPtr()) < 0)
				throw Log(SDL_GetError());
		}
	};


	template<typename ImplTex>
	class ETexLoad : public crtp<ImplTex, ETexLoad>
	{
		void _reset_size_()
		{
			Rect<int, int> size = this->_().shape();
			if (SDL_QueryTexture(this->_().texture(), nullptr, nullptr, &size.w, &size.h) != 0)
				throw Log(SDL_GetError());
			this->_().shape(size);
		}

		ImplTex& _load_(SDL_Texture* tex)
		{
			if (!tex)
				throw Log(SDL_GetError());

			this->_().texture(tex);
			_reset_size_();

			return this->_();
		}

	public:
		ImplTex& load(SDL_Surface* surface)
		{
			return _load_(SDL_CreateTextureFromSurface(this->_().renderer()->get(), surface));
		}

		ImplTex& load(const char* path)
		{
			return _load_(IMG_LoadTexture(this->_().renderer()->get(), path));
		}

		ImplTex& load(void* src, int size)
		{
			return _load_(IMG_LoadTexture_RW(this->_().renderer()->get(), SDL_RWFromMem(src, size), 1));
		}
		
	};


	template<typename ImplTex>
	class ETexAttrib : public crtp<ImplTex, ETexAttrib>
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


	using Texture = TextureFrame<ETexLoad, ETexRend, ETexAttrib>;

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