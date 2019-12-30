#pragma once

#include <utility>
#include <functional>

#include "Collider.h"
#include "Geometry.h"

namespace ctl::sdl
{
	template<typename Impl, typename... Tags>
	class EButton
	{
		static_assert(has_shape_v<Tags...>, "Parent doesn't support shapes.");
		Impl* const pthis = static_cast<Impl*>(this);

	public:
		void event(const SDL_Event& e)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				if (collision(Point(e.motion.x, e.motion.y), pthis->shape()))
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

		Impl& func(std::function<void()>&& f) noexcept 
		{
			m_func = std::move(f); 
			return *pthis; 
		}

		bool is_inside() const noexcept { return m_is_inside; }

	private:
		bool m_is_inside = false;
		std::function<void()> m_func;
	};
}