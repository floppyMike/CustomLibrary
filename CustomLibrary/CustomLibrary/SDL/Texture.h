#pragma once

#include <memory>

#include "Engine.h"
#include "SDLWindow.h"

namespace ctl
{
	class BaseTexture
	{
		struct Unique_Destructor { void operator()(SDL_Texture* t) { SDL_DestroyTexture(t); } };

	public:
		BaseTexture() = default;
		BaseTexture(BaseTexture&& x) = default;
		BaseTexture(const BaseTexture& x) = delete;

		BaseTexture& operator=(BaseTexture&& x) = default;
		BaseTexture& operator=(const BaseTexture& x) = delete;

		BaseTexture(SDLWindow* win) 
			: m_win(win) 
		{
		}

		void render(const SDLPoint& p, const double& angle, const SDL_RendererFlip& flip, const SDL_Point* const center = nullptr, const SDL_Rect* const clip = nullptr) const
		{
			SDLRect renderQuad = { m_win->cam.worldToScreen(p), m_dim };

			if (clip != nullptr)
				renderQuad.w = clip->w,
				renderQuad.h = clip->h;

			if (SDL_RenderCopyEx(m_win->renderer(), m_texture.get(), clip, &renderQuad, angle, center, flip) < 0)
				throw Log(SDL_GetError(), Log::Sev::ERR0R);
		}
		void render(const SDLPoint& p, const SDLRect* const clip = nullptr) const
		{
			SDLRect renderQuad = { m_win->cam.worldToScreen(p), m_dim };

			if (clip != nullptr)
				renderQuad.w = clip->w,
				renderQuad.h = clip->h;

			if (SDL_RenderCopy(m_win->renderer(), m_texture.get(), clip, &renderQuad) < 0)
				throw Log(SDL_GetError(), Log::Sev::ERR0R);
		}

		auto& colourMod(const SDL_Color &c) 
		{ 
			if (SDL_SetTextureColorMod(m_texture.get(), c.r, c.g, c.b) != 0)
				throw Log(SDL_GetError());

			return *this; 
		}
		auto colourMod() const
		{ 
			NumVec<Uint8, 3> c;

			const auto result = SDL_GetTextureColorMod(m_texture.get(), &c[0], &c[1], &c[2]);
			assert(result == 0 && "Texture not valid.");

			return c; 
		}

		auto& blendMode(const SDL_BlendMode &b) 
		{ 
			if (SDL_SetTextureBlendMode(m_texture.get(), b) != 0)
				throw Log(SDL_GetError());

			return *this; 
		}
		auto blendMode() const
		{ 
			SDL_BlendMode b;

			const auto result = SDL_GetTextureBlendMode(m_texture.get(), &b);
			assert(result == 0 && "Texture not valid.");

			return b; 
		}

		auto& alphaMod(const Uint8& a) 
		{ 
			if (SDL_SetTextureAlphaMod(m_texture.get(), a) != 0)
				throw Log(SDL_GetError());

			return *this; 
		}
		auto alphaMod() const
		{ 
			Uint8 a;

			const auto result = SDL_GetTextureAlphaMod(m_texture.get(), &a);
			assert(result == 0 && "Texture not valid.");

			return a; 
		}

		constexpr const auto& dim() const noexcept { return m_dim; }

		constexpr const auto& window() const noexcept { return m_win; }
		constexpr auto& window(SDLWindow* win) noexcept { m_win = win; return *this; }

	protected:
		SDLWindow* m_win = nullptr;

		std::unique_ptr<SDL_Texture, Unique_Destructor> m_texture;
		SDLDim m_dim = { 0, 0 };
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

		void load(SDL_Surface* surface, const SDL_Color* key = nullptr)
		{
			SDL_SetSurfaceRLE(surface, SDL_TRUE);

			if (key != nullptr)
				SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, key->r, key->g, key->b));

			m_dim = { surface->w, surface->h };

			m_texture.reset(SDL_CreateTextureFromSurface(m_win->renderer(), surface));
			assert(m_texture && "Surface is a nullptr.");

			SDL_FreeSurface(surface);
		}
	};

	class ModdedTexture : public BaseTexture
	{
	public:
		ModdedTexture(SDLWindow *engine)
			: BaseTexture(engine) {}
		ModdedTexture(ModdedTexture&& x) = default;
		ModdedTexture(const ModdedTexture& x) = default;
		ModdedTexture& operator=(ModdedTexture&& x) = default;
		ModdedTexture& operator=(const ModdedTexture& x) = default;

		//Exception: Fail on surface convert or texture creation
		auto& load(SDL_Surface* s)
		{
			SDL_Surface* f_s = SDL_ConvertSurfaceFormat(s, SDL_GetWindowPixelFormat(m_win->window()), 0);
			if (f_s == nullptr)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_GetWindowPixelFormat(m_win->window()), SDL_TEXTUREACCESS_STREAMING, f_s->w, f_s->h));
			if (!m_texture)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			lock();

			memcpy(m_pixels, f_s->pixels, f_s->pitch * f_s->h);
			unlock();

			m_pixels = nullptr;

			m_dim = { f_s->w, f_s->h };

			SDL_FreeSurface(f_s);
			SDL_FreeSurface(s);

			return *this;
		}

		auto& load(const SDLDim &wh, const SDL_TextureAccess &a)
		{
			m_texture.reset(SDL_CreateTexture(m_win->renderer(), SDL_PIXELFORMAT_RGBA8888, a, wh.w, wh.h));
			if (!m_texture)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			m_dim = wh;

			return *this;
		}

		void lock()
		{
			if (m_pixels != nullptr)
				Log::note("Texture is already locked.", Log::Sev::WARNING);

			if (SDL_LockTexture(m_texture.get(), nullptr, &m_pixels, &m_pitch) != 0)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);
		}

		void unlock()
		{
			if (m_pixels == nullptr)
				Log::note("Texture is already unlocked.", Log::Sev::WARNING);

			SDL_UnlockTexture(m_texture.get());
			m_pixels = nullptr;
			m_pitch = 0;
		}

		auto& target() 
		{ 
			if (SDL_SetRenderTarget(m_win->renderer(), m_texture.get()) != 0) 
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R); 

			return *this;
		}

		auto& cpyPixel(void* p)
		{
			if (m_pixels != nullptr)
				memcpy(m_pixels, p, m_pitch * m_dim.h);

			return *this;
		}

		template<typename T = void*>
		constexpr auto pixels() { return static_cast<T>(m_pixels); }
		template<typename T = void*>
		constexpr auto pixel(const NumVec<Uint32, 2> &xy) 
		{ 
			const auto &p = static_cast<T>(m_pixels); 
			return p[(xy[0] * m_pitch / 4) + xy[1]]; 
		}

		constexpr const auto& pitch() const { return m_pitch; }
		constexpr auto& pitch() { return m_pitch; }
		constexpr auto pixelCount() const { return m_pitch / 4 * m_dim.h; }

	private:
		void *m_pixels = nullptr;
		int m_pitch = 0;
	};
}