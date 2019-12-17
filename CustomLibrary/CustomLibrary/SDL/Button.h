#pragma once

#include <utility>
#include <functional>

#include "SDLWindow.h"
#include "Collider.h"
#include "Geometry.h"
#include <CustomLibrary/Traits.h>

namespace ctl::sdl
{
	//Note: Why not make the button itself into a extension?

	template<typename ImplTex>
	class ButtonEx : public crtp<ImplTex, ButtonEx>
	{
	public:
		void event(const SDL_Event& e)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				if (collision(Point(e.motion.x, e.motion.y), this->_().shape()))
				{
					if (!m_is_inside)
						m_is_inside = true;
				}
				else if (m_is_inside)
					m_is_inside = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (m_is_inside)
					m_func();
				break;

			default:
				break;
			}
		}

		ImplTex& func(std::function<void()>&& f) noexcept { m_func = std::move(f); return this->_(); }
		bool is_inside() const noexcept { return m_is_inside; }

	private:
		bool m_is_inside = false;
		std::function<void()> m_func;
	};


	/*template<typename Derived>
	class basicButton : public Shapeable<Rect<int, int>, Derived>
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


	template<template<typename> class ImplBut>
	class ButtonRend : public ImplBut<ButtonRend<ImplBut>>
	{
		using baseB = ImplBut<ButtonRend<ImplBut>>;

		void _syncSize_()
		{
			m_text.shape({ ((baseB::shape().w - m_text.shape().w) >> 1) + baseB::shape().x,
				((baseB::shape().h - m_text.shape().h) >> 1) + baseB::shape().y,
				m_text.shape().w, m_text.shape().h });
		}

	public:
		ButtonRend(Renderer* r)
			: m_text(r)
			, m_rect(r)
		{
		}

		ButtonRend& renderer(Renderer* r)
		{
			m_text.renderer(r);
			m_rect.renderer(r);

			return *this;
		}

		ButtonRend& shape(const Rect<int, int>& r)
		{
			baseB::shape(r);
			m_rect.shape(r);
			_syncSize_();

			return *this;
		}

		ButtonRend& text(TTF_Font* f, const char* str)
		{
			m_text.set(f).loadBlended(str);
			_syncSize_();

			return *this;
		}

		void draw() const
		{
			SDL_Color col = m_col;
			if (this->isInside())
			{
				col.r -= 20;
				col.g -= 20;
				col.b -= 20;
			}

			m_rect.renderer()->color(col);
			m_rect.drawFilled();

			m_rect.renderer()->color({ 0, 0, 0, 0xFF });
			m_rect.draw();

			m_text.draw();
		}

	private:
		SDL_Color m_col = { 0xFF, 0xFF, 0xFF, 0xFF };
		RectDraw<> m_rect;
		Text m_text;
	};*/



}