#pragma once

#include "../Tags.h"
#include "../Traits.h"

#include "TypeTraits.h"

#include <SDL.h>

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _ELoader_
		{};

		template<typename Impl>
		class _ELoader_<Impl, tag::isTexture> : public crtp<Impl, _ELoader_, tag::isTexture>
		{
		public:
			Impl& load_texture(SDL_Surface* surface)
			{
				Impl* const pthis = this->underlying();
				return pthis->texture(SDL_CreateTextureFromSurface(pthis->renderer()->get(), surface));
			}

			Impl& load_texture(const char* path)
			{
				auto& pthis = this->underlying()->ref();
				pthis.texture(IMG_LoadTexture(pthis.renderer()->get(), path));
				return *this->underlying();
			}

			Impl& load_texture(void* src, int size)
			{
				Impl* const pthis = this->underlying();
				return pthis->texture(IMG_LoadTexture_RW(pthis->renderer()->get(), SDL_RWFromMem(src, size), 1));
			}
		};

		template<typename Real, typename Tag>
		class _Loader_ 
			: public _Func_<Real, _Loader_<Real, Tag>>
			, public _UnPeeler_<_ELoader_, _Loader_<Real, Tag>, Real>
		{
		public:
			using _Func_<Real, _Loader_>::_Func_;
		};


		template<typename Real>
		class _Loader_<Real, tag::isFont>
			: public _Func_<Real, _Loader_<Real, tag::isFont>>
			, public _UnPeeler_<_Loader_, _Loader_<Real, tag::isFont>, Real>
		{
		public:
			using _Func_<Real, _Loader_>::_Func_;

			Real& load_font(const char* path, int pt)
			{
				auto* temp = TTF_OpenFont(path, pt);
				assert(temp != nullptr && "Nothing found at path.");
				this->m_o->font(temp);

				return *this;
			}
		};

#ifdef SDL_MIXER_H_
		template<typename Real>
		class _Loader_<Real, tag::isMusic>
			: public _Func_<Real, _Loader_<Real, tag::isMusic>>
			, public _UnPeeler_<_Loader_, _Loader_<Real, tag::isMusic>, Real>
		{
		public:
			using _Func_<Real, _Loader_>::_Func_;

			Real& load_music(std::string_view path)
			{
				if (Mix_Music* temp = Mix_LoadMUS(path.data()); temp)
					this->m_o->music(temp);
				else
					throw std::runtime_error(Mix_GetError());

				return *this;
			}
		};
#endif // SDL_MIXER_H_
		
	}

	template<typename T>
	using Load = detail::_Loader_<T, typename T::base_t::tag_t>;
}