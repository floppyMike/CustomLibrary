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
			const auto [halfWidth, halfHight] = std::pair(r.w / 2, r.h / 2);
			const auto [distanceX, distanceY] = std::pair(std::abs(r.x + halfWidth - c.x), std::abs(r.y + halfHight - c.y));

			if (distanceX > (halfWidth + c.r)) return false;
			if (distanceY > (halfHight + c.r)) return false;

			if (distanceX <= halfWidth) return true;
			if (distanceY <= halfHight) return true;

			const auto cornerDistance_sq = power2(distanceX - halfWidth) + power2(distanceY - halfHight);

			return (cornerDistance_sq <= power2(c.r));
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

	/**
	* @summary checks for collision between two objects
	* @param "d" point
	* @param "c" circle
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<typename T1, typename T2>
	constexpr auto collision(const T1& o1, const T2& o2) noexcept
	{
		static_assert(hasSDLTag_v<T1> && hasSDLTag_v<T2>, "Object has no tag.");
		return _impl_<typename T1::Tag, typename T2::Tag>::_(o1, o2);
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
}