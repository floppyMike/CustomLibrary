#pragma once

#include <SDL2/SDL.h>

#include "TypeTraits.h"
#include "../Traits.h"

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// Predefinitions
	// -----------------------------------------------------------------------------

	class Renderer;

	template<typename>
	class Delayed;

	// -----------------------------------------------------------------------------
	// Render Implementation
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename, typename, typename>
		class _Render_;

		/**
		 * @brief Handles rendering of delayed renderer
		 */
		template<typename Full, typename Impl>
		class _Render_<Delayed<Nonesuch>, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Render object and automatically lock it
			 */
			void locking_render()
			{
				this->render();
				this->obj()->do_render(false);
			}
		};

		/**
		 * @brief Handles rendering of renderer
		 */
		template<typename Full, typename Impl>
		class _Render_<Renderer, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Fill the buffer with a color
			 * @param col color to use
			 */
			void fill(const SDL_Color &col)
			{
				SDL_SetRenderDrawColor(this->obj()->get(), col.r, col.g, col.b, col.a);
				SDL_RenderClear(this->obj()->get());
			}

			/**
			 * @brief Renders the buffer
			 */
			void render() { SDL_RenderPresent(this->obj()->get()); }
		};
	} // namespace detail

	// -----------------------------------------------------------------------------
	// Render Extension
	// -----------------------------------------------------------------------------

	/**
	 * @brief Type for rendering type construction
	 * @tparam T Object to Render for type
	 */
	template<typename T>
	using Render = typename Filter<detail::_Render_, FunctorO<T>, T>::type;

	/**
	 * @brief Shows rendering options for object
	 *
	 * @param ptr ptr to object
	 * @return Render type for further options
	 */
	template<typename _T>
	auto render(_T *const ptr)
	{
		return Render<_T>(ptr);
	}

} // namespace ctl::sdl