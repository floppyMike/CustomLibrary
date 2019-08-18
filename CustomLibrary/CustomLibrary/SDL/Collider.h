#pragma once

#include <type_traits>
#include <array>

#include <CustomLibrary/utility.h>

#include "BasicTypes.h"

namespace ctl
{
	/**
	* @summary handles if no collision specialization is handled for parameters
	*/
	template<typename T1, typename T2>
	struct _impl_
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1&, const U2&) noexcept
		{
			static_assert(true, "collision for type not supported.");
			return false;
		}
	};

	/**
	* @summary handles collision between point and rectangle
	* @param "d" point
	* @param "r" rectangle
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<SDLTags::isPoint, SDLTags::isRect>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& d, const U2& r) noexcept
		{
			return !(d.x < r.x ||
				d.x > r.x + r.w ||
				d.y < r.y ||
				d.y > r.y + r.h);
		}
	};
	/**
	* @summary handles if parameters are in reverse
	*/
	template<>
	struct _impl_<SDLTags::isRect, SDLTags::isPoint>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r, const U2& d) noexcept
		{
			return _impl_<SDLTags::isPoint, SDLTags::isRect>::_(d, r);
		}
	};

	/**
	* @summary handles collision between rectangle and circle
	* @param "r" rectangle
	* @param "c" circle
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<SDLTags::isRect, SDLTags::isCircle>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r, const U2& c) noexcept
		{
			//
			// Algorithm not working!!!
			//
			const auto halfRad = c.r >> 1;

			//Find closest x offset
			const auto xEdge = c.x + halfRad;
			const auto cX = xEdge < r.x ? r.x :
				xEdge > r.x + r.w ? r.x + r.w :
				xEdge;

			//Find closest y offset
			const auto yEdge = c.y + halfRad;
			const auto cY = yEdge < r.y ? r.y :
				yEdge > r.y + r.h ? r.y + r.h :
				yEdge;

			//If the closest point is inside the circle
			return power2(cX - c.x - halfRad) + power2(cY - c.y - halfRad) < power2(c.r) >> 2;
		}
	};
	/**
	* @summary handles if parameters are in reverse
	*/
	template<>
	struct _impl_<SDLTags::isCircle, SDLTags::isRect>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r, const U2& c) noexcept
		{
			return _impl_<SDLTags::isRect, SDLTags::isCircle>::_(c, r);
		}
	};

	/**
	* @summary handles collision between rectangle and rectangle
	* @param "r1" rectangle 1
	* @param "r2" rectangle 2
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<SDLTags::isRect, SDLTags::isRect>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r1, const U2& r2) noexcept
		{
			return !(r1.y + r1.h <= r2.y ||
				r1.y >= r2.y + r2.h ||
				r1.x + r1.w <= r2.x ||
				r1.x >= r2.x + r2.w);
		}
	};

	/**
	* @summary handles collision between circle and circle
	* @param "c1" circle 1
	* @param "c2" circle 2
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<SDLTags::isCircle, SDLTags::isCircle>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& c1, const U2& c2) noexcept
		{
			//If the distance between the centres of the circles is less than the sum of their rad
			//return power2(*c1[0] + (*c1[2] >> 1) - *c2[0] - (*c2[2] >> 1)) + power2(*c1[1] + (*c1[2] >> 1) - *c2[1] - (*c2[2] >> 1)) < power2(*c1[2] + *c2[2]) >> 2;

			return power2(c1.x - c2.x) + power2(c1.y - c2.y) < power2(c1.r + c2.r);
		}
	};

	/**
	* @summary handles collision between point and point
	* @param "d1" point 1
	* @param "d2" point 2
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<SDLTags::isPoint, SDLTags::isPoint>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& d1, const U2& d2) noexcept
		{
			return d1 == d2;
		}
	};

	/**
	* @summary handles collision between point and circle
	* @param "d" point
	* @param "c" circle
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<SDLTags::isPoint, SDLTags::isCircle>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& d, const U2& c) noexcept
		{
			const auto dx = std::abs(d.x - c.x);
			const auto dy = std::abs(d.y - c.y);

			if (dx > c.r || dy > c.r)
				return false;

			else if (dx + dy <= c.r || power2(dx) + power2(dy) <= power2(c.r))
				return true;

			else
				return false;
		}
	};
	template<>
	struct _impl_<SDLTags::isCircle, SDLTags::isPoint>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& c, const U2& p) noexcept
		{
			return _impl_<SDLTags::isCircle, SDLTags::isPoint>::_(p, c);
		}
	};

	template<typename T1, typename T2>
	constexpr auto collision(const T1& d, const T2& r) noexcept
	{
		static_assert(hasSDLTag_v<T1> && hasSDLTag_v<T2>, "Object has no tag.");
		return _impl_<typename T1::Tag, typename T2::Tag>::_(d, r);
	}

	template<typename Geo_T>
	class Collider
	{
	public:
		static_assert(hasSDLTag_v<Geo_T>, "Type requires a \"using Tags\" alias");

		using value_type = Geo_T;

		/**
		* @summary constructs empty ptr
		*/
		Collider() = default;

		/**
		* @summary constructs with ptr
		* @param "g" the ptr
		*/
		constexpr Collider(Geo_T* const g) noexcept
			: m_dim(g)
		{
		}

		/**
		* @summary checks if object is inside this
		* @param "col" other Collider
		* @returns if "col" is inside this
		*/
		template<typename T>
		constexpr bool inside(const Collider<T>& col) const noexcept 
		{ 
			return collision(*m_dim, *col.m_dim);
		}

		/**
		* @summary ptr accessors
		*/
		constexpr const auto& ptr() const noexcept { return m_dim; }
		constexpr auto& ptr(Geo_T* const g) noexcept
		{ 
			m_dim = g;
			return *this;
		}

	private:
		Geo_T* m_dim = nullptr;
	};


	template<typename Geo_t>
	class ColliderVar : public Geo_t
	{
	public:
		using Val_T1 = Geo_t;

		/**
		* @summary constructs with 0 and configures collider
		*/
		constexpr ColliderVar() noexcept
			: m_collider(this)
		{
		}

		/**
		* @summary constructs with val and configures collider
		* @param "g" value to copy construct with
		*/
		constexpr ColliderVar(const Geo_t& g) noexcept
			: Geo_t(g)
			, m_collider(this)
		{
		}

		/**
		* @summary constructs with val and configures collider
		* @param "g" value reference to copy construct with
		*/
		constexpr ColliderVar(Geo_t& g) noexcept
			: Geo_t(g)
			, m_collider(this)
		{
		}

		/**
		* @summary construct with individual values
		* @param "argv" values based on type
		*/
		template<typename... Arg>
		constexpr ColliderVar(const Arg&... argv) noexcept
			: Geo_t(argv...)
			, m_collider(this)
		{
		}

		/**
		* @summary construct with individual values
		* @param "argv" values based on type
		*/
		template<typename... Arg>
		constexpr ColliderVar(Arg&... argv) noexcept
			: Geo_t(argv...)
			, m_collider(this)
		{
		}

		/**
		* @summary accesses collider object		
		*/
		constexpr const auto& collider() const { return m_collider; }

	private:
		Collider<Val_T1> m_collider;
	};

	//template<typename Geo_T>
	//template<typename T>
	//inline constexpr bool Collider<Geo_T>::inside(const Collider<T>& col) const noexcept
	//{
	//	if constexpr (std::is_same_v<T::Tag, SDLTags::isRect>)
	//			 if constexpr (std::is_same_v<Geo_T::Tag, SDLTags::isPoint>)  return Col::pointRect(m_dim, col.ptr());
	//		else if constexpr (std::is_same_v<Geo_T::Tag, SDLTags::isCircle>) return Col::rectCir(col.ptr(), m_dim);
	//		else															  return Col::rectRect(m_dim, col.ptr());

	//	else if constexpr (std::is_same_v<T::Tag, SDLTags::isCircle>)
	//			 if constexpr (std::is_same_v<Geo_T::Tag, SDLTags::isPoint>)  return Col::pointCir(m_dim, col.ptr());
	//		else if constexpr (std::is_same_v<Geo_T::Tag, SDLTags::isCircle>) return Col::cirCir(m_dim, col.ptr());
	//		else															  return Col::rectCir(col.ptr(), m_dim);

	//	else
	//			 if constexpr (std::is_same_v<Geo_T::Tag, SDLTags::isPoint>)  return Col::pointPoint(m_dim, col.ptr());
	//		else if constexpr (std::is_same_v<Geo_T::Tag, SDLTags::isCircle>) return Col::pointCir(col.ptr(), m_dim);
	//		else															  return Col::pointRect(col.ptr(), m_dim);
	//}
}