#ifndef _CTL_SDL2_Text_
#define _CTL_SDL2_Text_

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

	class Text : public Texture
	{
	public:
		using base_t = Texture;
		using tag_t = tag::isText;

		Text() = default;

		using Texture::Texture;

		auto& text(SDL_Texture* t, const char* text)
		{
			assert(t != nullptr && "Texture shouldn't be a nullptr.");
			this->texture(t);
			m_text = text;

			return *this;
		}

		constexpr const auto& text() const noexcept { return m_text; }

	private:
		std::string m_text;
	};

}

#endif // !_CTL_SDL2_Text_