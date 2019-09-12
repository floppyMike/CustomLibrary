/** @file BasicTypes.h
* Defines basic shapes.
* Definies basic shapes used for nearly all rendering operations
* @copyright GNU Public License
* @author Michaellu
* @version 1.0
*/

#pragma once

#include <type_traits>
#include "SDLTags.h"

namespace ctl
{
	namespace sdl
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

			const SDL_Point* pointPtr() const noexcept;
			SDL_Point* pointPtr() noexcept;

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

			constexpr RectRef& operator=(const RectRef&) noexcept = default;

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

			std::reference_wrapper<T1> x, y;
			std::reference_wrapper<T2> w, h;
		};

		template<typename T>
		class LineRef : Shape<T>
		{
		public:
			using num_t1 = T;
			using tag = Tags::isLine;

			constexpr LineRef() noexcept = delete;
			constexpr LineRef(const LineRef&) noexcept = default;

			constexpr LineRef& operator=(const LineRef&) noexcept = default;

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

			std::reference_wrapper<T> x1, y1, x2, y2;
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

			std::reference_wrapper<T1> x, y;
			std::reference_wrapper<T2> r;
		};

		
		//----------------------------------------------
		//Basic Shapes
		//----------------------------------------------

		template<typename T1, typename T2>
		class Rect : Shape<T1, T2>
		{
		public:
			using num_t1 = T1;
			using num_t2 = T2;
			using tag = Tags::isRect;

			/**
			* @summary constructs the Ref from this point and dimension
			*/
			constexpr Rect() noexcept = default;

			/**
			* @summary copies values and constructs the Ref from this point and dimension
			*/
			constexpr Rect(const Rect& r) noexcept
				: m_p(r.m_p), m_d(r.m_d)
			{
			}
			constexpr Rect& operator=(const Rect& r) noexcept
			{
				m_p = r.m_p;
				m_d = r.m_d;

				return *this;
			}

			/**
			* @summary construct from x, y, width and height
			*/
			constexpr Rect(const T1& x, const T1& y, const T2& w, const T2& h) noexcept
				: m_p(x, y), m_d(w, h)
			{
			}

			/**
			* @summary construct from point and dimension
			*/
			constexpr Rect(const Point<T1>& p, const Dim<T2>& d) noexcept
				: Rect(p.x, p.y, d.w, d.h)
			{
			}

			/**
			* @summary handles type overload to SDL_Rect
			* @remarks must be of type int
			*/
			operator SDL_Rect() const noexcept;

			const SDL_Rect* rectPtr() const noexcept;

			/**
			* @summary handles type overload to SDL_FRect
			* @remarks must be of type float
			*/
			operator SDL_FRect() const noexcept;

			/**
			* @summary point accessors
			* @returns reference to Point
			*/
			constexpr const auto& pos() const { return m_p; }
			constexpr auto& pos(const Point<T1>& p) { m_p = p; return *this; }

			/**
			* @summary dimension accessors
			* @returns reference to Dim
			*/
			constexpr const auto& dim() const { return m_d; }
			constexpr auto& dim(const Dim<T2>& d) { m_d = d; return *this; }

		private:
			Point<T1> m_p;
			Dim<T2> m_d;

		public:
			T1& x = m_p.x, & y = m_p.y;
			T2& w = m_d.w, & h = m_d.h;
		};

		template<typename T>
		class Line : public Shape<T>
		{
		public:
			using num_t1 = T;
			using tag = Tags::isLine;

			constexpr Line() noexcept = default;

			constexpr Line(const Line&) noexcept = default;

			constexpr Line& operator=(const Line&) noexcept = default;

			constexpr Line(const T& x1, const T& y1, const T& x2, const T& y2) noexcept
				: m_p1(x1, y1), m_p2(x2, y2)
			{
			}

			constexpr Line(const Point<T>& p1, const Point<T>& p2) noexcept
				: Line(m_p1.x, m_p1.y, m_p2.x, m_p2.y)
			{
			}

			/**
			* @summary point accessors
			* @returns reference to Point
			*/
			constexpr const auto& pos1() const { return m_p1; }
			constexpr auto& pos1(const Point<T>& p) { m_p1 = p; return *this; }

			/**
			* @summary point accessors
			* @returns reference to Point
			*/
			constexpr const auto& pos2() const { return m_p2; }
			constexpr auto& pos2(const Point<T>& p) { m_p2 = p; return *this; }

			T& x1 = m_p1.x, & y1 = m_p1.y, & x2 = m_p2.x, & y2 = m_p2.y;

		private:
			Point<T> m_p1;
			Point<T> m_p2;
		};

		template <typename T1, typename T2>
		class Circle : public Shape<T1, T2>
		{
		public:
			using num_t1 = T1;
			using num_t2 = T2;
			using tag = Tags::isCircle;

			constexpr Circle() noexcept = default;

			constexpr Circle(const Circle&) noexcept = default;

			constexpr Circle& operator=(const Circle&) noexcept = default;

			constexpr Circle(const T1& x1, const T1& y1, const T2& r) noexcept
				: m_p(x1, y1), r(r)
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
			constexpr const auto& pos() const { return m_p; }
			constexpr auto& pos(const Point<T1>& p) { m_p = p; return *this; }

			T1& x = m_p.x, & y = m_p.y;
			T2 r;

		private:
			Point<T1> m_p;
		};

		//----------------------------------------------
		//Basic Shape Reference Combinations
		//----------------------------------------------


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

		template<typename T>
		inline const SDL_Point* Point<T>::pointPtr() const noexcept
		{
			return reinterpret_cast<const SDL_Point * const>(this);
		}

		template<typename T>
		inline SDL_Point* Point<T>::pointPtr() noexcept
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

		template<typename T1, typename T2>
		inline Rect<T1, T2>::operator SDL_Rect() const noexcept
		{
			return { static_cast<int>(x), static_cast<int>(y),
				static_cast<int>(w), static_cast<int>(h) };
		}

		template<>
		inline const SDL_Rect* Rect<int, int>::rectPtr() const noexcept
		{
			return reinterpret_cast<const SDL_Rect * const>(this);
		}

		template<>
		inline Rect<int, int>::operator SDL_Rect() const noexcept
		{
			return *reinterpret_cast<const SDL_Rect * const>(this);
		}

		template<typename T1, typename T2>
		inline Rect<T1, T2>::operator SDL_FRect() const noexcept
		{
			return { static_cast<float>(this->x), static_cast<float>(this->y),
				static_cast<float>(this->w), static_cast<float>(this->h) };
		}

		template<>
		inline Rect<float, float>::operator SDL_FRect() const noexcept
		{
			return *reinterpret_cast<const SDL_FRect * const>(this);
		}
		template<typename T>
		template<typename U>
		inline Point<T>::operator Point<U>() const noexcept
		{
			return { static_cast<U>(x), static_cast<U>(y) };
		}

	}
}