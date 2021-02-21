#if not defined _CTL_SDL2_CAM2D_
#define _CTL_SDL2_CAM2D_

#include "../BasicTypes.h"

namespace ctl::sdl
{
	/**
	 * @brief Represents the top left camera point
	 */
	class Camera2D : public mth::Point<float>
	{
	public:
		using mth::Point<float>::Point;

		/**
		 * @brief translates world coord to screen coord
		 * @param loc world coord
		 * @return mth::Point
		 */
		constexpr auto world_screen(mth::Point<float> loc) const noexcept -> mth::Point<float>
		{
			return (loc - *this) * m_scale;
		}

		/**
		 * @brief translates world dim to screen dim
		 * @param dim world dim
		 * @return dim
		 */
		template<typename T>
		constexpr auto world_screen(mth::Dim<T> dim) const noexcept -> mth::Dim<T>
		{
			return dim * m_scale;
		}

		/**
		 * @brief translates screen coord to world coord
		 * @param loc screen coord
		 * @return mth::Point
		 */
		constexpr auto screen_world(mth::Point<float> loc) const noexcept -> mth::Point<float>
		{
			return loc / m_scale + *this;
		}

		/**
		 * @brief translates screen dim to world dim
		 * @param dim screen dim
		 * @return dim
		 */
		template<typename T>
		constexpr auto screen_world(mth::Dim<T> dim) const noexcept -> mth::Dim<T>
		{
			return dim / m_scale;
		}

		/**
		 * @brief Zooms the screen by a factor
		 * @param factor factor to zoom at
		 */
		constexpr void zoom(float factor) noexcept { m_scale *= factor; }

		/**
		 * @brief Zooms the screen by a factor
		 * @param factor factor to zoom at
		 */
		void zoom(float factor, Point<int> p) noexcept
		{
			const auto w = screen_world(p);
			m_scale *= factor;
			const auto s = screen_world(p);
			*this += w - s;
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
			x += dx / m_scale;
			y += dy / m_scale;
		}

	private:
		float m_scale = 1.f;
	};

} // namespace ctl::sdl

#endif