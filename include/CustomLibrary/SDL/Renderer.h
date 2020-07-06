#pragma once

#include <SDL2/SDL.h>

#include "../Error.h"
#include "../Traits.h"
#include "../Dim.h"

#include "Engine.h"
#include "TypeTraits.h"
#include "Options.h"
#include "Render.h"

#include <cassert>

namespace ctl::sdl
{
	class Renderer
	{
		struct Unique_Des
		{
			void operator()(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
		};

	public:
		using base_t = Renderer;
		using tag_t	 = tag::isRenderer;

		template<typename ImplWin>
		Renderer(ImplWin *win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			create(win, rendererFlags);
		}

		Renderer(const Renderer &)		= delete;
		Renderer(Renderer &&r) noexcept = default;

		template<typename ImplWin>
		void create(ImplWin *win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			if (auto *r = SDL_CreateRenderer(win->get(), -1, rendererFlags); r)
				m_renderer.reset(r);
			else
				throw std::runtime_error(SDL_GetError());
		}

		void destroy() { m_renderer.reset(); }

		[[nodiscard]] auto get() const noexcept -> auto *
		{
			assert(m_renderer && "Renderer isn't loaded.");
			return m_renderer.get();
		}

		auto option() { return Option<std::decay_t<decltype(*this)>>(this); }
		auto render() { return Render<std::decay_t<decltype(*this)>>(this); }

	private:
		std::unique_ptr<SDL_Renderer, Unique_Des> m_renderer;
	};

	template<typename T>
	class LDelayedRenderer : public T
	{
	public:
		using base_t = T;
		using tag_t	 = typename tag::isRenderDelay;

		using T::T;

		void do_render(bool r) { m_do_render = r; }
		auto will_render() const noexcept { return m_do_render; }
		auto render() { return Render<std::decay_t<decltype(*this)>>(this); }

	private:
		bool m_do_render = true;
	};

	template<template<typename> class... Ex>
	using LRenderer = MixBuilder<Renderer, Ex...>;

} // namespace ctl::sdl
