#pragma once

#include <CustomLibrary/Vector.h>

#include "Engine.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Collider.h"

namespace ctl
{
	class Camera2D
	{
	public:
		Camera2D(const SDLDim &screen)
			: m_col(&m_camLoc.x, &m_camLoc.y, &screen.w, &screen.h)
		{
		}

		auto screenToWorld(SDLPoint loc) const noexcept
		{
			loc += m_camLoc;
			return loc;
		}

		auto worldToScreen(SDLPoint loc) const noexcept
		{
			loc -= m_camLoc;
			return loc;
		}

		constexpr auto& translate(const NumVec<int, 2> &val) noexcept
		{
			m_camLoc.x += val[0];
			m_camLoc.y += val[1];

			return *this;
		}

		constexpr const auto& loc() const noexcept { return m_camLoc; }
		constexpr auto& loc(const SDLPoint& loc) noexcept { m_camLoc = loc; return *this; }

		constexpr const auto& col() const noexcept { return m_col; }

	private:
		SDLPoint m_camLoc = { 0, 0 };
		ctl::BoxCol m_col;
	};

	class SDLWindow : public WindowBase
	{
	public:
		//For rendererFlags check https://wiki.libsdl.org/SDL_CreateRenderer#Remarks
		SDLWindow(const std::string& name,
			const SDLDim& dim,
			const Uint32& windowFlags = SDL_WINDOW_SHOWN, 
			const Uint32 &rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
			: WindowBase(name, dim, windowFlags)
			, cam(m_dim)
		{
			if ((m_renderer = SDL_CreateRenderer(m_window, -1, rendererFlags)) == nullptr)
				throw ctl::Log(SDL_GetError(), ctl::Log::Sev::ERR0R);

			SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}

		SDLWindow(SDLWindow &&) = delete;
		SDLWindow(const SDLWindow &) = delete;

		virtual ~SDLWindow() { destroy(); }

		void destroy()
		{
			if (m_renderer != nullptr)
				SDL_DestroyRenderer(m_renderer);
			WindowBase::destroy();
		}

		inline void clear()
		{
			SDL_SetRenderDrawColor(m_renderer, bg.r, bg.g, bg.b, bg.a);
			SDL_RenderClear(m_renderer);
		}
		inline void render()
		{
			SDL_RenderPresent(m_renderer);
		}

		//Check https://wiki.libsdl.org/SDL_BlendMode#Values
		auto& blend(const SDL_BlendMode &m) 
		{
			SDL_SetRenderDrawBlendMode(m_renderer, m);
			return *this;
		}

		void _render_() override
		{
			clear();
			_invoke_(&StateBase::render);
			render();
		}

		constexpr inline auto* renderer() { return m_renderer; }

		//Public variable for changing the background of the renderer
		SDL_Colour bg = { 0xFF, 0xFF, 0xFF, 0xFF };

		//Public variable for changing the cameras position
		Camera2D cam;

	private:
		SDL_Renderer *m_renderer = nullptr;
	};

	template<typename ...T>
	struct SDLObject
	{
		SDLWindow& win;
		SDL& en;
		ResourceManager<T...>& res;
	};
}