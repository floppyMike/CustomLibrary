#pragma once

#include <CustomLibrary/Error.h> 

#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include "Camera2D.h"
#include "State.h"

namespace ctl::sdl
{
	class SDLWindow : public IWindow
	{
	public:
		SDLWindow(const char* name,
			const Dim<int>& dim,
			const Uint32& windowFlags = SDL_WINDOW_SHOWN,
			const Uint32& rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
			: m_win(name, dim, windowFlags)
			, m_renderer(&m_win, rendererFlags)
		{
			m_renderer.setLogicalSize(m_win.dim());
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
		IState& setState(Args&&... args)
		{
			return *(m_state = std::make_unique<State>(std::forward<Args>(args)...));
		}

		SDLWindow& removState() noexcept
		{
			m_state.reset(nullptr);
			return *this;
		}

		constexpr auto& renderer() noexcept { return m_renderer; }
		constexpr auto& window() noexcept { return m_win; }

	private:
		Window m_win;
		Renderer m_renderer;

		Camera2D m_cam;

		std::unique_ptr<IState> m_state;


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

		void fixedUpdate() override
		{
			m_state->fixedUpdate();
		}

		void render() override
		{
			m_renderer.setColor({ 0xFF, 0xFF, 0xFF, 0xFF });
			m_renderer.fill();

			m_state->draw();

			m_renderer.render();
		}
	};

}