#pragma once

#include <SDL_ttf.h>

#include <memory>
#include <string>

#include <CustomLibrary/utility.h>
#include <CustomLibrary/SDL/Texture.h>

namespace ctl
{
	class Font
	{
	public:
		struct Unique_Deleter { void operator()(TTF_Font* f) { TTF_CloseFont(f); } };

		Font() = default;
		Font(const Font&) = delete;
		Font(Font&&) = default;

		Font& operator=(Font&&) = default;
		Font& operator=(const Font&) = delete;

		/**
		* @summary loads font from path
		* @param "path" path where font file is located
		* @param "pt" font size
		* @exception "Log" at fail
		*/
		Font& load(const char* path, const int& pt);

		/**
		* @summary sets font style
		* @param "style" font style
		* @remarks a font must be loaded
		* @remarks font styles https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_22.html
		*/
		Font& style(const int& style);

		/**
		* @summary gets font style
		* @remarks a font must be loaded
		* @returns font style
		*/
		int style();

		auto* ptr() { return m_ptr.get(); }
		constexpr const auto& pt() const { return m_pt; }

	private:
		std::unique_ptr<TTF_Font, Unique_Deleter> m_ptr;
		unsigned int m_pt = 0;
	};

	class Text : public FixedTexture
	{
	public:
		Text() = default;
		Text(Text&& x) = default;
		Text(const Text&) = delete;

		Text(SDLWindow* win)
			: FixedTexture(win)
		{
		}

		Text& operator=(Text&& x) = default;
		Text& operator=(const Text& x) = delete;

		/**
		* @summary loads a text
		* @param "text" text to load
		* @param "colour" text colour
		* @exception "Log" at fail
		* @remarks fast load algorithm but the worst output
		*/
		Text& loadSolid(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF });
		/**
		* @summary loads a text
		* @param "text" text to load
		* @param "bg" background colour
		* @param "colour" text colour
		* @exception "Log" at fail
		* @remarks slow load algorithm but nicer and with background colour
		*/
		Text& loadShaded(const char* text, const SDL_Color& bg, const SDL_Color& colour = { 0, 0, 0, 0xFF });
		/**
		* @summary loads a text
		* @param "text" text to load
		* @param "colour" text colour
		* @exception "Log" at fail
		* @remarks slowest load algorithm but nicest
		*/
		Text& loadBlended(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF });
		/**
		* @summary loads a text
		* @param "text" text to load
		* @param "wrapper" size of line before newline
		* @param "colour" text colour
		* @exception "Log" at fail
		* @remarks same as blended, has a sizeable cache and supports newline
		*/
		Text& loadWrapped(const char* text, const Uint16& wrapper, const SDL_Color& colour = { 0, 0, 0, 0xFF });

		/**
		* @summary loads a text from custom function
		* @param "text" text to load
		* @param "loader" function to load from
		* @param "args" arguments for loader
		* @exception "Log" if loader returns a nullptr
		* @remarks Return type must be SDL_Surface
		*/
		template<typename T, typename... Args>
		Text& load(const char* text, T (*loader)(Args...), Args&&... args)
		{
			static_assert(std::is_same_v<T, SDL_Surface*>, "Return type must be of type SDL_Surface.");
			_load_(loader(std::forward<Args>(args)...), text);
			return *this;
		}

		/**
		* @summary gets graphical size of text
		* @param "text" text to see size of
		* @exception "Log" at fail
		* @returns Dim<int> of width and height
		*/
		Dim<int> hypoSize(const char* text);

		/**
		* @summary text accessor
		*/
		constexpr const std::string& text() const { return m_text; }

		/**
		* @summary font accessors
		*/
		constexpr auto& font(Font* font) { m_font = font; return *this; }
		constexpr const auto& font() const { return m_font; }

	private:
		using FixedTexture::load;

		/**
		* @summary load surface to texture
		* @param "s" surface to convert
		* @param "text" text to store as string
		* @exception "Log" at fail
		*/
		void _load_(SDL_Surface* s, const char* text);

		std::string m_text;
		Font *m_font = nullptr;
	};
}