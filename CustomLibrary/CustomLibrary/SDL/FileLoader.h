#pragma once

#include "../Tags.h"
#include "../Traits.h"

#include <SDL.h>

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _Loader_ {};

		template<typename Impl>
		class _Loader_<Impl, tag::isTexture> : public crtp<Impl, _Loader_, tag::isTexture>
		{
		public:
			Impl& load_texture(SDL_Surface* surface)
			{
				Impl* const pthis = this->underlying();
				return pthis->texture(SDL_CreateTextureFromSurface(pthis->renderer()->get(), surface));
			}

			Impl& load_texture(const char* path)
			{
				Impl* const pthis = this->underlying();
				return pthis->texture(IMG_LoadTexture(pthis->renderer()->get(), path));
			}

			Impl& load_texture(void* src, int size)
			{
				Impl* const pthis = this->underlying();
				return pthis->texture(IMG_LoadTexture_RW(pthis->renderer()->get(), SDL_RWFromMem(src, size), 1));
			}
		};

		template<typename Impl>
		class _Loader_<Impl, tag::isFont> : public crtp<Impl, _Loader_, tag::isFont>
		{
		public:
			Impl& load_font(const char* path, int pt)
			{
				Impl* const pthis = this->underlying();

				auto* temp = TTF_OpenFont(path, pt);
				assert(temp != nullptr && "Nothing found at path.");

				return pthis->font(temp);
			}
		};
	}

	template<typename Impl, typename... T>
	struct ELoader : detail::_Loader_<Impl, T>...
	{
		using detail::_Loader_<Impl, T>::_Loader_...;
	};
}