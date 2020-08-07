#pragma once

#include "Camera2D.h"
#include "Engine.h"
#include "Render.h"
#include "Renderer.h"
#include "State.h"
#include "Window.h"
#include "../Error.h"

namespace ctl::sdl
{
	/**
	 * @brief Stock general SDL2 Window
	 */
	class SDLWindow : public IWindow
	{
	public:
		/**
		 * @brief Initialize Window and Renderer
		 *
		 * @param name Name of the window
		 * @param dim Size of the window
		 * @param windowFlags window init flags https://wiki.libsdl.org/SDL_WindowFlags#Values
		 * @param rendererFlags renderer init flags https://wiki.libsdl.org/SDL_RendererFlags#Values
		 */
		SDLWindow(const char *name, const mth::Dim<int> &dim, const Uint32 &windowFlags = SDL_WINDOW_SHOWN,
				  const Uint32 &rendererFlags = SDL_RENDERER_ACCELERATED)
			: m_win(name, dim, windowFlags)
			, m_renderer(&m_win, rendererFlags)
		{
			m_renderer.logical_size(m_win.dim());
		}

		~SDLWindow() { destroy(); }

		/**
		 * @brief Close and deallocate the window
		 */
		void destroy()
		{
			m_win.destroy();
			m_renderer.destroy();
		}

		/**
		 * @brief Queue a state change
		 * The state change will take place when a new loop iteration takes place. Keep in mind that the previous state
		 * IS DEALLOCATED when the construction is takeing place.
		 * @tparam State State type to change to
		 * @param args Arguments to construct the new state
		 */
		template<typename State, typename... _Args>
		void queue_state(_Args &&... args)
		{
			m_state.set<State>(std::forward<_Args>(args)...);
		}

		/**
		 * @brief Get the renderer
		 * @return Renderer&
		 */
		constexpr auto renderer() noexcept -> auto & { return m_renderer; }
		/**
		 * @brief Get the window
		 * @return Window&
		 */
		constexpr auto window() noexcept -> auto & { return m_win; }

		/**
		 * @brief Get the camera
		 * @return Camera2D&
		 */
		constexpr auto camera() noexcept -> auto & { return m_cam; }

		void pre_pass() override { m_state.update(); }
		void event(const SDL_Event &e) override
		{
			if (e.window.windowID == m_win.ID())
			{
				m_state->input(e);
				m_state->event(e);
			}
		}
		void update() override { m_state->update(); }
		void fixed_update() override { m_state->fixed_update(); }
		void render() override
		{
			if (m_renderer.will_render())
			{
				auto r = sdl::render(&m_renderer);

				r.fill(sdl::WHITE);
				m_state->draw();
				r.locking_render();
			}
		}

	private:
		Window						m_win;
		sdl::Delayed<sdl::Renderer> m_renderer;

		Camera2D m_cam;

		StateManager<IState> m_state;
	};

} // namespace ctl::sdl
