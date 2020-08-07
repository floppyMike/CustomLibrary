#pragma once

#include "Dim.h"
#include "Point.h"
#include "Traits.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Concept
	// -----------------------------------------------------------------------------

	template<typename T>
	concept simple_rect = simple_point<T> &&simple_dim<T>;

	// -----------------------------------------------------------------------------
	// Rect
	// -----------------------------------------------------------------------------

	template<arithmetic T1, arithmetic T2>
	class Rect
	{
	public:
		constexpr Rect() noexcept = default;

		constexpr Rect(const Rect &r) noexcept = default;
		constexpr auto operator=(const Rect &r) noexcept -> Rect & = default;

		constexpr Rect(const T1 &x, const T1 &y, const T2 &w, const T2 &h) noexcept
			: x(x)
			, y(y)
			, w(w)
			, h(h)
		{
		}

		constexpr Rect(const mth::Point<T1> &p, const mth::Dim<T2> &d) noexcept
			: Rect(p.x, p.y, d.w, d.h)
		{
		}

		constexpr auto pos() const -> mth::Point<T1> { return { x, y }; }
		constexpr void pos(const mth::Point<T1> &p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr auto dim() const noexcept -> mth::Dim<T2> { return { w, h }; }
		constexpr void dim(const mth::Dim<T2> &d) noexcept
		{
			w = d.w;
			h = d.h;
		}

		constexpr void translate(const mth::Point<T1> &delta) noexcept
		{
			x += delta.x;
			y += delta.y;
		}

		constexpr void resize(const mth::Dim<T2> &delta) noexcept
		{
			w += delta.w;
			h += delta.h;
		}

		T1 x, y;
		T2 w, h;
	};

	//----------------------------------------------
	// Implementation
	//----------------------------------------------

	// #ifdef SDL_h_
	// 	template<>
	// 	inline SDL_Rect *Rect<int, int>::rect_ptr() noexcept
	// 	{
	// 		return reinterpret_cast<SDL_Rect *>(this);
	// 	}

	// 	template<>
	// 	inline const SDL_Rect *Rect<int, int>::rect_ptr() const noexcept
	// 	{
	// 		return reinterpret_cast<const SDL_Rect *>(this);
	// 	}
	// #endif // SDL_h_

} // namespace ctl::mth
