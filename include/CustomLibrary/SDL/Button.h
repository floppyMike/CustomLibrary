#pragma once

#include <SDL2/SDL.h>

#include <utility>
#include <functional>

#include "Collider.h"
#include "Geometry.h"

namespace ctl::sdl
{
	template<typename T>
	class LButton : public T
	{
	public:
		using base_t = T;
		using tag_t	 = tag::isUnassigned;

		using T::T;

		void input(const SDL_Event &e)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				if (collision(mth::Point(e.motion.x, e.motion.y), this->shape()))
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

			default: break;
			}
		}

		auto func(std::function<void()> &&f) noexcept -> auto &
		{
			m_func = std::move(f);
			return *this;
		}

		auto is_inside() const noexcept { return m_is_inside; }

	private:
		bool				  m_is_inside = false;
		std::function<void()> m_func;
	};
} // namespace ctl::sdl