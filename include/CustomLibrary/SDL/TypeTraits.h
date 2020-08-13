#pragma once

#include <SDL2/SDL.h>

#include "../Traits.h"

#include <cassert>

namespace ctl::sdl
{
	class Renderer;

	template<typename T>
	concept uses_sdl_renderer = requires(T t)
	{
		{
			t.renderer()->get()
		}
		->std::same_as<SDL_Renderer *>;
	};

	// -----------------------------------------------------------------------------
	// Functor Extension (Mixin)
	// -----------------------------------------------------------------------------

	/**
	 * @brief Additionaly provides a renderer ptr to the base
	 * @tparam T Object type
	 * @tparam Renderer Renderer type
	 */
	template<typename T, typename Renderer>
	class FunctorR : public Functor<T>
	{
	public:
		using Functor<T>::Functor;

		constexpr FunctorR() = default;
		constexpr FunctorR(T *o, Renderer *r)
			: Functor<T>(o)
			, m_r(r)
		{
		}

		constexpr auto renderer(Renderer *const r) noexcept { m_r = r; }
		constexpr auto renderer() const noexcept
		{
			assert(m_r != nullptr && "Renderer isn't assigned.");
			return m_r;
		}

	private:
		Renderer *m_r = nullptr;
	};

} // namespace ctl::sdl
