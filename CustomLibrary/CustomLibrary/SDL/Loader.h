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
		class _Loader_
		{};

		template<typename Impl>
		class _Loader_<Impl, tag::isTexture>
			: public crtp<Impl, _Loader_, tag::isTexture>
		{
		public:
			Impl& load_texture(SDL_Surface* surface)
			{
				auto* pthis = this->underlying()->pthis();
				pthis->texture(SDL_CreateTextureFromSurface(pthis->renderer()->get(), surface));
				return *this->underlying();
			}

			Impl& load_texture(const char* path)
			{
				auto* pthis = this->underlying()->pthis();
				pthis->texture(IMG_LoadTexture(pthis->renderer()->get(), path));
				return *this->underlying();
			}

			Impl& load_texture(void* src, int size)
			{
				auto* pthis = this->underlying()->pthis();
				pthis->texture(IMG_LoadTexture_RW(pthis->renderer()->get(), SDL_RWFromMem(src, size), 1));
				return *this->underlying();
			}
		};

		template<typename Impl>
		class _Loader_<Impl, tag::isFont>
			: public crtp<Impl, _Loader_, tag::isFont>
		{
		public:
			auto& load_font(const char* path, int pt)
			{
				auto* temp = TTF_OpenFont(path, pt);
				assert(temp != nullptr && "Nothing found at path.");
				this->underlying()->pthis()->font(temp);

				return *this;
			}
		};

#ifdef SDL_MIXER_H_
		template<typename Impl>
		class _Loader_<Impl, tag::isMusic>
			: public crtp<Impl, _Loader_, tag::isMusic>
		{
		public:
			auto& load_music(std::string_view path)
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
	class Load
		: public detail::_UnPeeler_<detail::_Loader_, Load<T>, T>
	{
	public:
		Load() = default;
		Load(T* o)
			: m_o(o)
		{
		}

		auto* pthis() noexcept { return m_o; }
		auto& pthis(T* o) noexcept { m_o = o; return *this; }

	private:
		T* m_o;
	};
}