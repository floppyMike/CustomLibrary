#pragma once

#include "TypeTraits.h"
#include "../Traits.h"

#include <SDL.h>

namespace ctl::sdl
{
	namespace detail
	{
		template<typename Impl, typename Tag>
		class _ERender_
		{
		public:
			
		};

		template<typename Impl>
		class _ERender_<Impl, tag::isRenderDelay> : public crtp<Impl, _ERender_, tag::isRenderDelay>
		{
		public:
			_ERender_() = default;

			void locking_render()
			{
				auto* const pthis = this->underlying();

				pthis->render();
				pthis->ref().do_render(false);
			}
		};



		template<typename Real, typename Tag>
		class _Render_
			: public _Func_<Real, _Render_<Real, Tag>>
			, public _UnPeeler_<_ERender_, _Render_<Real, Tag>, Real>
		{
		public:
			using _Func_<Real, _Render_>::_Func_;
		};

		template<typename Real>
		class _Render_<Real, tag::isRenderer>
			: public _Func_<Real, _Render_<Real, tag::isRenderer>>
			, public _UnPeeler_<_ERender_, _Render_<Real, tag::isRenderer>, Real>
		{
		public:
			using _Func_<Real, _Render_<Real, tag::isRenderer>>::_Func_;

			void fill(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->m_o->get(), col.r, col.g, col.b, col.a);
				SDL_RenderClear(this->m_o->get());
			}

			void render()
			{
				SDL_RenderPresent(this->m_o->get());
			}
		};


	}

	template<typename T>
	using Render = detail::_Render_<T, typename T::base_t::tag_t>;
}