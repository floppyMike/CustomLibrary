#pragma once

#include <utility>
#include <functional>

#include "SDLWindow.h"
#include "Collider.h"
#include "Geometry.h"

namespace ctl
{
	class Button
	{
	public:
		Button()
			: m_col(&m_buttSize.x, &m_buttSize.y, &m_buttSize.w, &m_buttSize.h)
		{}

		Button(Text&& t, std::function<void()>&& f, const Rect& size)
			: m_buttText(std::move(t))
			, m_buttSize(size)
			, m_col(&m_buttSize.x, &m_buttSize.y, &m_buttSize.w, &m_buttSize.h)
			, m_func(std::move(f))
		{
			m_buttText.loc({ ((size.w - m_buttText.dim().w) >> 1) + size.x, ((size.h - m_buttText.dim().h) >> 1) + size.y });
		}

		Button(Text& t, std::function<void()>&& func, const Rect& size)
			: Button(std::move(t), std::forward<std::function<void()>>(func), size)
		{
		}

		auto& event(const SDL_Event &e)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				if (m_col.inside(DotCol(&unmove(e.motion.x + m_buttText.window()->cam.loc().x), &unmove(e.motion.y + m_buttText.window()->cam.loc().y))))
				{
					if (!m_isInside)
					{
						m_isInside = true;
						m_colour = { m_colour.r - 5u, m_colour.g - 5u, m_colour.b - 5u, m_colour.a };
					}
				}
				else if (m_isInside)
				{
					m_isInside = false;
					m_colour = { m_colour.r + 5u, m_colour.g + 5u, m_colour.b + 5u, m_colour.a };
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (m_isInside)
					m_func();
				break;

			default:
				break;
			}

			return *this;
		}

		auto& draw()
		{
			ctl::draw<Fig::SQUARE_FILLED>(m_colour, m_buttText.window(), m_buttSize);
			m_buttText.render();
			return *this;
		}

		constexpr const auto& text() const { return m_buttText; }
		auto& text(Text &&t) { m_buttText = std::move(t); return *this; }
		auto& text(Text &t) { m_buttText = std::move(t); return *this; }

		constexpr const auto& dim() const { return m_buttSize; }
		constexpr auto& dim(const Rect& r) { m_buttSize = r; m_buttText.loc({ ((m_buttSize.w - m_buttText.dim().w) >> 1) + m_buttSize.x, ((m_buttSize.h - m_buttText.dim().h) >> 1) + m_buttSize.y }); return *this; }

		constexpr const auto& func() const { return m_func; }
		auto& func(std::function<void()>&& f) { m_func = std::move(f); return *this; }

		constexpr const auto& colour() const { return m_colour; }
		constexpr auto& colour(const SDL_Color& colour) { m_colour = colour; return *this; }

	private:
		Text m_buttText;

		Rect m_buttSize;

		BoxCol m_col;
		bool m_isInside = false;

		SDL_Color m_colour = { 220, 220, 220, 0xFF };

		std::function<void()> m_func;
	};
}