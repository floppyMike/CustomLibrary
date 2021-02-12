#if not defined _CTL_SDL2_RENDERER_
#define _CTL_SDL2_RENDERER_

#include "SDL_init.h"

#include "../Error.h"
#include "../BasicTypes.h"

#include "TypeTraits.h"

#include <memory>

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
		auto color(const SDL_Color &col) const noexcept { SDL_SetRenderDrawColor(get(), col.r, col.g, col.b, col.a); }

		/**
		 * @brief Set the logical size of the rendering
		 * The logical size dictates how the draw area is displayed relative to the screen size. In other words it
		 * automaticaly scales based on Windows dimensions.
		 * @param dim Dimension to render on
		 */
		void logical_size(const mth::Dim<int> &dim) const noexcept { SDL_RenderSetLogicalSize(get(), dim.w, dim.h); }

		/**
		 * @brief Sets the blending used for drawing
		 * Blending indicates how the colors are displayed on the screen.
		 */
		auto blend_mode(const SDL_BlendMode &b) const noexcept
		{
			const auto r = SDL_SetRenderDrawBlendMode(get(), b);
			ASSERT(r == 0, SDL_GetError());
		}

		/**
		 * @brief Gets the blending used for drawing
		 * Blending indicates how the colors are displayed on the screen.
		 * @return Blend mode used. https://wiki.libsdl.org/SDL_SetTextureBlendMode#blendMode
		 */
		[[nodiscard]] auto blend_mode() const noexcept
		{
			SDL_BlendMode b;
			const auto	  r = SDL_GetRenderDrawBlendMode(get(), &b);
			ASSERT(r == 0, SDL_GetError());

			return b;
		}

		/**
		 * @brief Set the viewport for the renderer
		 * The viewport dictates what part of the world screen gets rendered onto the screen
		 * @param rect Size and location of the viewport
		 */
		auto viewport(const mth::Rect<int, int> &rect) const noexcept
		{
			const auto r = SDL_RenderSetViewport(get(), reinterpret_cast<const SDL_Rect *>(&rect));
			ASSERT(r == 0, SDL_GetError());
		}

		/**
		 * @brief Get the viewport for the renderer
		 * The viewport dictates what part of the world screen gets rendered onto the screen
		 * @return Rectange of the viewport
		 */
		[[nodiscard]] auto viewport() const noexcept
		{
			mth::Rect<int, int> rect;
			SDL_RenderGetViewport(get(), reinterpret_cast<SDL_Rect *>(&rect));

			return rect;
		}

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

#endif