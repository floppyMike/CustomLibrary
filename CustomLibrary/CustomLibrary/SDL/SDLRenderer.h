#pragma once

#include <CustomLibrary/Error.h>
#include <SDL.h>

namespace ctl
{
	namespace sdl
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

			void setColor(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(m_renderer, col.r, col.g, col.b, col.a);
			}

			void clear()
			{
				SDL_RenderClear(m_renderer);
			}

			void render()
			{
				SDL_RenderPresent(m_renderer);
			}

			void setLogicalSize(const Dim<int>& dim)
			{
				SDL_RenderSetLogicalSize(m_renderer, dim.w, dim.h);
			}

			constexpr auto* get() 
			{
				return m_renderer; 
			}

		private:
			SDL_Renderer* m_renderer = nullptr;
		};

	}
}