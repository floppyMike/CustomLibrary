#pragma once

#include "../Error.h"
#include "../Traits.h"
#include "../Dim.h"
#include <SDL.h>

#include <cassert>

namespace ctl::sdl
{
	template<template<typename> class... Ex>
	class basicRenderer : public Ex<basicRenderer<Ex...>>...
	{
	public:
		template<typename ImplWin>
		basicRenderer(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			if ((m_renderer = SDL_CreateRenderer(win->get(), -1, rendererFlags)) == nullptr)
				throw ctl::err::Log(SDL_GetError());

			SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}

		basicRenderer(const basicRenderer&) = delete;
		basicRenderer(basicRenderer&& r) noexcept
			: m_renderer(r.m_renderer)
		{
			r.m_renderer = nullptr;
		}

		~basicRenderer()
		{
			destroy();
		}

		void destroy()
		{
			if (m_renderer != nullptr)
				SDL_DestroyRenderer(m_renderer),
				m_renderer = nullptr;
		}

		constexpr auto* get() 
		{
			assert(m_renderer != nullptr && "Renderer isn't loaded.");
			return m_renderer; 
		}

	private:
		SDL_Renderer* m_renderer = nullptr;
	};


	template<typename Impl>
	class ERendererDefault
	{
		Impl* const pthis = static_cast<Impl*>(this);

	public:
		void logical_size(const mth::Dim<int>& dim)
		{
			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderSetLogicalSize(pthis->get(), dim.w, dim.h);
		}

		void color(const SDL_Color& col)
		{
			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_SetRenderDrawColor(pthis->get(), col.r, col.g, col.b, col.a);
		}

		void fill()
		{
			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderClear(pthis->get());
		}
	};

	template<typename Impl>
	class ERendererRender : public crtp<Impl, ERendererRender>
	{
		Impl* const pthis = static_cast<Impl*>(this);

	public:
		void do_render(bool r)
		{
			m_do_render = r;
		}

		bool will_render() const noexcept
		{
			return m_do_render;
		}

		void render()
		{
			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderPresent(pthis->get());
			m_do_render = false;
		}

	private:
		bool m_do_render = true;
	};


	using Renderer = basicRenderer<ERendererDefault, ERendererRender>;
}