#pragma once

#include <SDL2/SDL.h>

#include "../Error.h"
#include "../Traits.h"
#include "../Dim.h"

#include "Engine.h"
#include "TypeTraits.h"
#include "Render.h"

#include <cassert>

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// Renderer
	// -----------------------------------------------------------------------------

	/**
	 * @brief Manages a Renderer Instance
	 */
	class Renderer
	{
		struct Unique_Des
		{
			void operator()(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
		};

	public:
		/**
		 * @brief Initialize the renderer
		 *
		 * @tparam ImplWin Window type
		 * @param win Window ptr
		 * @param rendererFlags SDL_Renderer Flags https://wiki.libsdl.org/SDL_RendererFlags#Values
		 */
		template<typename ImplWin>
		explicit Renderer(ImplWin *win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			create(win, rendererFlags);
		}

		Renderer(const Renderer &)		= delete;
		Renderer(Renderer &&r) noexcept = default;

		/**
		 * @brief Initialize the renderer
		 *
		 * @tparam ImplWin Window type
		 * @param win Window ptr
		 * @param rendererFlags SDL_Renderer Flags https://wiki.libsdl.org/SDL_RendererFlags#Values
		 */
		template<typename ImplWin>
		void create(ImplWin *win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			if (auto *const r = SDL_CreateRenderer(win->get(), -1, rendererFlags); r)
				m_renderer.reset(r);
			else
				throw std::runtime_error(SDL_GetError());
		}

		/**
		 * @brief Deinitialize the renderer
		 */
		void destroy() { m_renderer.reset(); }

		/**
		 * @brief Get SDL_Renderer ptr
		 * @return SDL_Renderer ptr
		 */
		[[nodiscard]] auto get() const noexcept
		{
			assert(m_renderer && "Renderer isn't loaded.");
			return m_renderer.get();
		}

		/**
		 * @brief Set the draw color
		 * @param col new Color
		 */
		auto color(const SDL_Color &col) -> void
		{
			SDL_SetRenderDrawColor(m_renderer.get(), col.r, col.g, col.b, col.a);
		}

		/**
		 * @brief Set the logical size of the rendering
		 * The logical size dictates how the draw area is displayed relative to the screen size. In other words it
		 * automaticaly scales based on Windows dimensions.
		 * @param dim Dimension to render on
		 */
		void logical_size(const mth::Dim<int> &dim) { SDL_RenderSetLogicalSize(m_renderer.get(), dim.w, dim.h); }

	private:
		std::unique_ptr<SDL_Renderer, Unique_Des> m_renderer;
	};

	// -----------------------------------------------------------------------------
	// Delayed Renderer Extension
	// -----------------------------------------------------------------------------

	/**
	 * @brief Enables the renderer to skip draw cycles when nothing changed.
	 * @tparam T base
	 */
	template<typename T>
	class Delayed : public T
	{
	public:
		using base = T;

		using T::T;

		/**
		 * @brief Enables/Disables the rendering
		 * @param r should render
		 */
		void do_render(bool r) { m_do_render = r; }
		/**
		 * @brief Checks if renderer wants to render
		 * @return bool
		 */
		auto will_render() const noexcept { return m_do_render; }

	private:
		bool m_do_render = true;
	};

	template<template<typename> class... Ex>
	using ERenderer = typename MixBuilder<Renderer, Ex...>::type;

} // namespace ctl::sdl
