#pragma once

#include <SDL2/SDL.h>

#include "../Traits.h"
#include "../BasicTypes.h"

#include <cassert>
#include <tuple>
#include <type_traits>
#include <concepts>

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// Functor Extension (Mixin)
	// -----------------------------------------------------------------------------

	template<typename T, typename Renderer>
	class FunctorR : public Functor<T>
	{
	public:
        using Functor<T>::Functor;

		constexpr FunctorR() = default;
		constexpr FunctorR(T *o, Renderer *r)
			: Functor<T>(o), m_r(r)
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
