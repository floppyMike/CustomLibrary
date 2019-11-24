#pragma once

#include <CustomLibrary/Error.h>
#include <CustomLibrary/Traits.h>
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
				throw ctl::Log(SDL_GetError());

			SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
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
	class ERendererDefault : public crtp<Impl, ERendererDefault>
	{
	public:
		void logical_size(const Dim<int>& dim)
		{
			assert(this->_().get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderSetLogicalSize(this->_().get(), dim.w, dim.h);
		}

		void color(const SDL_Color& col)
		{
			assert(this->_().get() != nullptr && "Renderer isn't loaded.");
			SDL_SetRenderDrawColor(this->_().get(), col.r, col.g, col.b, col.a);
		}

		void fill()
		{
			assert(this->_().get() != nullptr && "Renderer isn't loaded.");
			SDL_RenderClear(this->_().get());
		}
	};

	template<typename Impl>
	class ERendererRender : public crtp<Impl, ERendererRender>
	{
	public:
		void request_render()
		{
			m_do_render = true;
		}

		void render()
		{
			assert(this->_().get() != nullptr && "Renderer isn't loaded.");
			if (m_do_render)
			{
				SDL_RenderPresent(this->_().get());
				m_do_render = false;
			}
		}

	private:
		bool m_do_render = true;
	};


	using Renderer = basicRenderer<ERendererDefault, ERendererRender>;
}