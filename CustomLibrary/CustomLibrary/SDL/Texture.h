#pragma once

#include <memory>

#include "Engine.h"
#include "SDLWindow.h"
#include "SDLTraits.h"

namespace ctl
{
	class BaseTexture : public Graphical
	{
		struct Unique_Destructor { void operator()(SDL_Texture* t) { SDL_DestroyTexture(t); } };

	public:
		BaseTexture() = default;
		BaseTexture(BaseTexture&& x) = default;
		BaseTexture(const BaseTexture& x) = delete;

		BaseTexture& operator=(BaseTexture&& x) = default;
		BaseTexture& operator=(const BaseTexture& x) = delete;

		/**
		* @summary construct empty Texture
		* @param "win" window ptr
		*/
		BaseTexture(SDLWindow* win)
			: Graphical(win)
		{
		}

		/**
		* @summary puts texture into renderer
		* @param "angle" angle at which to rotate the texture
		* @param "flip" determains if texture should be flipped
		* @param "center" determains the center of object (if nullptr then middle is taken)
		* @param "clip" clip of part that should only be rendered
		* @exception "Log" at fail
		*/
		void render(const double& angle, const SDL_RendererFlip& flip,
			const SDL_Point* const center = nullptr, const SDL_Rect* const clip = nullptr) const;

		/**
		* @summary puts texture into renderer
		* @param "clip" clip of part that should only be rendered
		* @exception "Log" at fail
		*/
		void render(const SDL_Rect* const clip = nullptr) const;

		/**
		* @summary colour modifiers
		*/
		BaseTexture& colourMod(const SDL_Color& c);
		NumVec<Uint8, 3> colourMod() const;

		BaseTexture& blendMode(const SDL_BlendMode& b);
		SDL_BlendMode blendMode() const;

		BaseTexture& alphaMod(const Uint8& a);
		Uint8 alphaMod() const;

	protected:
		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
	};

	class FixedTexture : public BaseTexture
	{
	public:
		FixedTexture() = default;
		FixedTexture(FixedTexture&& x) = default;
		FixedTexture(const FixedTexture& x) = default;

		FixedTexture& operator=(FixedTexture&& x) = default;
		FixedTexture& operator=(const FixedTexture& x) = default;

		FixedTexture(SDLWindow* engine)
			: BaseTexture(engine)
		{
		}

		/**
		* @summary loads texture from SDL_Surface
		* @param "surface" SDL_Surface to load from
		* @param "key" colour key
		* @exception "Log" at fail
		*/
		FixedTexture& load(SDL_Surface* surface, const SDL_Color* const key = nullptr);
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
//		auto& load(const SDLDim &wh, const SDL_TextureAccess &a)
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