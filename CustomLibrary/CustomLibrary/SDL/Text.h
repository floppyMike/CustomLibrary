#pragma once

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
		Font(const Font&) = default;
		Font(Font&&) = default;

		Font& operator=(Font&&) = default;
		Font& operator=(const Font&) = default;

		//Exception: If file doesn't exit
		auto& load(const std::string& path, const int& pt)
		{
			m_ptr.reset(TTF_OpenFont(path.c_str(), pt));
			if (!m_ptr)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			m_pt = pt;

			return *this;
		}

		//Exception: If font not loaded
		//Font Styles: https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_22.html
		auto& style(const int& style)
		{
			if (!m_ptr)
				throw ctl::Log("Font: Font not loaded.", Log::Sev::ERR0R);

			TTF_SetFontStyle(m_ptr.get(), style);

			return *this;
		}
		//Exception: If font not loaded
		auto style()
		{
			if (!m_ptr)
				throw ctl::Log("Font: Font not loaded.", Log::Sev::ERR0R);

			return TTF_GetFontStyle(m_ptr.get());
		}

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
		Text(const Text&) = default;

		Text(SDLWindow* win)
			: FixedTexture(win)
		{
		}

		Text& operator=(Text&& x) = default;
		Text& operator=(const Text& x) = default;

		//Exception: If font has a problem
		auto& loadSolid(const std::string& text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			_load_(TTF_RenderUTF8_Solid(m_font->ptr(), text.c_str(), colour), text);
			return *this;
		}
		//Exception: If font has a problem
		auto& loadShaded(const std::string& text, const SDL_Color& bg, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			_load_(TTF_RenderUTF8_Shaded(m_font->ptr(), text.c_str(), colour, bg), text);
			return *this;
		}
		//Exception: If font has a problem
		auto& loadBlended(const std::string& text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			_load_(TTF_RenderUTF8_Blended(m_font->ptr(), text.c_str(), colour), text);
			return *this;
		}
		//Exception: If font has a problem
		auto& loadWrapped(const std::string& text, const Uint16& wrapper, const SDL_Color& colour = { 0, 0, 0, 0xFF })
		{
			_load_(TTF_RenderUTF8_Blended_Wrapped(m_font->ptr(), text.c_str(), colour, wrapper), text);
			return *this;
		}

		//Exception: If font has a problem
		auto hypoSize(const std::string &text)
		{
			NumVec<int, 2> temp;
			if (TTF_SizeUTF8(m_font->ptr(), text.c_str(), &temp[0], &temp[1]) != 0)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);
			return temp;
		}

		constexpr const std::string& textString() const { return m_text; }

		constexpr auto& font(Font* font) { m_font = font; return *this; }
		constexpr const auto& font() const { return m_font; }

	private:
		using FixedTexture::load;

		void _load_(SDL_Surface* s, const std::string& text)
		{
			if (s == nullptr)
				throw ctl::Log(SDL_GetError(), Log::Sev::ERR0R);

			load(s);
			m_text = text;
		}

		std::string m_text;
		Font *m_font = nullptr;
	};
}