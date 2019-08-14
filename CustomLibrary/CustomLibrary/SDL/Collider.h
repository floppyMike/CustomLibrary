#pragma once

#include <type_traits>
#include <array>

#include <CustomLibrary/utility.h>

#include "BasicTypes.h"

namespace ctl
{
	template<typename Point_T, typename Rect_T1, typename Rect_T2>
	constexpr auto pointRect(const SDLPoint<Point_T>* const d,
		const SDLRect<Rect_T1, Rect_T2>* const r) noexcept
	{
		return !(d->x < r->x ||
			d->x > r->x + r->w ||
			d->y < r->y ||
			d->y > r->y + r->h);
	}

	template<typename Rect_T1, typename Rect_T2, typename Cir_T1, typename Cir_T2>
	constexpr bool rectCir(const SDLRect<Rect_T1, Rect_T2>* const r,
		const SDLCircle<Cir_T1, Cir_T2>* const c)
	{
		const auto halfRad = c->r >> 1;

		//Find closest x offset
		const auto xEdge = c->x + halfRad;
		const auto cX = xEdge < r->x ? r->x :
			xEdge > r->x + r->w ? r->x + r->w :
			xEdge;

		//Find closest y offset
		const auto yEdge = c->y + halfRad;
		const auto cY = yEdge < r->y ? r->y :
			yEdge > r->y + r->h ? r->y + r->h :
			yEdge;

		//If the closest point is inside the circle
		return power2(cX - c->x - halfRad) + power2(cY - c->y - halfRad) < power2(c->r) >> 2;
	}

	template<typename Rect1_T1, typename Rect1_T2, typename Rect2_T1, typename Rect2_T2>
	constexpr bool rectRect(const SDLRect<Rect1_T1, Rect1_T2>* const r1,
		const SDLRect<Rect2_T1, Rect2_T2>* const r2) noexcept
	{
		return !(r1->y + r1->h <= r2->y ||
			r1->y >= r2->y + r2->h ||
			r1->x + r1->w <= r2->x ||
			r1->x >= r2->x + r2->w);
	}

	template<typename Cir1_T1, typename Cir1_T2, typename Cir2_T1, typename Cir2_T2>
	constexpr bool cirCir(const SDLCircle<Cir1_T1, Cir1_T2>* const c1,
		const SDLCircle<Cir2_T1, Cir2_T2>* const c2) noexcept
	{
		//If the distance between the centres of the circles is less than the sum of their rad
		//return power2(*c1[0] + (*c1[2] >> 1) - *c2[0] - (*c2[2] >> 1)) + power2(*c1[1] + (*c1[2] >> 1) - *c2[1] - (*c2[2] >> 1)) < power2(*c1[2] + *c2[2]) >> 2;

		return power2(c1->x - c2->x) + power2(c1->y - c2->y) < power2(c1->r + c2->r);
	}

	template<typename Point1_T, typename Point2_T>
	constexpr bool pointPoint(const SDLPoint<Point1_T>* const d1, const SDLPoint<Point2_T>* const d2) noexcept
	{
		return d1 == d2;
	}

	template<typename Point_T, typename Cir_T1, typename Cir_T2>
	constexpr bool pointCir(const SDLPoint<Point_T>* const d, const SDLCircle<Cir_T1, Cir_T2>* const c) noexcept
	{
		const auto dx = std::abs(d->x - c->x);
		const auto dy = std::abs(d->y - c->y);

		if (dx > c->r || dy > c->r)
			return false;

		else if (dx + dy <= c->r || power2(dx) + power2(dy) <= power2(c->r))
			return true;

		else
			return false;
	}

	template<typename T, typename Derived>
	class _BasicCollision_
	{
		/**
		* @summary converts this to derived type
		* @returns derived this ptr
		*/
		constexpr const auto* dThis() const noexcept { return static_cast<const Derived* const>(this); }
		constexpr auto* dThis() noexcept { return static_cast<Derived* const>(this); }

