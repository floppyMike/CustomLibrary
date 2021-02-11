#pragma once

#include <SDL2/SDL.h>

#include "../Traits.h"
#include "../Rect.h"

#include <cassert>

namespace ctl::sdl
{
	class Renderer;

	// clang-format off
	template<typename T>
	concept uses_sdl_shape = requires(T t)
	{
		{ t.shape() } -> same_as<mth::Rect<int, int> &, const mth::Rect<int, int> &>;
		t.shape(mth::Rect<int, int>());
	};

	template<typename T>
	concept uses_sdl_shape_const = requires(T t)
	{
		{ t.shape() } -> std::same_as<const mth::Rect<int, int> &>;
		t.shape(mth::Rect<int, int>());
	};

	template<typename T>
	concept uses_sdl_renderer = requires(T t)
	{
		{ t.renderer()->get() } -> std::same_as<SDL_Renderer *>;
	};
	// clang-format on

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
		constexpr explicit FunctorR(T *o)
			: Functor<T>(o)
		{
		}
		constexpr explicit FunctorR(Renderer *r)
			: m_r(r)
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
