#if not defined _CTL_SDL2_LAYOUT_
#define _CTL_SDL2_LAYOUT_

#include "../Rect.h"
#include "../utility.h"
#include "TypeTraits.h"

namespace ctl::sdl
{
	/**
	 * @brief Extension for getting scaling based on window size
	 */
	template<uses_sdl_shape T>
	class Scaleable : public T
	{
	public:
		using base = T;
		using T::T;

		/**
		 * @brief Set the relative size of the frame together with the original object size
		 *
		 * @param r object size
		 * @param win relative frame size to the object
		 */
		void relative_size(mth::Rect<int, int> r, mth::Dim<int> win)
		{
			T::shape(r);
			m_org_r	  = r;
			m_org_win = win;
		}

		/**
		 * @brief Scale the object based on the new dimensions
		 * @param r frame size to scale on
		 */
		void resize(mth::Dim<int> r)
		{
			const float scale_w = (float)r.w / m_org_win.w, scale_h = (float)r.h / m_org_win.h;
			T::shape({ static_cast<int>(m_org_r.x * scale_w), static_cast<int>(m_org_r.y * scale_h),
					   static_cast<int>(m_org_r.w * scale_w), static_cast<int>(m_org_r.h * scale_h) });
		}

	private:
		mth::Dim<int>		m_org_win;
		mth::Rect<int, int> m_org_r;
	};
} // namespace ctl::sdl

#endif
