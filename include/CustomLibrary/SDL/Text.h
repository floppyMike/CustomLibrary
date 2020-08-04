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
	// -----------------------------------------------------------------------------
	// Font
	// -----------------------------------------------------------------------------

	/**
	 * @brief Manages a font object
	 */
	class Font
	{
		struct Unique_Deleter
		{
			void operator()(TTF_Font *f) { TTF_CloseFont(f); }
		};

	public:
		Font()		  = default;
		Font(Font &&) = default;
		auto operator=(Font &&) -> Font & = default;

		Font(const Font &) noexcept = delete;
		auto operator=(const Font &) noexcept = delete;

		/**
		 * @brief Get the managed font ptr
		 * @return TTF_Font*
		 */
		auto font() noexcept
		{
			assert(m_ptr && "Font is not loaded.");
			return m_ptr.get();
		}

		/**
		 * @brief Change the managed font
		 * @param f new font
		 */
		auto font(TTF_Font *f) noexcept { m_ptr.reset(f); }

		/**
		 * @brief Set a font stype
		 * @param style Style to use https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_22.html
		 */
		void style(int style) { TTF_SetFontStyle(font(), style); }

		/**
		 * @brief Get the font stype used
		 * @return Font Style
		 */
		auto style() { return TTF_GetFontStyle(font()); }

		/**
		 * @brief Get the hypertheoretical text dimension
		 *
		 * @param text Text to measure on
		 * @return Dimension
		 */
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

	// -----------------------------------------------------------------------------
	// Text
	// -----------------------------------------------------------------------------

	/**
	 * @brief Manages a text
	 */
	class Text : public Texture
	{
	public:
		using base = Texture;

		Text() = default;

		using Texture::Texture;

		/**
		 * @brief Changes the texture with its associated text
		 *
		 * @param t Texture ptr
		 * @param text Text
		 */
		auto text(SDL_Texture *t, const char *text)
		{
			this->texture(t);
			m_text = text;
		}

		/**
		 * @brief Get the text
		 * @return const char *
		 */
		[[nodiscard]] constexpr auto text() const noexcept -> const auto & { return m_text; }

	private:
		std::string m_text;
	};

	template<template<typename> class... Ex>
	using EText = typename MixBuilder<Text, Ex...>::type;

} // namespace ctl::sdl

#endif // !_CTL_SDL2_Text_
