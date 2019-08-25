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
	template<typename... T>
	class SDLShape
	{
	public:
		static_assert(std::conjunction_v<std::is_arithmetic<T>...>, "Type must be arithmetic");

	protected:
		SDLShape() = default;
	};

	//----------------------------------------------
	//Basic Shapes
	//----------------------------------------------

	template<typename T>
	class SDLPoint : public SDLShape<T>
	{
	public:
		using Val_T1 = T;
		using Tag = SDLTags::isPoint;

		constexpr SDLPoint() = default;
		constexpr SDLPoint(const SDLPoint&) = default;

		constexpr SDLPoint& operator=(const SDLPoint&) = default;

		/**
		* Copy constructs from coords.
		* @param [in] x x-coord
		* @param [in] y y-coord
		*/
		constexpr SDLPoint(const T& x, const T& y)
			: x(x), y(y)
		{
		}
		
		template<typename U>
		operator SDLPoint<U>() const noexcept;

		/**
		* @summary handles type overload to SDL_Point
		*/
		operator SDL_Point() const noexcept;

		/**
		* @summary handles type overload to SDL_FPoint
		*/
		operator SDL_FPoint() const noexcept;

		/**
		* @summary assignment operators
		*/
		constexpr auto& operator+=(const SDLPoint& p) noexcept;
		constexpr auto& operator-=(const SDLPoint& p) noexcept;

		/**
		* @summary arithmitic operators
		*/
		constexpr auto operator+(const SDLPoint& p) const noexcept;
		constexpr auto operator-(const SDLPoint& p) const noexcept;

		/**
		* @summary relational operators
		*/
		constexpr auto operator==(const SDLPoint& p) const noexcept;

		T x, y;
	};

	template<typename T>
	class SDLDim
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

		using Val_T1 = T;
		using Tag = SDLTags::isDim;

		constexpr SDLDim() = default;
		constexpr SDLDim(const SDLDim&) = default;

		constexpr SDLDim& operator=(const SDLDim&) = default;

		/**
		* @summary construct from width and height
		*/
		constexpr SDLDim(const T& pw, const T& ph) noexcept
			: w(pw), h(ph)
		{
		}

		Val_T1 w, h;
	};

	template<typename T1, typename T2>
	class SDLRect : public SDLShape<T1, T2>
	{
	public:
		using Val_T1 = T1;
		using Val_T2 = T2;
		using Tag = SDLTags::isRect;

		/**
		* @summary constructs the Ref from this point and dimension
		*/
		constexpr SDLRect() noexcept = default;

		/**
		* @summary copies values and constructs the Ref from this point and dimension
		*/
		constexpr SDLRect(const SDLRect& r) noexcept
			: m_p(r.m_p), m_d(r.m_d)
		{
		}
		constexpr SDLRect& operator=(const SDLRect& r) noexcept
		{
			m_p = r.m_p;
			m_d = r.m_d;

			return *this;
		}

		/**
		* @summary construct from x, y, width and height
		*/
		constexpr SDLRect(const T1& x, const T1& y, const T2& w, const T2& h) noexcept
			: m_p(x, y), m_d(w, h)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr SDLRect(const SDLPoint<T1>& p, const SDLDim<T2>& d) noexcept
			: SDLRect(p.x, p.y, d.w, d.h)
		{
		}

		/**
		* @summary handles type overload to SDL_Rect
		* @remarks must be of type int
		*/
		operator SDL_Rect() const noexcept;

		/**
		* @summary handles type overload to SDL_FRect
		* @remarks must be of type float
		*/
		operator SDL_FRect() const noexcept;

		/**
		* @summary point accessors
		* @returns reference to SDLPoint
		*/
		constexpr const auto& pos() const { return m_p; }
		constexpr auto& pos(const SDLPoint<T1>& p) { m_p = p; return *this; }

		/**
		* @summary dimension accessors
		* @returns reference to SDLDim
		*/
		constexpr const auto& dim() const { return m_d; }
		constexpr auto& dim(const SDLDim<T2>& d) { m_d = d; return *this; }

	private:
		SDLPoint<T1> m_p;
		SDLDim<T2> m_d;

	public:
		T1& x = m_p.x, & y = m_p.y;
		T2& w = m_d.w, & h = m_d.h;
	};

	template<typename T>
	class SDLLine : public SDLShape<T>
	{
	public:
		using Val_T1 = T;
		using Tag = SDLTags::isLine;

		constexpr SDLLine() noexcept = default;

		constexpr SDLLine(const SDLLine&) noexcept = default;

		constexpr SDLLine& operator=(const SDLLine&) noexcept = default;

		constexpr SDLLine(const T& x1, const T& y1, const T& x2, const T& y2) noexcept
			: m_p1(x1, y1), m_p2(x2, y2)
		{
		}

		constexpr SDLLine(const SDLPoint<T>& p1, const SDLPoint<T>& p2) noexcept
			: SDLLine(m_p1.x, m_p1.y, m_p2.x, m_p2.y)
		{
		}

		/**
		* @summary point accessors
		* @returns reference to SDLPoint
		*/
		constexpr const auto& pos1() const { return m_p1; }
		constexpr auto& pos1(const SDLPoint<T>& p) { m_p1 = p; return *this; }

		/**
		* @summary point accessors
		* @returns reference to SDLPoint
		*/
		constexpr const auto& pos2() const { return m_p2; }
		constexpr auto& pos2(const SDLPoint<T>& p) { m_p2 = p; return *this; }

		T& x1 = m_p1.x, & y1 = m_p1.y, & x2 = m_p2.x, & y2 = m_p2.y;

	private:
		SDLPoint<T> m_p1;
		SDLPoint<T> m_p2;
	};

	template <typename T1, typename T2>
	class SDLCircle : public SDLShape<T1, T2>
	{
	public:
		using Val_T1 = T1;
		using Val_T2 = T2;
		using Tag = SDLTags::isCircle;

		constexpr SDLCircle() noexcept = default;

		constexpr SDLCircle(const SDLCircle&) noexcept = default;

		constexpr SDLCircle& operator=(const SDLCircle&) noexcept = default;

		constexpr SDLCircle(const T1& x1, const T1& y1, const T2& r) noexcept
			: m_p(x1, y1), r(r)
		{
		}

		constexpr SDLCircle(const SDLPoint<T1>& p, const T2& r) noexcept
			: SDLCircle(p.x, p.y, r)
		{
		}

		/**
		* @summary point accessors
		* @returns reference to SDLPoint
		*/
		constexpr const auto& pos() const { return m_p; }
		constexpr auto& pos(const SDLPoint<T1>& p) { m_p = p; return *this; }

		T1& x = m_p.x, & y = m_p.y;
		T2 r;

	private:
		SDLPoint<T1> m_p;
	};

	//----------------------------------------------
	//Basic Shape Reference Combinations
	//----------------------------------------------

	template<typename T1, typename T2>
	class SDLRectRef : SDLShape<T1, T2>
	{
	public:
		using Val_T1 = T1;
		using Val_T2 = T2;
		using Tag = SDLTags::isRect;

		constexpr SDLRectRef() noexcept = delete;
		constexpr SDLRectRef(const SDLRectRef&) noexcept = default;

		constexpr SDLRectRef& operator=(const SDLRectRef&) noexcept = default;

		/**
		* @summary construct from x, y, width and height references
		*/
		constexpr SDLRectRef(T1& x, T1& y, T2& w, T2& h) noexcept
			: x(x), y(y), w(w), h(h)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr SDLRectRef(SDLPoint<T1>& p, SDLDim<T2>& d) noexcept
			: SDLRectRef(p.x, p.y, d.w, d.h)
		{
		}

		std::reference_wrapper<T1> x, y;
		std::reference_wrapper<T2> w, h;
	};

	template<typename T>
	class SDLLineRef : SDLShape<T>
	{
	public:
		using Val_T1 = T;
		using Tag = SDLTags::isLine;

		constexpr SDLLineRef() noexcept = delete;
		constexpr SDLLineRef(const SDLLineRef&) noexcept = default;

		constexpr SDLLineRef& operator=(const SDLLineRef&) noexcept = default;

		/**
		* @summary construct from x, y, width and height references
		*/
		constexpr SDLLineRef(T& x1, T& y1, T& x2, T& y2) noexcept
			: x1(x1), y1(y1), x2(x2), y2(y2)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr SDLLineRef(SDLPoint<T>& p1, SDLPoint<T>& p2) noexcept
			: SDLLineRef(p1.x, p1.y, p2.x, p2.y)
		{
		}

		std::reference_wrapper<T> x1, y1, x2, y2;
	};

	template<typename T1, typename T2>
	class SDLCircleRef : SDLShape<T1, T2>
	{
	public:
		using Val_T1 = T1;
		using Val_T2 = T2;
		using Tag = SDLTags::isCircle;

		constexpr SDLCircleRef() noexcept = delete;
		constexpr SDLCircleRef(const SDLCircleRef&) noexcept = default;

		constexpr SDLCircleRef& operator=(const SDLCircleRef&) noexcept = default;

		/**
		* @summary construct from x, y, width and height references
		*/
		constexpr SDLCircleRef(T1& x, T1& y, T2& r) noexcept
			: x(x), y(y), r(r)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr SDLCircleRef(SDLPoint<T1>& p, T2& r) noexcept
			: SDLCircleRef(p.x, p.y, r)
		{
		}

		std::reference_wrapper<T1> x, y;
		std::reference_wrapper<T2> r;
	};

	/****************************************
	* Implementations
	****************************************/

	template<>
	inline SDLPoint<int>::operator SDL_Point() const noexcept
	{
		return *reinterpret_cast<const SDL_Point * const>(this);
	}
	template<typename T>
	inline SDLPoint<T>::operator SDL_Point() const noexcept
	{
		return { static_cast<int>(x), static_cast<int>(y) };
	}

	template<typename T>
	template<typename U>
	inline SDLPoint<T>::operator SDLPoint<U>() const noexcept
	{
		return { static_cast<U>(x), static_cast<U>(y) };
	}

	template<>
	inline SDLPoint<float>::operator SDL_FPoint() const noexcept
	{
		return *reinterpret_cast<const SDL_FPoint * const>(this);
	}

	template<typename T>
	inline constexpr auto& SDLPoint<T>::operator+=(const SDLPoint& p) noexcept
	{
		x += p.x;
		y += p.y;

		return *this;
	}

	template<typename T>
	inline constexpr auto& SDLPoint<T>::operator-=(const SDLPoint& p) noexcept
	{
		x -= p.x;
		y -= p.y;

		return *this;
	}

	template<typename T>
	inline constexpr auto SDLPoint<T>::operator+(const SDLPoint& p) const noexcept
	{
		return SDLPoint(x + p.x, y + p.y);
	}

	template<typename T>
	inline constexpr auto SDLPoint<T>::operator-(const SDLPoint& p) const noexcept
	{
		return SDLPoint(x - p.x, y - p.y);
	}

	template<typename T>
	inline constexpr auto SDLPoint<T>::operator==(const SDLPoint& p) const noexcept
	{
		return x == p.x && y == p.y;
	}

	template<typename T1, typename T2>
	inline SDLRect<T1, T2>::operator SDL_Rect() const noexcept
	{
		return { static_cast<int>(x), static_cast<int>(y),
			static_cast<int>(w), static_cast<int>(h) };
	}

	template<typename T1, typename T2>
	inline SDLRect<T1, T2>::operator SDL_FRect() const noexcept
	{
		return { static_cast<float>(this->x), static_cast<float>(this->y),
			static_cast<float>(this->w), static_cast<float>(this->h) };
	}

}