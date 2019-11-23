#pragma once

#include <CustomLibrary/Error.h>
#include <SDL.h>

#include <cassert>

namespace ctl::sdl
{
	class Renderer
	{
	public:
		template<typename ImplWin>
		Renderer(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
		{
			if ((m_renderer = SDL_CreateRenderer(win->get(), -1, rendererFlags)) == nullptr)
				throw ctl::Log(SDL_GetError());

			SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}

		~Renderer()
		{
			destroy();
		}

		void destroy()
		{
			if (m_renderer != nullptr)
				SDL_DestroyRenderer(m_renderer),
				m_renderer = nullptr;
		}

		void color(const SDL_Color& col)
		{
			assert(m_renderer != nullptr && "Renderer isn't loaded.");
			SDL_SetRenderDrawColor(m_renderer, col.r, col.g, col.b, col.a);
		}

		void fill()
		{
			assert(m_renderer != nullptr && "Renderer isn't loaded.");
			SDL_RenderClear(m_renderer);
		}

		void render()
		{
			assert(m_renderer != nullptr && "Renderer isn't loaded.");
			SDL_RenderPresent(m_renderer);
		}

		void logical_size(const Dim<int>& dim)
		{
			assert(m_renderer != nullptr && "Renderer isn't loaded.");
			SDL_RenderSetLogicalSize(m_renderer, dim.w, dim.h);
		}

		constexpr auto* get() 
		{
			assert(m_renderer != nullptr && "Renderer isn't loaded.");
			return m_renderer; 
		}

	private:
		SDL_Renderer* m_renderer = nullptr;
	};

}