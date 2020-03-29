#pragma once

#include "Point.h"
#include "Tags.h"

namespace ctl::mth
{
	template <typename T1, typename T2>
	class Circle
	{
		static_assert(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>, "Type must be arithmetic");

	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag = tag::isCircle;

		constexpr Circle() noexcept = default;
		constexpr Circle(const Circle&) noexcept = default;

		constexpr Circle& operator=(const Circle& c) noexcept = default;

		constexpr Circle(const T1& x1, const T1& y1, const T2& r) noexcept
			: x(x1), y(y1), r(r)
		{
		}

		constexpr Circle(const mth::Point<T1>& p, const T2& r) noexcept
			: Circle(p.x, p.y, r)
		{
		}

		/**
		* @summary point accessors
		* @returns reference to mth::Point
		*/
		constexpr auto pos() const noexcept
		{
			return mth::Point<T1>(x, y);
		}

		constexpr void pos(const mth::Point<T1>& p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		constexpr auto dim() const noexcept
		{
			return r;
		}

		constexpr void dim(T2 rad) noexcept
		{
			r = rad;
		}

		constexpr void translate(const mth::Point<T1>& delta) noexcept
		{
			x += delta.x;
			y += delta.y;
		}

		constexpr void resize(T2 delta) noexcept
		{
			r += delta;
		}

		T1 x, y;
		T2 r;
	};


	template<typename T1, typename T2>
	class CircleRef
	{
		static_assert(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>, "Type must be arithmetic");

	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag = tag::isCircle;

		constexpr CircleRef() noexcept = delete;
		constexpr CircleRef(const CircleRef&) noexcept = default;

		constexpr CircleRef& operator=(const CircleRef&) noexcept = default;

		/**
		* @summary construct from x, y, width and height references
		*/
		constexpr CircleRef(T1& x, T1& y, T2& r) noexcept
			: x(x), y(y), r(r)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr CircleRef(mth::Point<T1>& p, T2& r) noexcept
			: CircleRef(p.x, p.y, r)
		{
		}

		constexpr void translate(const mth::Point<T1>& delta) noexcept
		{
			x += delta.x;
			y += delta.y;
		}

		constexpr void pos(const mth::Point<T1>& p) noexcept
		{
			x = p.x;
			y = p.y;
		}

		const void resize(T2 delta) noexcept
		{
			r += delta;
		}

		T1& x, & y;
		T2& r;
	};
}