#pragma once

#include "BasicTypes.h"
#include "SDLTraits.h"
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

		TTF_Font* get() noexcept
		{
			assert(m_ptr && "Font is not loaded.");
			return m_ptr.get();
		}

		basicFont& reset(TTF_Font* f) noexcept
		{
			m_ptr.reset(f);
			return *this;
		}

	private:
		std::unique_ptr<TTF_Font, Unique_Deleter> m_ptr;
	};


	template<typename ImplFont>
	class FontLoad : public crtp<ImplFont, FontLoad>
	{
	public:
		ImplFont& load(const char* path, int pt)
		{
			auto* temp = TTF_OpenFont(path, pt);
			if (temp == nullptr)
				throw ctl::Log(SDL_GetError());

			return this->_().reset(temp);
		}
	};


	template<typename ImplFont>
	class FontAttrib : public crtp<ImplFont, FontAttrib>
	{
	public:
		ImplFont& style(int style)
		{
			TTF_SetFontStyle(this->_().get(), style);
			return this->_();
		}

		int style()
		{
			return TTF_GetFontStyle(this->_().get());
		}

		Dim<int> hypoSize(const char* text)
		{
			Dim<int> temp;

			if (TTF_SizeUTF8(this->_().get(), text, &temp.w, &temp.h) != 0)
				throw Log(SDL_GetError());

			return temp;
		}
	};


	using Font = basicFont<FontLoad, FontAttrib>;


	template<typename ImplTex>
	class TextLoad : public crtp<ImplTex, TextLoad>, public ReliesOn<TTF_Font, ImplTex>
	{
		using baseR = ReliesOn<TTF_Font, ImplTex>;

		template<typename T>
		using has_load = decltype(std::declval<T&>().load(std::declval<SDL_Surface*>()));

		ImplTex& _load_(SDL_Surface* s, const char* text)
		{
			if (s == nullptr)
				throw Log(SDL_GetError());

			static_assert(ctl::is_detected_v<has_load, ImplTex>, "\"load\" required for turning SDL_Surface to Texture");

			this->_().load(s);
			SDL_FreeSurface(s);

			m_text = text;
			return this->_();
		}

	public:
		ImplTex& loadSolid(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Solid(this->get<TTF_Font>(), text, colour), text);
		}

		ImplTex& loadShaded(const char* text, const SDL_Color& bg, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Shaded(this->get<TTF_Font>(), text, colour, bg), text);
		}

		ImplTex& loadBlended(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Blended(this->get<TTF_Font>(), text, colour), text);
		}

		ImplTex& loadWrapped(const char* text, const Uint16& wrapper, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Blended_Wrapped(this->get<TTF_Font>(), text, colour, wrapper), text);
		}

		constexpr const std::string& text() const noexcept { return m_text; }

		using baseR::set;

	private:
		std::string m_text;
	};


	using Text = basicTexture<Renderer, TexLoad, TextLoad, TexRend, TexAttrib>;

}