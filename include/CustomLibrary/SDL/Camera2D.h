#if not defined _CTL_SDL2_CAM2D_
#define _CTL_SDL2_CAM2D_

#include "../BasicTypes.h"
#include "SDL_init.h"

namespace ctl::sdl
{
	/**
	 * @brief Represents the top left camera point
	 */
	class Camera2D
	{
	public:
		/**
		 * @brief translates world coord to screen coord
		 * @param loc world coord
		 * @return mth::Point
		 */
		template<typename T>
		constexpr auto world_screen(mth::Point<T> loc) const noexcept -> mth::Point<int>
		{
			return (mth::Point<int>)((loc - m_loc) * m_scale);
		}

		/**
		 * @brief translates world dim to screen dim
		 * @param dim world dim
		 * @return dim
		 */
		template<typename T>
		constexpr auto world_screen(mth::Dim<T> dim) const noexcept -> mth::Dim<int>
		{
			return (mth::Dim<int>)(dim * m_scale);
		}

		/**
		 * @brief translates world to screen
		 * @param s shape
		 * @return transformed shape
		 */
		template<typename T>
		constexpr auto world_screen(mth::Rect<T, T> s) const noexcept -> mth::Rect<int, int>
		{
			return { world_screen(s.pos()), world_screen(s.dim()) };
		}

		/**
		 * @brief translates world to screen
		 * @param s shape
		 * @return transformed shape
		 */
		template<typename T>
		constexpr auto world_screen(mth::Circle<T, T> s) const noexcept -> mth::Circle<int, int>
		{
			return { world_screen(s.pos()), s.r * m_scale };
		}

		/**
		 * @brief translates world to screen
		 * @param s shape
		 * @return transformed shape
		 */
		template<typename T>
		constexpr auto world_screen(mth::Line<T> s) const noexcept -> mth::Line<int>
		{
			return { world_screen(s.pos1()), world_screen(s.pos2()) };
		}

		/**
		 * @brief translates screen coord to world coord
		 * @param loc screen coord
		 * @return mth::Point
		 */
		template<typename T>
		[[nodiscard]] constexpr auto screen_world(mth::Point<T> loc) const noexcept -> mth::Point<float>
		{
			return static_cast<mth::Point<float>>(loc) / m_scale + m_loc;
		}

		/**
		 * @brief translates screen dim to world dim
		 * @param dim screen dim
		 * @return dim
		 */
		template<typename T>
		[[nodiscard]] constexpr auto screen_world(mth::Dim<T> dim) const noexcept -> mth::Dim<float>
		{
			return static_cast<mth::Dim<float>>(dim) / m_scale;
		}

		/**
		 * @brief translates screen rect to world rect
		 * @param rect screen rect
		 * @return rect
		 */
		template<typename T>
		[[nodiscard]] constexpr auto screen_world(mth::Rect<T, T> rect) const noexcept -> mth::Rect<float, float>
		{
			return { screen_world(rect.pos()), screen_world(rect.dim()) };
		}

		/**
		 * @brief Zooms the screen by a factor
		 * @param factor factor to zoom at
		 */
		constexpr void zoom(float factor) noexcept { m_scale *= factor; }

		/**
		 * @brief Zooms the screen by a factor
		 * @param factor factor to zoom at
		 * @param p point to zoom into
		 */
		void zoom(float factor, mth::Point<float> p) noexcept
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
		[[nodiscard]] constexpr auto scale() const noexcept { return m_scale; }

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

	private:
		mth::Point<float> m_loc	  = { 0, 0 };
		float			  m_scale = 1.F;
	};

} // namespace ctl::sdl

#endif
