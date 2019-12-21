#pragma once

#include "BasicTypes.h"
#include "Texture.h"
#include <CustomLibrary/Error.h>

#include <SDL_ttf.h>

#include <cassert>

namespace ctl::sdl
{
	template<template<typename> class... Func>
	class basicFont : public Func<basicFont<Func...>>...
	{
		struct Unique_Deleter { void operator()(TTF_Font* f) { TTF_CloseFont(f); } };

	public:
		basicFont() = default;
		basicFont(basicFont&&) = default;
		basicFont& operator=(basicFont&&) = default;

		auto* get() noexcept
		{
			assert(m_ptr && "Font is not loaded.");
			return m_ptr.get();
		}

		auto& reset(TTF_Font* f) noexcept
		{
			m_ptr.reset(f);
			return *this;
		}

	private:
		std::unique_ptr<TTF_Font, Unique_Deleter> m_ptr;
	};


	template<typename ImplFont>
	class EFontPathLoader : public crtp<ImplFont, EFontPathLoader>
	{
	public:
		auto& load(const char* path, int pt)
		{
			auto* temp = TTF_OpenFont(path, pt);
			assert(temp != nullptr && "Nothing found at path.");

			return this->_().reset(temp);
		}
	};


	template<typename ImplFont>
	class EFontAttrib : public crtp<ImplFont, EFontAttrib>
	{
	public:
		auto& style(int style)
		{
			TTF_SetFontStyle(this->_().get(), style);
			return this->_();
		}

		auto style()
		{
			return TTF_GetFontStyle(this->_().get());
		}

		auto hypoSize(const char* text)
		{
			Dim<int> temp;
			TTF_SizeUTF8(this->_().get(), text, &temp.w, &temp.h);
			return temp;
		}
	};


	using Font = basicFont<EFontPathLoader, EFontAttrib>;


	template<typename ImplTex>
	class ETextureTextLoader : public crtp<ImplTex, ETextureTextLoader>
	{
		template<typename T>
		using has_load = decltype(std::declval<T&>().load(std::declval<SDL_Surface*>()));

		auto& _load_(SDL_Surface* s, const char* text)
		{
			assert(s != nullptr && "Font isn't assigned.");

			static_assert(ctl::is_detected_v<has_load, ImplTex>, "\"load\" required for turning SDL_Surface to Texture");

			this->_().load(s);
			SDL_FreeSurface(s);

			m_text = text;
			return this->_();
		}

	public:
		auto& font(TTF_Font* f) noexcept
		{
			m_font = f;
			return this->_();
		}
		auto* font() noexcept
		{
			return m_font;
		}

		ImplTex& load_solid(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Solid(m_font, text, colour), text);
		}

		ImplTex& load_shaded(const char* text, const SDL_Color& bg, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Shaded(m_font, text, colour, bg), text);
		}

		ImplTex& load_blended(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Blended(m_font, text, colour), text);
		}

		ImplTex& load_wrapped(const char* text, const Uint16& wrapper, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Blended_Wrapped(m_font, text, colour, wrapper), text);
		}

		constexpr const auto& text() const noexcept { return m_text; }

	private:
		TTF_Font* m_font = nullptr;
		std::string m_text;
	};


	using Text = TextureFrame<ETexLoad, ETextureTextLoader, ETexRend, ETexAttrib>;

}