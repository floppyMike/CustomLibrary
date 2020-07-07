#ifndef _CTL_SDL2_Text_
#define _CTL_SDL2_Text_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../BasicTypes.h"
#include "../Error.h"

#include "Texture.h"
#include "Loader.h"

#include <cassert>

namespace ctl::sdl
{
	class Font
	{
		struct Unique_Deleter
		{
			void operator()(TTF_Font *f) { TTF_CloseFont(f); }
		};

	public:
		using base_t = Font;
		using tag_t	 = tag::isFont;

		Font()		  = default;
		Font(Font &&) = default;
		auto operator=(Font &&) -> Font & = default;

		auto font() noexcept -> auto *
		{
			assert(m_ptr && "Font is not loaded.");
			return m_ptr.get();
		}

		auto font(TTF_Font *f) noexcept -> auto &
		{
			m_ptr.reset(f);
			return *this;
		}

		void style(int style) { TTF_SetFontStyle(font(), style); }

		auto style() { return TTF_GetFontStyle(font()); }

		auto hypo_size(const char *text)
		{
			mth::Dim<int> temp;
			TTF_SizeUTF8(font(), text, &temp.w, &temp.h);
			return temp;
		}

	private:
		std::unique_ptr<TTF_Font, Unique_Deleter> m_ptr;
	};

	template<template<typename> class... Ex>
	using EFont = typename MixBuilder<Font, Ex...>::type;

	class Text : public Texture
	{
	public:
		using base_t = Texture;
		using tag_t	 = tag::isText;

		Text() = default;

		using Texture::Texture;

		auto text(SDL_Texture *t, const char *text) -> auto &
		{
			assert(t != nullptr && "Texture shouldn't be a nullptr.");
			this->texture(t);
			m_text = text;

			return *this;
		}

		[[nodiscard]] constexpr auto text() const noexcept -> const auto & { return m_text; }

	private:
		std::string m_text;
	};

	template<template<typename> class... Ex>
	using EText = typename MixBuilder<Text, Ex...>::type;

} // namespace ctl::sdl

#endif // !_CTL_SDL2_Text_
