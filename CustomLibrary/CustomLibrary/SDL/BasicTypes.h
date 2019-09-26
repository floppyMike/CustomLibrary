/** @file BasicTypes.h
* Defines basic shapes.
* Definies basic shapes used for nearly all rendering operations
* @copyright GNU Public License
* @author Michaellu
* @version 1.0
*/

#pragma once

#include "SDLTags.h"

namespace ctl::sdl
{
	template<typename... T>
	class Shape
	{
	public:
		static_assert(std::conjunction_v<std::is_arithmetic<T>...>, "Type must be arithmetic");

	protected:
		Shape() = default;
	};


	//----------------------------------------------
	//Basic Shapes
	//----------------------------------------------

	template<typename T>
	class Point : Shape<T>
	{
	public:
		using num_t1 = T;
		using tag = Tags::isPoint;

		constexpr Point() = default;
		constexpr Point(const Point&) = default;

		constexpr Point& operator=(const Point&) = default;

		/**
		* Copy constructs from coords.
		* @param [in] x x-coord
		* @param [in] y y-coord
		*/
		constexpr Point(const T& x, const T& y)
			: x(x), y(y)
		{
		}

		template<typename U>
		operator Point<U>() const noexcept;

		/**
		* @summary handles type overload to SDL_Point
		*/
		operator SDL_Point() const noexcept;

		/**
		* @summary handles type overload to SDL_FPoint
		*/
		operator SDL_FPoint() const noexcept;

		const SDL_Point* pointPtr() const noexcept
		{
			static_assert(false, "Type must be int.");
			return nullptr;
		}
		SDL_Point* pointPtr() noexcept
		{
			static_assert(false, "Type must be int.");
			return nullptr;
		}

		/**
		* @summary assignment operators
		*/
		constexpr auto& operator+=(const Point& p) noexcept;
		constexpr auto& operator-=(const Point& p) noexcept;

		/**
		* @summary arithmitic operators
		*/
		constexpr auto operator+(const Point& p) const noexcept;
		constexpr auto operator-(const Point& p) const noexcept;

		/**
		* @summary relational operators
		*/
		constexpr auto operator==(const Point& p) const noexcept;

		T x, y;
	};

	template<typename T>
	class Dim : Shape<T>
	{
	public:
		using num_t1 = T;
		using tag = Tags::isDim;

		constexpr Dim() = default;
		constexpr Dim(const Dim&) = default;

		constexpr Dim& operator=(const Dim&) = default;

		/**
		* @summary construct from width and height
		*/
		constexpr Dim(const T& pw, const T& ph) noexcept
			: w(pw), h(ph)
		{
		}

		T w, h;
	};


	//----------------------------------------------
	//References
	//----------------------------------------------

	template<typename T1, typename T2>
	class RectRef : Shape<T1, T2>
	{
	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag = Tags::isRect;

		constexpr RectRef() noexcept = delete;
		constexpr RectRef(const RectRef&) noexcept = default;

		/**
		* @summary construct from x, y, width and height references
		*/
		constexpr RectRef(T1& x, T1& y, T2& w, T2& h) noexcept
			: x(x), y(y), w(w), h(h)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr RectRef(Point<T1>& p, Dim<T2>& d) noexcept
			: RectRef(p.x, p.y, d.w, d.h)
		{
		}

		T1& x, & y;
		T2& w, & h;
	};

	template<typename T>
	class LineRef : Shape<T>
	{
	public:
		using num_t1 = T;
		using tag = Tags::isLine;

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
		constexpr LineRef(Point<T>& p1, Point<T>& p2) noexcept
			: LineRef(p1.x, p1.y, p2.x, p2.y)
		{
		}

		T& x1, & y1, & x2, & y2;
	};

	template<typename T1, typename T2>
	class CircleRef : Shape<T1, T2>
	{
	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag = Tags::isCircle;

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
		constexpr CircleRef(Point<T1>& p, T2& r) noexcept
			: CircleRef(p.x, p.y, r)
		{
		}

		T1& x, & y;
		T2& r;
	};


	//----------------------------------------------
	//Basic Shapes
	//----------------------------------------------

	template<typename T1, typename T2>
	class Rect : Shape<T1, T2>
	{
		using type = Rect<T1, T2>;

	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag = Tags::isRect;

		constexpr Rect() noexcept = default;

		constexpr Rect(const Rect& r) noexcept = default;
		constexpr Rect& operator=(const Rect& r) noexcept = default;

		constexpr Rect(const T1& x, const T1& y, const T2& w, const T2& h) noexcept
			: x(x), y(y), w(w), h(h)
		{
		}

		constexpr Rect(const Point<T1>& p, const Dim<T2>& d) noexcept
			: Rect(p.x, p.y, d.w, d.h)
		{
		}

		operator SDL_Rect() const noexcept
		{
			return { static_cast<int>(x), static_cast<int>(y),
				static_cast<int>(w), static_cast<int>(h) };
		}
		operator SDL_FRect() const noexcept
		{
			return { static_cast<float>(x), static_cast<float>(y),
				static_cast<float>(w), static_cast<float>(h) };
		}

