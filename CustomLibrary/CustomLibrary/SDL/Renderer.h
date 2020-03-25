#pragma once

#include "../Error.h"
#include "../Traits.h"
#include "../Dim.h"
#include <SDL.h>

#include <cassert>

namespace ctl::sdl
{
	template<template<typename, typename...> class... Ex>
	class basicRenderer : public Ex<basicRenderer<Ex...>, tag::isRenderer>...
	{
	public:
		template<typename ImplWin>
		basicRenderer(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			create(win, rendererFlags);
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

		template<typename ImplWin>
		void create(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			if ((m_renderer = SDL_CreateRenderer(win->get(), -1, rendererFlags)) == nullptr)
				throw std::runtime_error(SDL_GetError());
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


	template<typename Impl, typename... tags_t>
	class ERendererSettings : public crtp<Impl, ERendererSettings, tags_t...>
	{
		static_assert(tag::has_tag_v<tag::isRenderer, tags_t...>, "Parent must be a renderer.");

	public:
		void logical_size(const mth::Dim<int>& dim)
		{
			Impl* const pthis = this->underlying();

			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderSetLogicalSize(pthis->get(), dim.w, dim.h);
		}

		void color(const SDL_Color& col)
		{
			Impl* const pthis = this->underlying();

			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_SetRenderDrawColor(pthis->get(), col.r, col.g, col.b, col.a);
		}

		void fill()
		{
			Impl* const pthis = this->underlying();

			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			color(m_fill_col);
			SDL_RenderClear(pthis->get());
		}

		void fill_color(const SDL_Color& col) noexcept { m_fill_col = col; }
		const auto& fill_color() const noexcept { return m_fill_col; }

	private:
		SDL_Color m_fill_col = sdl::WHITE;
	};

	template<typename Impl, typename... tags_t>
	class ERendererRender : public crtp<Impl, ERendererRender, tags_t...>
	{
		static_assert(tag::has_tag_v<tag::isRenderer, tags_t...>, "Parent must be a renderer.");

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
			Impl* const pthis = this->underlying();

			assert(pthis->get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderPresent(pthis->get());
			m_do_render = false;
		}

	private:
		bool m_do_render = true;
	};


	using Renderer = basicRenderer<ERendererSettings, ERendererRender>;
}