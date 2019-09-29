#pragma once

#include <utility>
#include <functional>

#include "SDLWindow.h"
#include "Collider.h"
#include "Geometry.h"

namespace ctl::sdl
{
	template<template<typename> class... Func>
	class basicButton : public Shapeable<Rect<int, int>, basicButton<Func...>>, public Func<basicButton<Func...>>...
	{
	public:
		basicButton() = default;

		basicButton(std::function<void()>&& func)
			: m_func(std::move(func))
		{
		}

		void event(const SDL_Event &e)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				if (collision(Point(e.motion.x, e.motion.y), this->m_shape))
				{
					if (!m_isInside)
						m_isInside = true;
				}
				else if (m_isInside)
					m_isInside = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (m_isInside)
					m_func();
				break;

			default:
				break;
			}
		}

		auto& func(std::function<void()>&& f) noexcept { m_func = std::move(f); return *this; }
		bool isInside() const noexcept { return m_isInside; }

	private:
		bool m_isInside = false;
		std::function<void()> m_func;
	};


	template<typename ImplBut>
	class ButtonRend : public Renderable<Renderer, ImplBut>, public crtp<ImplBut, ButtonRend>
	{
	protected:
		ButtonRend()
		{
			m_text.renderer(this->_().renderer());
			m_rect.shape(this->_().shape());
		}

	public:
		ImplBut& text(TTF_Font* f, const char* str)
		{
			m_text.set(f).loadBlended(str);
			m_text.shape({ ((this->_().shape().w - m_text.shape().w) >> 1) + this->_().shape().x, 
				((this->_().shape().h - m_text.shape().h) >> 1) + this->_().shape().y, 
				m_text.shape().w, m_text.shape().h });

			return this->_();
		}

		void draw() const
		{
			SDL_Color col = m_col;
			if (this->_().isInside())
			{
				col.r -= 5;
				col.g -= 5;
				col.b -= 5;
			}

			this->renderer()->setColor(col);
			m_rect.drawFilled();

			this->renderer()->setColor({ 0, 0, 0, 0xFF });
			m_rect.draw();

			m_text.draw();
		}

	private:
		SDL_Color m_col = { 0xFF, 0xFF, 0xFF, 0xFF };
		RectDraw<> m_rect;
		Text m_text;
	};

}