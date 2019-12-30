#pragma once

#include "Tags.h"
#include "Point.h"

namespace ctl::mth
{
	template<typename T>
	class Line
	{
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	public:
		using num_t1 = T;
		using tag = tag::isLine;

		constexpr Line() noexcept = default;

		constexpr Line(const Line&) noexcept = default;

		constexpr Line& operator=(const Line& l) noexcept = default;

		constexpr Line(const T& x1, const T& y1, const T& x2, const T& y2) noexcept
			: x1(x1), y1(y1), x2(x2), y2(y2)
		{
		}

		constexpr Line(const mth::Point<T>& p1, const mth::Point<T>& p2) noexcept
			: Line(p1.x, p1.y, p2.x, p2.y)
		{
		}

		template<typename U>
		constexpr operator Line<U>() noexcept
		{
			return { static_cast<U>(x1), static_cast<U>(y1),
				static_cast<U>(x2), static_cast<U>(y2) };
		}

#ifdef SDL_h_
		const SDL_Point* point_ptr() const noexcept
		{
			static_assert(false, "Type must be int.");
			return nullptr;
		}
		SDL_Point* point_ptr() noexcept
		{
			static_assert(false, "Type must be int.");
			return nullptr;
		}
#endif // SDL_h_


		constexpr mth::Point<T> pos1() const noexcept
		{
			return { x1, y1 };
		}
		constexpr auto& pos1(const mth::Point<T>& p) noexcept
		{
			x1 = p.x;
			y1 = p.y;
			return *this;
		}

		constexpr mth::Point<T> pos2() const noexcept
		{
			return { x2, y2 };
		}
		constexpr auto& pos2(const mth::Point<T>& p) noexcept
		{
			x2 = p.x;
			y2 = p.y;
			return *this;
		}

		constexpr auto& translate(const mth::Point<T>& delta1, const mth::Point<T>& delta2) noexcept
		{
			x1 += delta1.x;
			y1 += delta1.y;
			x2 += delta2.x;
			y2 += delta2.y;
			return *this;
		}

		constexpr auto& pos(const mth::Point<T>& p1, const mth::Point<T>& p2) noexcept
		{
			x1 = p1.x;
			y1 = p1.y;
			x2 = p2.x;
			y2 = p2.y;

			return *this;
		}

		T x1, y1, x2, y2;
	};


	template<typename T>
	class LineRef
	{
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	public:
		using num_t1 = T;
		using tag = tag::isLine;

		constexpr LineRef() noexcept = delete;
		constexpr LineRef(const LineRef&) noexcept = default;

		/**
		* @summary construct from x, y, width and height references
		*/
		constexpr LineRef(T& x1, T& y1, T& x2, T& y2) noexcept
			: x1(x1), y1(y1), x2(x2), y2(y2)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr LineRef(mth::Point<T>& p1, mth::Point<T>& p2) noexcept
			: LineRef(p1.x, p1.y, p2.x, p2.y)
		{
		}

		constexpr auto& translate(const mth::Point<T>& delta1, const mth::Point<T>& delta2) noexcept
		{
			x1 += delta1.x;
			y1 += delta1.y;
			x2 += delta2.x;
			y2 += delta2.y;
			return *this;
		}

		constexpr auto& pos(const mth::Point<T>& p1, const mth::Point<T>& p2) noexcept
		{
			x1 = p1.x;
			y1 = p1.y;
			x2 = p2.x;
			y2 = p2.y;

			return *this;
		}

		T& x1, & y1, & x2, & y2;
	};


	//----------------------------------------------
	//Implementation
	//----------------------------------------------

#ifdef SDL_h_
	template<>
	inline const SDL_Point* Line<int>::point_ptr() const noexcept
	{
		return reinterpret_cast<const SDL_Point* const>(this);
	}

	template<>
	inline SDL_Point* Line<int>::point_ptr() noexcept
	{
		return reinterpret_cast<SDL_Point*>(this);
	}
#endif // SDL_h_
}