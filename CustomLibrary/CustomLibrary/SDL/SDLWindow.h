#pragma once

#include <CustomLibrary/Error.h> 

#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include "Camera2D.h"
#include "EventWatch.h"

namespace ctl
{
	namespace sdl
	{
		template<typename ImplState = StateBase, 
			typename ImplWatch = EventWatch,
			typename ImplCam = Camera2D,
			typename ImplWin = Window,
			typename ImplRend = Renderer>
		class SDLWindow
		{
		public:
			using state_t = ImplState;

			/**
			* @summary construct window and SDL renderer
			* @param "name" name of window
			* @param "dim" size of window
			* @param "windowFlags" window creation flags
			* @param "rendererFlags" SDL renderer creation flags
			* @exception "Log" if renderer creation fails
			* @remarks "windowFlags" check https://wiki.libsdl.org/SDL_CreateRenderer#Remarks
			* @remarks "rendererFlags" check https://wiki.libsdl.org/SDL_CreateRenderer#Remarks
			*/
			SDLWindow(const char* name,
				const Dim<int>& dim,
				const Uint32& windowFlags = SDL_WINDOW_SHOWN,
				const Uint32& rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
				: m_win(name, dim, windowFlags)
				, m_renderer(&m_win, rendererFlags)
			{
				
			}

			~SDLWindow()
			{
				destroy();
			}

			void destroy()
			{
				m_win.destroy();
				m_renderer.destroy();
			}

			template<typename State, typename ...Args>
			ImplState& setState(Args&&... args)
			{
				m_state = std::make_unique<State>(&m_renderer, std::forward<Args>(args)...);
				return *m_state;
			}

			SDLWindow& removState() noexcept
			{
				m_state.reset(nullptr);
				return *this;
			}

			constexpr bool focus(EventWatch::Focus foc) const noexcept
			{
				return m_watch.state(foc);
			}

			SDLWindow& fillColor(const SDL_Color& col)
			{
				m_renderer.setColor(col);
				return *this;
			}

			constexpr auto* renderer() { return &m_renderer; }
			constexpr auto* window() { return &m_win; }

			void event(const SDL_Event& e)
			{
				if (e.window.windowID == m_win.ID())
				{
					switch (e.window.event)
					{
					case SDL_WINDOWEVENT_SIZE_CHANGED: m_win.update(e);   break;
					default:						   m_watch.listen(e); break;
					}

					m_state->input(e);
					m_state->event(e);
				}
			}

			void update()
			{
				m_state->update();
			}

			void fixedUpdate()
			{
				m_state->fixedUpdate();
			}

			void render()
			{
				m_renderer.setColor({ 0xFF, 0xFF, 0xFF, 0xFF });
				m_renderer.clear();

				m_state->render();

				m_renderer.render();
			}

		private:
			ImplWin m_win;
			ImplRend m_renderer;

			ImplCam m_cam;
			ImplWatch m_watch;

			std::unique_ptr<ImplState> m_state;
		};

	}
}