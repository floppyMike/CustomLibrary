#pragma once

#include "../BasicTypes.h"
#include "Texture.h"
#include "../Error.h"

#include <SDL_ttf.h>

#include <cassert>

namespace ctl::sdl
{
	class Font
	{
		struct Unique_Deleter { void operator()(TTF_Font* f) { TTF_CloseFont(f); } };

	public:
		using base_t = Font;
		using tag_t = tag::isFont;

		Font() = default;
		Font(Font&&) = default;
		Font& operator=(Font&&) = default;

		constexpr auto* font() noexcept
		{
			assert(m_ptr && "Font is not loaded.");
			return m_ptr.get();
		}

		auto& font(TTF_Font* f) noexcept
		{
			m_ptr.reset(f);
			return *this;
		}

		auto& style(int style)
		{
			TTF_SetFontStyle(font(), style);
			return *this;
		}

		auto style()
		{
			return TTF_GetFontStyle(font());
		}

		auto hypo_size(const char* text)
		{
			mth::Dim<int> temp;
			TTF_SizeUTF8(font(), text, &temp.w, &temp.h);
			return temp;
		}

	private:
		std::unique_ptr<TTF_Font, Unique_Deleter> m_ptr;
	};


	template<typename Impl, typename... T>
	class ETextureFromText : public crtp<Impl, ETextureFromText, T...>
	{
		//static_assert(tag::has_tag_v<tag::isTexture, T...>, "Parent must be a texture.");

		auto& _load_(SDL_Surface* s, const char* text)
		{
			Impl* const pthis = this->underlying();
			assert(s != nullptr && "Font isn't assigned.");

			pthis->texture(SDL_CreateTextureFromSurface(pthis->renderer()->get(), s));
			SDL_FreeSurface(s);

			m_text = text;
			return *pthis;
		}

	public:
		auto& font(TTF_Font* f) noexcept
		{
			m_font = f;
			return *this->underlying();
		}
		auto* font() noexcept
		{
			assert(m_font && "Font no assigned.");
			return m_font;
		}

		Impl& load_solid(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Solid(m_font, text, colour), text);
		}

		Impl& load_shaded(const char* text, const SDL_Color& bg, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Shaded(m_font, text, colour, bg), text);
		}

		Impl& load_blended(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Blended(m_font, text, colour), text);
		}

		Impl& load_wrapped(const char* text, const Uint16& wrapper, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			return _load_(TTF_RenderUTF8_Blended_Wrapped(m_font, text, colour, wrapper), text);
		}

		constexpr const auto& text() const noexcept { return m_text; }

	private:
		TTF_Font* m_font = nullptr;
		std::string m_text;
	};

}