	public:
		/**
		* @summary ptr accessors
		*/
		constexpr const auto& ptr() const noexcept { return m_dim; }
		constexpr auto& ptr(const T* const g) noexcept
		{ 
			m_dim = g;
			return *dThis();
		}

	protected:
		/**
		* @summary constructs empty ptr
		*/
		_BasicCollision_() = default;

		/**
		* @summary constructs with ptr
		* @param "g" the ptr
		*/
		constexpr _BasicCollision_(const T* const g)
			: m_dim(g)
		{
		}

		const T* m_dim = nullptr;
	};

	//Predefinitions
	template<typename T1, typename T2>
	class RectCol;

	template<typename T1, typename T2>
	class CirCol;

	template<typename T>
	class PointCol : public _BasicCollision_<SDLPoint<T>, PointCol>
	{
	public:
		using _BasicCollision_::_BasicCollision_;

		template<typename T1, typename T2>
		auto inside(const RectCol<T1, T2>& r) const noexcept { return dotRect(m_dim, r.m_dim); }

		template<typename T1, typename T2>
		auto inside(const CirCol<T1, T2>& c) const noexcept { return dotCir(m_dim, c.m_dim); }

		template<typename T1>
		auto inside(const PointCol<T1>& d) const noexcept { return dotDot(m_dim, d.m_dim); }
	};

	template<typename Rect_T1, typename Rect_T2>
	class RectCol : public _BasicCollision_<SDLRect<Rect_T1, Rect_T2>, RectCol>
	{
	public:
		using _BasicCollision_::_BasicCollision_;

		template<typename T1, typename T2>
		bool inside(const RectCol<T1, T2>& r) const noexcept { return rectRect(m_dim, r.m_dim); }

		template<typename T1, typename T2>
		bool inside(const CirCol<T1, T2>& c) const noexcept { return rectCir(m_dim, c.m_dim); }

		template<typename T1>
		bool inside(const PointCol<T1>& d) const noexcept { return dotRect(d.m_dim, m_dim); }
	};

	template<typename Cir_T1, typename Cir_T2>
	class CirCol : public _BasicCollision_<SDLCircle<Cir_T1, Cir_T2>, CirCol>
	{
	public:
		using _BasicCollision_::_BasicCollision_;

		template<typename T1, typename T2>
		bool inside(const RectCol<T1, T2>& r) const noexcept { return rectCir(r.m_dim, m_dim); }

		template<typename T1, typename T2>
		bool inside(const CirCol<T1, T2>& c) const noexcept { return cirCir(m_dim, c.m_dim); }

		template<typename T1>
		bool inside(const PointCol<T1>& d) const noexcept { return dotCir(d.m_dim, m_dim); }
	};


	template<typename Col, typename Type>
	class _ColVar_ : public Col
	{
	public:
		_ColVar_()
		{
			if constexpr (std::is_same_v<Type, ctl::SDLPoint>)
				this->m_dim = { &m_val.x, &m_val.y };

			if constexpr (std::is_same_v<Type, ctl::SDLRect>)
				this->m_dim = { &m_val.x, &m_val.y, &m_val.w, &m_val.h };

			if constexpr (std::is_same_v<Type, ctl::SDLCircle>)
				this->m_dim = { &m_val.x, &m_val.y, &m_val.r };
		}

		_ColVar_(const Type& v)
			: m_val(v)
			, _ColVar_()
		{
		}

		constexpr auto& transform(const ctl::NumVec<int, 2>& vec)
		{
			m_val.x += vec[0];
			m_val.y += vec[1];
		}

		constexpr const auto& var() const { return m_val; }
		constexpr auto& var(const Type& newVal) { m_val = newVal; return *this; }

	private:
		Type m_val;
	};

	//using DotColVar = _ColVar_<ctl::DotCol, ctl::SDLPoint>;
	//using BoxColVar = _ColVar_<ctl::BoxCol, ctl::SDLRect>;
	//using CirColVar = _ColVar_<ctl::CirCol, ctl::SDLCircle>;
}