#pragma once

#include "Dim.h"
#include "Point.h"
#include "Tags.h"

namespace ctl::mth
{
	template<typename T1, typename T2>
	class Rect
	{
		static_assert(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>, "Type must be arithmetic");

	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag	 = tag::isRect;

		constexpr Rect() noexcept = default;

		constexpr Rect(const Rect &r) noexcept = default;
		constexpr Rect &operator=(const Rect &r) noexcept = default;

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

#ifdef SDL_h_
		operator SDL_Rect() const noexcept
		{
			return { static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h) };
		}
		operator SDL_FRect() const noexcept
		{
			return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h) };
		}

		SDL_Rect *rect_ptr() noexcept
		{
			static_assert(std::is_same_v<T1, int> && std::is_same_v<T2, int>, "Must be of type int.");
			return reinterpret_cast<SDL_Rect *>(this);
		}
		const SDL_Rect *rect_ptr() const noexcept
		{
			static_assert(std::is_same_v<T1, int> && std::is_same_v<T2, int>, "Must be of type int.");
			return reinterpret_cast<const SDL_Rect *const>(this);
		}
#endif // SDL_h_

		constexpr mth::Point<T1> pos() const { return { x, y }; }

		constexpr void pos(const mth::Point<T1> &p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr mth::Dim<T2> dim() const noexcept { return { w, h }; }

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

	template<typename T1, typename T2>
	class RectRef
	{
		static_assert(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>, "Type must be arithmetic");

	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag	 = tag::isRect;

		constexpr RectRef() noexcept				= delete;
		constexpr RectRef(const RectRef &) noexcept = default;

		/**
		 * @summary construct from x, y, width and height references
		 */
		constexpr RectRef(T1 &x, T1 &y, T2 &w, T2 &h) noexcept
			: x(x)
			, y(y)
			, w(w)
			, h(h)
		{
		}

		/**
		 * @summary construct from point and dimension
		 */
		constexpr RectRef(mth::Point<T1> &p, mth::Dim<T2> &d) noexcept
			: RectRef(p.x, p.y, d.w, d.h)
		{
		}

		constexpr void translate(const mth::Point<T1> &delta) noexcept
		{
			x += delta.x;
			y += delta.y;
		}

		constexpr void pos(const mth::Point<T1> &p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr void resize(const mth::Dim<T2> &delta) noexcept
		{
			w += delta.w;
			h += delta.h;
		}

		T1 &x, &y;
		T2 &w, &h;
	};

	//----------------------------------------------
	// Implementation
	//----------------------------------------------

#ifdef SDL_h_
	template<>
	inline SDL_Rect *Rect<int, int>::rect_ptr() noexcept
	{
		return reinterpret_cast<SDL_Rect *>(this);
	}

	template<>
	inline const SDL_Rect *Rect<int, int>::rect_ptr() const noexcept
	{
		return reinterpret_cast<const SDL_Rect *>(this);
	}
#endif // SDL_h_

} // namespace ctl::mth
