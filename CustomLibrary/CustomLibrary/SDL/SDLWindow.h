#pragma once

#include <CustomLibrary/Error.h> 

#include "Engine.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Collider.h"

namespace ctl
{
	class Camera2D
	{
	public:
		/**
		* @summary constructs collider from camera location and screen size ref
		* @param "screen" ref to screen SDLDim
		*/
		Camera2D(SDLDim<int>& screen) noexcept
			: m_col(m_camLoc, screen)
		{
		}

		/**
		* @summary translates screen coord to world coord
		* @param "screen" ref to screen SDLDim
		* @returns SDLPoint of world coord
		*/
		template<typename T>
		SDLPoint<T> screenToWorld(const SDLPoint<T>& loc) const noexcept { return loc += m_camLoc; }

		/**
		* @summary translates world coord to screen coord
		* @param "screen" ref to world SDLDim
		* @returns SDLPoint of screen coord
		*/
		template<typename T>
		SDLPoint<T> worldToScreen(SDLPoint<T> loc) const noexcept
		{
			return loc -= m_camLoc;
		}

		/**
		* @summary moves SDLPoint
		* @param "deltaX" x distance
		* @param "deltaY" y distance
		*/
		constexpr Camera2D& mov(const float& deltaX, const float& deltaY) noexcept;

		/**
		* @summary location accessors
		*/
		constexpr const auto& loc() const noexcept { return m_camLoc; }
		constexpr Camera2D& loc(const SDLPoint<float>& loc) noexcept
		{ 
			m_camLoc = loc; 
			return *this; 
		}

		/**
		* @summary collider const-accessors
		*/
		constexpr const auto& col() const noexcept { return m_col.collider(); }

	private:
		SDLPoint<float> m_camLoc = { 0.f, 0.f };
		ctl::ColliderVar<ctl::SDLRectRef<float, int>> m_col;
	};

	template<typename ImplWatch, typename ImplWinDB>
	class SDLWindow : public BasicWindow<ImplWatch, ImplWinDB>
	{
		using type = SDLWindow<ImplWatch, ImplWinDB>;

	public:
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
		SDLWindow(const std::string& name,
			const SDLDim<int>& dim,
			const Uint32& windowFlags = SDL_WINDOW_SHOWN,
			const Uint32& rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
			: BasicWindow(name, dim, windowFlags)
			, cam(m_dim)
		{
			if ((m_renderer = SDL_CreateRenderer(m_window, -1, rendererFlags)) == nullptr)
				throw ctl::Log(SDL_GetError());

			SDL_RenderSetLogicalSize(m_renderer, m_dim.w, m_dim.h);
			SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}

		virtual ~SDLWindow() 
		{ 
			destroy(); 
		}

		/**
		* @summary destroy window and renderer
		*/
		void destroy()
		{
			if (m_renderer != nullptr)
				SDL_DestroyRenderer(m_renderer);

			BasicWindow::destroy();
		}

		/**
		* @summary clear render object
		*/
		void clear();

		/**
		* @summary render present items
		*/
		void render();

		/**
		* @summary blend mode accessors
		* @remarks check https://wiki.libsdl.org/SDL_BlendMode#Values
		*/
		SDL_BlendMode blendMode() noexcept;
		SDLWindow& blendMode(const SDL_BlendMode& m) noexcept;

		/**
		* @summary render loop
		*/
		void _render_() override final;

		/**
		* @summary SDL renderer accessor
		*/
		constexpr auto* renderer() { return m_renderer; }

		SDL_Color bg = { 0xFF, 0xFF, 0xFF, 0xFF };
		Camera2D cam;

	private:
		SDL_Renderer *m_renderer = nullptr;
	};

	struct SDLObject
	{
		SDL& en;
		SDLWindow& win;
	};
}