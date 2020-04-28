#pragma once

#include "TypeTraits.h"
#include "../Traits.h"

#include <SDL.h>

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _Render_
		{			
		};

		template<typename Impl>
		class _Render_<Impl, tag::isRenderDelay> 
			: public crtp<Impl, _Render_, tag::isRenderDelay>
		{
		public:
			_Render_() = default;

			void locking_render()
			{
				auto* const pthis = this->underlying();

				pthis->render();
				pthis->obj()->do_render(false);
			}
		};

		template<typename Impl>
		class _Render_<Impl, tag::isRenderer>
			: public crtp<Impl, _Render_, tag::isRenderer>
		{
		public:
			void fill(const SDL_Color& col)
			{
				auto* pthis = this->underlying();

				SDL_SetRenderDrawColor(pthis->obj()->get(), col.r, col.g, col.b, col.a);
				SDL_RenderClear(pthis->obj()->get());
			}

			void render()
			{
				SDL_RenderPresent(this->underlying()->obj()->get());
			}
		};


	}

	template<typename T>
	class Render
		: public detail::_UnPeeler_<detail::_Render_, Render<T>, T>
	{
	public:
		Render() = default;
		Render(T* o)
			: m_o(o)
		{
		}

		auto* obj() const noexcept { return m_o; }
		auto& obj(T* o) noexcept { m_o = o; return *this; }

	private:
		T* m_o;
	};
}