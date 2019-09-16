#pragma once

#include "BasicTypes.h"
#include "SDLTraits.h"
#include <CustomLibrary/Error.h>

#include <SDL_ttf.h>

#include <cassert>

namespace ctl
{
	namespace sdl
	{
		class Font
		{
			struct Unique_Deleter { void operator()(TTF_Font* f) { TTF_CloseFont(f); } };

		public:
			Font() = default;
			Font(Font&&) = default;
			Font& operator=(Font&&) = default;

			TTF_Font* get() noexcept
			{
				return m_ptr.get();
			}

			Font& reset(TTF_Font* f) noexcept
			{
				m_ptr.reset(f);
				return *this;
			}

			Font& style(int style)
			{
				assert(!m_ptr && "Font is not loaded.");
				TTF_SetFontStyle(m_ptr.get(), style);
				return *this;
			}

			int style()
			{
				assert(!m_ptr && "Font is not loaded.");
				return TTF_GetFontStyle(m_ptr.get());
			}

		private:
			std::unique_ptr<TTF_Font, Unique_Deleter> m_ptr;
		};


		template<typename ImplFont = Font>
		class FontLoad : public ReliesOn<ImplFont, FontLoad<ImplFont>>
		{
			using base1 = ReliesOn<ImplFont, FontLoad>;

		public:
			ImplFont& load(const char* path, int pt)
			{
				auto* temp = TTF_OpenFont(path, pt);
				if (temp == nullptr)
					throw ctl::Log(SDL_GetError());

				return this->get<ImplFont>()->reset(temp);
			}

			using base1::set;
		};


		template<typename ImplFont = Font>
		class TextLoad : public ReliesOn<ImplFont, TextLoad<ImplFont>>
		{
			using base1 = ReliesOn<ImplFont, TextLoad>;

			SDL_Surface* _load_(SDL_Surface* s, const char* text)
			{
				if (s == nullptr)
					throw Log(SDL_GetError());

				m_text = text;
				return s;
			}

		public:
			SDL_Surface* loadSolid(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Solid(this->get<ImplFont>()->get(), text, colour), text);
			}

			SDL_Surface* loadShaded(const char* text, const SDL_Color& bg, const SDL_Color& colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Shaded(this->get<ImplFont>()->get(), text, colour, bg), text);
			}

			SDL_Surface* loadBlended(const char* text, const SDL_Color& colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Blended(this->get<ImplFont>()->get(), text, colour), text);
			}

			SDL_Surface* loadWrapped(const char* text, const Uint16& wrapper, const SDL_Color& colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Blended_Wrapped(this->get<ImplFont>()->get(), text, colour, wrapper), text);
			}

			template<typename T, typename... Args>
			SDL_Surface* load(const char* text, T(*loader)(Args...), Args&&... args)
			{
				static_assert(std::is_same_v<T, SDL_Surface*>, "Return type must be of type SDL_Surface.");
				return _load_(loader(std::forward<Args>(args)...), text);
			}

			constexpr const std::string& text() const { return m_text; }

			using base1::set;

		private:
			std::string m_text;
		};


		template<typename ImplFont>
		class FontAttrib : public ReliesOn<ImplFont, FontAttrib<ImplFont>>
		{
			using base1 = ReliesOn<ImplFont, FontAttrib>;

		public:
			Dim<int> hypoSize(const char* text)
			{
				Dim<int> temp;

				if (TTF_SizeUTF8(this->get<ImplFont>()->get(), text, &temp.w, &temp.h) != 0)
					throw Log(SDL_GetError());

				return temp;
			}

			using base1::set;
		};

	}
}