#ifndef _CTL_SDL2_OPTIONS_
#define _CTL_SDL2_OPTIONS_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "TypeTraits.h"

#include "../Dim.h"

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _Option_
		{
		};

		template<typename Impl>
		class _Option_<Impl, tag::isRenderer> : public crtp<Impl, _Option_, tag::isRenderer>
		{
		public:
			auto &color(const SDL_Color &col)
			{
				SDL_SetRenderDrawColor(this->underlying()->obj()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			void logical_size(const mth::Dim<int> &dim)
			{
				SDL_RenderSetLogicalSize(this->underlying()->obj()->get(), dim.w, dim.h);
			}
		};

		template<typename Impl>
		class _Option_<Impl, tag::isFont> : public crtp<Impl, _Option_, tag::isFont>
		{
		public:
			void style(int style) { TTF_SetFontStyle(this->underlying()->obj()->font(), style); }

			auto style() { return TTF_GetFontStyle(this->underlying()->obj()->font()); }

			auto hypo_size(const char *text)
			{
				mth::Dim<int> temp;
				TTF_SizeUTF8(this->underlying()->obj()->font(), text, &temp.w, &temp.h);
				return temp;
			}
		};
	} // namespace detail

	template<typename T>
	using Option = FunctionalO<T, detail::_Option_>;

} // namespace ctl::sdl

#endif // !_CTL_SDL2_OPTIONS_
