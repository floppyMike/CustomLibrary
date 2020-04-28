#pragma once

#include <CustomLibrary/Error.h> 

#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include "Camera2D.h"
#include "State.h"
#include "Render.h"

namespace ctl::sdl
{
	class SDLWindow : public IWindow
	{
	public:
		SDLWindow(const char* name,
			const mth::Dim<int>& dim,
			const Uint32& windowFlags = SDL_WINDOW_SHOWN,
			const Uint32& rendererFlags = SDL_RENDERER_ACCELERATED)
			: m_win(name, dim, windowFlags)
			, m_renderer(&m_win, rendererFlags)
		{
			//m_renderer.logical_size(m_win.dim());
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
		void queue_state(Args&&... args)
		{
			m_state.set<State>(std::forward<Args>(args)...);
		}

		constexpr auto& renderer() noexcept { return m_renderer; }
		constexpr auto& window() noexcept { return m_win; }

	private:
		Window m_win;
		sdl::DelayedRenderer<sdl::Renderer> m_renderer;

		Camera2D m_cam;

		StateManager<IState> m_state;


		void pre_pass() override
		{
			m_state.update();
		}

		void event(const SDL_Event& e) override
		{
			if (e.window.windowID == m_win.ID())
			{
				m_state->input(e);
				m_state->event(e);
			}
		}

		void update() override
		{
			m_state->update();
		}

		void fixed_update() override
		{
			m_state->fixed_update();
		}

		void render() override
		{
			if (m_renderer.will_render())
			{
				Render<decltype(m_renderer)> rend(&m_renderer);

				rend.fill(sdl::WHITE);
				m_state->draw();
				rend.locking_render();
			}
		}
	};

}