#pragma once

#include <CustomLibrary/Observer.h>
#include <CustomLibrary/Vector.h>

#include "Engine.h"
#include "State.h"

namespace ctl
{
	class SDLPoint : public SDL_Point
	{
	public:
		using SDL_Point::SDL_Point;

		constexpr auto& operator+=(const SDLPoint& p) noexcept
		{
			x += p.x;
			y += p.y;

			return *this;
		}
		constexpr auto& operator-=(const SDLPoint& p) noexcept
		{
			x -= p.x;
			y -= p.y;

			return *this;
		}

		constexpr auto operator+(const SDLPoint& p) const noexcept
		{
			return SDLPoint(x + p.x, y + p.y);
		}
		constexpr auto operator-(const SDLPoint& p) const noexcept
		{
			return SDLPoint(x - p.x, y - p.y);
		}
	};

	class SDLDim
	{
	public:
		int w = 0, h = 0;

		constexpr SDLDim() = default;
		constexpr SDLDim(const SDLDim&) = default;
		constexpr SDLDim(SDLDim&&) = default;

		constexpr SDLDim& operator=(const SDLDim&) = default;
		constexpr SDLDim& operator=(SDLDim&&) = default;

		constexpr SDLDim(const int& pw, const int& ph)
			: w(pw)
			, h(ph)
		{
		}
	};

	class SDLRect : public SDL_Rect
	{
	public:
		SDLRect() = default;
		SDLRect(const SDLRect&) = default;
		SDLRect(SDLRect&&) = default;

		constexpr SDLRect& operator=(const SDLRect&) = default;
		constexpr SDLRect& operator=(SDLRect&&) = default;

		using SDL_Rect::SDL_Rect;

		constexpr SDLRect(const SDLPoint& p, const SDLDim& d)
			: SDL_Rect(p.x, p.y, d.w, d.h)
		{
		}
	};

	class SDLCircle
	{
	public:
		int x = 0, y = 0, r = 0;

		constexpr SDLCircle() = default;
		SDLCircle(const SDLCircle&) = default;
		SDLCircle(SDLCircle&&) = default;

		constexpr SDLCircle& operator=(const SDLCircle&) = default;
		constexpr SDLCircle& operator=(SDLCircle&&) = default;

		constexpr SDLCircle(const int& px, const int& py, const int& pr)
			: x(px)
			, y(py)
			, r(pr)
		{
		}

		constexpr SDLCircle(const SDLPoint& p, const int& r)
			: SDLCircle(p.x, p.y, r)
		{
		}
	};

	class WindowBase : public ObSu<SDL_Event>::Observer
	{
	public:
		//For windowFlags check https://wiki.libsdl.org/SDL_WindowFlags#Values
		WindowBase(const std::string &name,
			const SDLDim &dim,
			const Uint32 &windowFlags = SDL_WINDOW_SHOWN)
			: m_dim(dim)
			, m_focus(0b010011)
		{
			if ((m_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_dim.w, m_dim.h, windowFlags)) == nullptr)
				throw ctl::Log(SDL_GetError());

			m_id = SDL_GetWindowID(m_window);
		}

		virtual ~WindowBase()
		{
			destroy();
		}

		void destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window);
		}

		inline void _invoke_(void (StateBase::*f)(const SDL_Event &), const SDL_Event &e) noexcept
		{
			((*m_state).*f)(e);
		}
		inline void _invoke_(void (StateBase::*f)()) noexcept
		{
			((*m_state).*f)();
		}

		virtual void _render_() = 0;

		template<typename State, typename ...Args>
		auto& setState(Args&&... args)
		{
			m_state = std::make_unique<State>(args...);
			return *this;
		}
		auto& removState() noexcept
		{
			m_state.reset();
			return *this;
		}

		constexpr inline auto* window() noexcept { return m_window; }
		constexpr const auto& ID() const noexcept { return m_id; }
		constexpr const auto& dim() const noexcept { return m_dim; }

		enum Focus { MOUSE, KEYBOARD, FULLSCREEN, MINIMIZED, SHOWN, CLOSE };
		constexpr const auto& focus() const noexcept { return m_focus; }

	protected:
		void update(const SDL_Event &e) override final
		{
			if (e.window.windowID == m_id)
			{
				if (e.type == SDL_WINDOWEVENT)
					switch (e.window.event)
					{
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						m_dim.w = e.window.data1;
						m_dim.h = e.window.data2;
						break;

					case SDL_WINDOWEVENT_ENTER:
						m_focus[MOUSE] = true;
						break;

					case SDL_WINDOWEVENT_LEAVE:
						m_focus[MOUSE] = false;
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						m_focus[KEYBOARD] = true;
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
						m_focus[KEYBOARD] = false;
						break;

					case SDL_WINDOWEVENT_MINIMIZED:
						m_focus[MINIMIZED] = true;
						break;

					case SDL_WINDOWEVENT_MAXIMIZED:
						m_focus[MINIMIZED] = false;
						break;

					case SDL_WINDOWEVENT_RESTORED:
						m_focus[MINIMIZED] = false;
						break;

					case SDL_WINDOWEVENT_CLOSE:
						m_focus[SHOWN] = false;
						m_focus[CLOSE] = true;
						destroy();
						break;
					}

				_invoke_(&StateBase::input, e);
				_invoke_(&StateBase::event, e);
			}
		}

		SDL_Window *m_window = nullptr;
		Uint32 m_id;

		SDLDim m_dim;
		std::bitset<6> m_focus;

		std::unique_ptr<StateBase> m_state;
	};
}