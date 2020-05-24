#pragma once

#include "../Tags.h"
#include "../Traits.h"
#include "Renderer.h"
#include "TypeTraits.h"
#include <SDL.h>

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _LoaderR_
		{
		};

#ifdef SDL_IMAGE_H_
		template<typename Impl>
		class _LoaderR_<Impl, tag::isTexture> : public crtp<Impl, _LoaderR_, tag::isTexture>
		{
		public:
			Impl &surface(SDL_Surface *surface)
			{
				auto *pthis = this->underlying();
				pthis->obj()->texture(SDL_CreateTextureFromSurface(pthis->renderer()->get(), surface));
				return *this->underlying();
			}

			Impl &file(const char *path)
			{
				auto *pthis = this->underlying();
				pthis->obj()->texture(IMG_LoadTexture(pthis->renderer()->get(), path));
				return *this->underlying();
			}

			Impl &bytes(void *src, int size)
			{
				auto *pthis = this->underlying();
				pthis->obj()->texture(IMG_LoadTexture_RW(pthis->renderer()->get(), SDL_RWFromMem(src, size), 1));
				return *this->underlying();
			}
		};
#endif // SDL_IMAGE_H_

#ifdef SDL_TTF_H_
		template<typename Impl>
		class _LoaderR_<Impl, tag::isText> : public crtp<Impl, _LoaderR_, tag::isText>
		{
			auto &_load_(SDL_Surface *s, const char *text)
			{
				auto *pthis = this->underlying();
				pthis->obj()->text(SDL_CreateTextureFromSurface(pthis->renderer()->get(), s), text);
				SDL_FreeSurface(s);

				return *pthis;
			}

		public:
			Impl &solid(TTF_Font *f, const char *text, const SDL_Color &colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Solid(f, text, colour), text);
			}

			Impl &shaded(TTF_Font *f, const char *text, const SDL_Color &bg,
						 const SDL_Color &colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Shaded(f, text, colour, bg), text);
			}

			Impl &blended(TTF_Font *f, const char *text, const SDL_Color &colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Blended(f, text, colour), text);
			}

			Impl &wrapped(TTF_Font *f, const char *text, const Uint16 &wrapper,
						  const SDL_Color &colour = { 0, 0, 0, 0xFF })
			{
				return _load_(TTF_RenderUTF8_Blended_Wrapped(f, text, colour, wrapper), text);
			}
		};
#endif // !SDL_TTF_H_

		template<typename, typename>
		class _LoaderO_
		{
		};

#ifdef SDL_MIXER_H_
		template<typename Impl>
		class _LoaderO_<Impl, tag::isMusic> : public crtp<Impl, _LoaderO_, tag::isMusic>
		{
		public:
			auto &file(std::string_view path)
			{
				if (Mix_Music *temp = Mix_LoadMUS(path.data()); temp)
					this->underlying()->obj()->music(temp);
				else
					throw std::runtime_error(Mix_GetError());

				return *this;
			}
		};
#endif // SDL_MIXER_H_

#ifdef SDL_TTF_H_
		template<typename Impl>
		class _LoaderO_<Impl, tag::isFont> : public crtp<Impl, _LoaderO_, tag::isFont>
		{
		public:
			auto &file(const char *path, int pt)
			{
				auto *temp = TTF_OpenFont(path, pt);
				assert(temp != nullptr && "Nothing found at path.");
				this->underlying()->obj()->font(temp);

				return *this;
			}
		};
#endif // !SDL_TTF_H_

	} // namespace detail

	template<typename T>
	using LoadR = FunctionalR<T, detail::_LoaderR_>;

	template<typename T>
	using LoadO = FunctionalR<T, detail::_LoaderO_>;
} // namespace ctl::sdl
