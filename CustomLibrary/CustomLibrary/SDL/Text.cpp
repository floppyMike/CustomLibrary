#include "Text.h"

ctl::Font& ctl::Font::load(const char* path, const int& pt)
{
	m_ptr.reset(TTF_OpenFont(path, pt));
	if (!m_ptr)
		throw ctl::Log(SDL_GetError());

	m_pt = pt;
	return *this;
}

ctl::Font& ctl::Font::style(const int& style)
{
	assert(!m_ptr && "Font is not loaded.");
	TTF_SetFontStyle(m_ptr.get(), style);

	return *this;
}

int ctl::Font::style()
{
	assert(!m_ptr && "Font is not loaded.");
	return TTF_GetFontStyle(m_ptr.get());
}

ctl::Text& ctl::Text::loadSolid(const char* text, const SDL_Color& colour)
{
	_load_(TTF_RenderUTF8_Solid(m_font->ptr(), text, colour), text);
	return *this;
}

ctl::Text& ctl::Text::loadShaded(const char* text, const SDL_Color& bg, const SDL_Color& colour)
{
	_load_(TTF_RenderUTF8_Shaded(m_font->ptr(), text, colour, bg), text);
	return *this;
}

ctl::Text& ctl::Text::loadBlended(const char* text, const SDL_Color& colour)
{
	_load_(TTF_RenderUTF8_Blended(m_font->ptr(), text, colour), text);
	return *this;
}

ctl::Text& ctl::Text::loadWrapped(const char* text, const Uint16& wrapper, const SDL_Color& colour)
{
	_load_(TTF_RenderUTF8_Blended_Wrapped(m_font->ptr(), text, colour, wrapper), text);
	return *this;
}

ctl::Dim<int> ctl::Text::hypoSize(const char* text)
{
	Dim<int> temp;

	if (TTF_SizeUTF8(m_font->ptr(), text, &temp.w, &temp.h) != 0)
		throw Log(SDL_GetError());

	return temp;
}

void ctl::Text::_load_(SDL_Surface* s, const char* text)
{
	if (s == nullptr)
		throw Log(SDL_GetError());

	load(s);
	m_text = text;
}
