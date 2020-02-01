#pragma once

#include "Surface.h"
#include "Renderer.h"

#include "../Traits.h"
#include "Geometry.h"

#include <SDL_image.h>

namespace ctl::sdl
{
	template<template<typename, typename...> class... Ex>
	class TextureFrame : RectFrame<>, public Ex<TextureFrame<Ex...>, tag::isTexture, tag::isRect>...
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
			assert(tex && "SDL_Texture is a nullptr.");

			m_texture.reset(tex);
			reset_shape();

			return *this;
		}

		auto& reset_shape()
		{
			if (SDL_QueryTexture(m_texture.get(), nullptr, nullptr, &this->m_shape.w, &this->m_shape.h) != 0)
				throw err::Log(SDL_GetError());
			return *this;
		}
		
		using RectFrame<>::renderer;
		using RectFrame<>::shape;
		using RectFrame<>::translate;
		using RectFrame<>::pos;

	private:
		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
	};


	template<typename Impl, typename... T>
	class ETextureRender : public crtp<Impl, ETextureRender, T...>
	{
		static_assert(tag::has_tag_v<tag::isTexture, T...>, "Parent must be a texture.");

	public:
		void draw(const SDL_Rect* blit = nullptr) const
		{
			const Impl* const cpthis = this->underlying();

			if (SDL_RenderCopy(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr()) < 0)
				throw err::Log(SDL_GetError());
		}

		void draw(double angle, const mth::Point<int>& center, SDL_RendererFlip flip, const SDL_Rect* blit = nullptr) const
		{
			const Impl* const cpthis = this->underlying();

			if (SDL_RenderCopyEx(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr(), angle, center.point_ptr(), flip) < 0)
				throw err::Log(SDL_GetError());
		}

		auto& color_mod(Uint8 r, Uint8 g, Uint8 b)
		{
			Impl* const pthis = this->underlying();

			if (SDL_SetTextureColorMod(pthis->texture(), r, g, b) != 0)
				throw err::Log(SDL_GetError());

			return *this;
		}
		auto color_mod() const
		{
			const Impl* const cpthis = this->underlying();

			std::tuple<Uint8, Uint8, Uint8> c;

			if (SDL_GetTextureColorMod(cpthis->texture(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
				throw err::Log(SDL_GetError());

			return c;
		}

		auto& blend_mode(const SDL_BlendMode& b)
		{
			Impl* const pthis = this->underlying();

			if (SDL_SetTextureBlendMode(pthis->texture(), b) != 0)
				throw err::Log(SDL_GetError());

			return *this;
		}
		SDL_BlendMode blend_mode() const
		{
			const Impl* const cpthis = this->underlying();

			SDL_BlendMode b;

			if (SDL_GetTextureBlendMode(cpthis->texture(), &b) != 0)
				throw err::Log(SDL_GetError());

			return b;
		}

		auto& alpha_mod(const Uint8& a)
		{
			Impl* const pthis = this->underlying();

			if (SDL_SetTextureAlphaMod(pthis->texture(), a) != 0)
				throw err::Log(SDL_GetError());

			return *this;
		}
		Uint8 alpha_mod() const
		{
			const Impl* const cpthis = this->underlying();

			Uint8 a;

			if (SDL_GetTextureAlphaMod(cpthis->texture(), &a) == -1)
				throw err::Log(SDL_GetError());

			return a;
		}
	};


	template<typename Impl, typename... T>
	class ETextureLoader : public crtp<Impl, ETextureLoader, T...>
	{
		static_assert(tag::has_tag_v<tag::isTexture, T...>, "Parent must be a texture.");

	public:
		Impl& load(SDL_Surface* surface)
		{
			Impl* const pthis = this->underlying();
			return pthis->texture(SDL_CreateTextureFromSurface(pthis->renderer()->get(), surface));
		}

		Impl& load(const char* path)
		{
			Impl* const pthis = this->underlying();
			return pthis->texture(IMG_LoadTexture(pthis->renderer()->get(), path));
		}

		Impl& load(void* src, int size)
		{
			Impl* const pthis = this->underlying();
			return pthis->texture(IMG_LoadTexture_RW(pthis->renderer()->get(), SDL_RWFromMem(src, size), 1));
		}
	};



	using Texture = TextureFrame<ETextureLoader, ETextureRender>;

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
//				throw ctl::err::Log(SDL_GetError(), Log::Sev::ERR0R);
//
//			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_GetWindowPixelFormat(m_win->window()), SDL_TEXTUREACCESS_STREAMING, f_s->w, f_s->h));
//			if (!m_texture)
//				throw ctl::err::Log(SDL_GetError(), Log::Sev::ERR0R);
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
//				throw ctl::err::Log(SDL_GetError(), Log::Sev::ERR0R);
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
//				throw ctl::err::Log(SDL_GetError(), Log::Sev::ERR0R);
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
//				throw ctl::err::Log(SDL_GetError(), Log::Sev::ERR0R); 
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