#pragma once

#include <CustomLibrary/Error.h>
#include <SDL.h>

namespace ctl
{
	namespace sdl
	{
		template<typename ImplWin>
		class Renderer
		{
		public:
			Renderer(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
				: m_win(win)
			{
				if ((m_renderer = SDL_CreateRenderer(m_win->get(), -1, rendererFlags)) == nullptr)
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
					SDL_DestroyRenderer(m_renderer);
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

			void setLogicalSize(const SDLDim<int>& dim)
			{
				SDL_RenderSetLogicalSize(m_renderer, dim.w, dim.h);
			}

			constexpr auto* get() { return m_renderer; }

		private:
			ImplWin* m_win;
			SDL_Renderer* m_renderer = nullptr;
		};

	}
}