#if not defined _CTL_SDL2_CAM2D_
#define _CTL_SDL2_CAM2D_

#include "../Geometry.h"
#include "SDL_init.h"

namespace ctl::sdl
{
	/**
	 * @brief Represents the top left camera point
	 */
	struct Camera2D
	{
		mth::Point<float> m_loc	  = { 0.F, 0.F };
		float			  m_scale = 1.F;

		/**
		 * @brief translates world coord to screen coord
		 * @param loc world coord
		 * @return mth::Point
		 */
		constexpr auto world_screen(mth::Point<float> p) const noexcept -> mth::Point<int>
		{
			return (mth::Point<int>)((p - m_loc) * m_scale);
		}

		/**
		 * @brief translates world dim to screen dim
		 * @param dim world dim
		 * @return dim
		 */
		constexpr auto world_screen(mth::Dim<float> dim) const noexcept -> mth::Dim<int>
		{
			return (mth::Dim<int>)(dim * m_scale);
		}

		/**
		 * @brief translates world to screen
		 * @param s shape
		 * @return transformed shape
		 */
		constexpr auto world_screen(mth::Rect<float> s) const noexcept -> mth::Rect<int>
		{
			const auto p = world_screen(s.pos());
			const auto d = world_screen(s.dim());

			return { p.x, p.y, d.w, d.h };
		}

		/**
		 * @brief translates world to screen
		 * @param s shape
		 * @return transformed shape
		 */
		constexpr auto world_screen(mth::Circle<float> s) const noexcept -> mth::Circle<int>
		{
			const auto p = world_screen(s.pos());
			const auto r = (int)(s.r * m_scale);

			return { p.x, p.y, r };
		}

		/**
		 * @brief translates world to screen
		 * @param s shape
		 * @return transformed shape
		 */
		constexpr auto world_screen(mth::Line<float> s) const noexcept -> mth::Line<int>
		{
			const auto p1 = world_screen(s.pos1());
			const auto p2 = world_screen(s.pos2());

			return { p1.x, p1.y, p2.x, p2.y };
		}

		/**
		 * @brief translates screen coord to world coord
		 * @param loc screen coord
		 * @return mth::Point
		 */
		constexpr auto screen_world(mth::Point<int> p) const noexcept -> mth::Point<float>
		{
			return (mth::Point<float>)(p) / m_scale + m_loc;
		}

		/**
		 * @brief translates screen dim to world dim
		 * @param dim screen dim
		 * @return dim
		 */
		constexpr auto screen_world(mth::Dim<int> dim) const noexcept -> mth::Dim<float>
		{
			return (mth::Dim<float>)(dim) / m_scale;
		}

		/**
		 * @brief translates screen rect to world rect
		 * @param rect screen rect
		 * @return rect
		 */
		constexpr auto screen_world(mth::Rect<int> rect) const noexcept -> mth::Rect<float>
		{
			const auto p = screen_world(rect.pos());
			const auto d = screen_world(rect.dim());

			return { p.x, p.y, d.w, d.h };
		}

		/**
		 * @brief Zooms the screen by a factor
		 * @param factor factor to zoom at
		 */
		constexpr void zoom(float factor) noexcept
		{
			m_scale *= factor;
		}

		/**
		 * @brief Zooms the screen by a factor
		 * @param factor factor to zoom at
		 * @param p point to zoom into
		 */
		void zoom(float factor, mth::Point<int> p) noexcept
		{
			const auto w = screen_world(p);
			m_scale *= factor;
			const auto s = screen_world(p);
			m_loc += w - s;
		}

		/**
		 * @brief Get the scale
		 * @return current scale
		 */
		[[nodiscard]] constexpr auto scale() const noexcept
		{
			return m_scale;
		}

		/**
		 * @brief Move the camera with the scale
		 * @param dx delta x
		 * @param dy delta y
		 */
		constexpr void translate(float dx, float dy) noexcept
		{
			m_loc.x += dx / m_scale;
			m_loc.y += dy / m_scale;
		}
	};

} // namespace ctl::sdl

#endif