		SDL_Rect* rectPtr() noexcept
		{
			static_assert(false, "Must be of type int.");
			return nullptr;
		}
		const SDL_Rect* rectPtr() const noexcept
		{
			static_assert(false, "Must be of type int.");
			return nullptr;
		}

		constexpr Point<T1> pos() const
		{
			return { x, y };
		}
		constexpr auto& pos(const Point<T1>& p) noexcept
		{
			x = p.x;
			y = p.y;
			return *this;
		}

		constexpr Dim<T2> dim() const noexcept
		{
			return { w, h };
		}
		constexpr type& dim(const Dim<T2>& d) noexcept
		{
			w = d.w;
			h = d.h;
			return *this;
		}

		T1 x, y;
		T2 w, h;
	};


	template<typename T>
	class Line : Shape<T>
	{
		using type = Line<T>;
		using base = LineRef<T>;

	public:
		using num_t1 = T;
		using tag = Tags::isLine;

		constexpr Line() noexcept = default;

		constexpr Line(const Line&) noexcept = default;

		constexpr Line& operator=(const Line& l) noexcept = default;

		constexpr Line(const T& x1, const T& y1, const T& x2, const T& y2) noexcept
			: x1(x1), y1(y1), x2(x2), y2(y2)
		{
		}

		constexpr Line(const Point<T>& p1, const Point<T>& p2) noexcept
			: Line(p1.x, p1.y, p2.x, p2.y)
		{
		}

		template<typename U>
		constexpr operator Line<U>() noexcept
		{
			return { static_cast<U>(x1), static_cast<U>(y1), 
				static_cast<U>(x2), static_cast<U>(y2) };
		}

		constexpr SDL_Point* pointPtr() noexcept
		{
			return reinterpret_cast<SDL_Point*>(this);
		}

		constexpr const SDL_Point* pointPtr() const noexcept
		{
			return reinterpret_cast<const SDL_Point*>(this);
		}

		constexpr Point<T> pos1() const noexcept
		{
			return { x1, y1 };
		}
		constexpr auto& pos1(const Point<T>& p) noexcept
		{
			x1 = p.x;
			y1 = p.y;
			return *this;
		}

		constexpr Point<T> pos2() const noexcept
		{
			return { x2, y2 };
		}
		constexpr auto& pos2(const Point<T>& p) noexcept
		{
			x2 = p.x;
			y2 = p.y;
			return *this;
		}

		T x1, y1, x2, y2;
	};


	template <typename T1, typename T2>
	class Circle : Shape<T1, T2>
	{
		using type = Circle<T1, T2>;

	public:
		using num_t1 = T1;
		using num_t2 = T2;
		using tag = Tags::isCircle;

		constexpr Circle() noexcept = default;
		constexpr Circle(const Circle&) noexcept = default;

		constexpr Circle& operator=(const Circle& c) noexcept = default;

		constexpr Circle(const T1& x1, const T1& y1, const T2& r) noexcept
			: x(x1), y(y1), r(r)
		{
		}

		constexpr Circle(const Point<T1>& p, const T2& r) noexcept
			: Circle(p.x, p.y, r)
		{
		}

		/**
		* @summary point accessors
		* @returns reference to Point
		*/
		constexpr const auto& pos() const noexcept
		{
			return Point<T1>(x, y);
		}
		constexpr auto& pos(const Point<T1>& p) noexcept
		{
			x = p.x;
			y = p.y;
			return *this;
		}

		T1 x, y;
		T2 r;
	};


	//----------------------------------------------
	//Implementation
	//----------------------------------------------

	template<typename T>
	inline Point<T>::operator SDL_Point() const noexcept
	{
		return { static_cast<int>(x), static_cast<int>(y) };
	}

	template<typename T>
	inline Point<T>::operator SDL_FPoint() const noexcept
	{
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	template<>
	inline const SDL_Point* Point<int>::pointPtr() const noexcept
	{
		return reinterpret_cast<const SDL_Point * const>(this);
	}

	template<>
	inline SDL_Point* Point<int>::pointPtr() noexcept
	{
		return reinterpret_cast<SDL_Point*>(this);
	}

	template<typename T>
	inline constexpr auto& Point<T>::operator+=(const Point& p) noexcept
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	template<typename T>
	inline constexpr auto& Point<T>::operator-=(const Point& p) noexcept
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}

	template<typename T>
	inline constexpr auto Point<T>::operator+(const Point& p) const noexcept
	{
		return Point(x + p.x, y + p.y);
	}

	template<typename T>
	inline constexpr auto Point<T>::operator-(const Point& p) const noexcept
	{
		return Point(x - p.x, y - p.y);
	}

	template<typename T>
	inline constexpr auto Point<T>::operator==(const Point& p) const noexcept
	{
		return x == p.x && y == p.y;
	}

	template<typename T>
	template<typename U>
	inline Point<T>::operator Point<U>() const noexcept
	{
		return { static_cast<U>(x), static_cast<U>(y) };
	}

	template<>
	inline SDL_Rect* Rect<int, int>::rectPtr() noexcept
	{
		return reinterpret_cast<SDL_Rect*>(this);
	}

	template<>
	inline const SDL_Rect* Rect<int, int>::rectPtr() const noexcept
	{
		return reinterpret_cast<const SDL_Rect*>(this);
	}

}