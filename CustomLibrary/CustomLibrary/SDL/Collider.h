#pragma once

#include "BasicTypes.h"
#include "SDLTraits.h"
#include <CustomLibrary/Traits.h>

namespace ctl::sdl
{
	/**
	* @summary handles if no collision specialization is handled for parameters
	*/
	template<typename T1, typename T2>
	struct _impl_
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1&, const U2&) noexcept;
	};

	/**
	* @summary handles collision between point and rectangle
	* @param "d" point
	* @param "r" rectangle
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<Tags::isPoint, Tags::isRect>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& d, const U2& r) noexcept;
	};
	/**
	* @summary handles if parameters are in reverse
	*/
	template<>
	struct _impl_<Tags::isRect, Tags::isPoint>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r, const U2& d) noexcept
		{
			return _impl_<Tags::isPoint, Tags::isRect>::_(d, r);
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
	struct _impl_<Tags::isRect, Tags::isCircle>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r, const U2& c) noexcept;
	};
	/**
	* @summary handles if parameters are in reverse
	*/
	template<>
	struct _impl_<Tags::isCircle, Tags::isRect>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r, const U2& c) noexcept
		{
			return _impl_<Tags::isRect, Tags::isCircle>::_(c, r);
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
	struct _impl_<Tags::isRect, Tags::isRect>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& r1, const U2& r2) noexcept;
	};

	/**
	* @summary handles collision between circle and circle
	* @param "c1" circle 1
	* @param "c2" circle 2
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<Tags::isCircle, Tags::isCircle>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& c1, const U2& c2) noexcept;
	};

	/**
	* @summary handles collision between point and point
	* @param "d1" point 1
	* @param "d2" point 2
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<Tags::isPoint, Tags::isPoint>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& d1, const U2& d2) noexcept;
	};

	/**
	* @summary handles collision between point and circle
	* @param "d" point
	* @param "c" circle
	* @remarks object for partial function specialization
	* @returns if collision is taking place
	*/
	template<>
	struct _impl_<Tags::isPoint, Tags::isCircle>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& d, const U2& c) noexcept;
	};
	template<>
	struct _impl_<Tags::isCircle, Tags::isPoint>
	{
		template<typename U1, typename U2>
		static constexpr bool _(const U1& c, const U2& p) noexcept
		{
			return _impl_<Tags::isCircle, Tags::isPoint>::_(p, c);
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
		return _impl_<typename T1::tag, typename T2::tag>::_(o1, o2);
	}

	template<typename Geo_T>
	class Collider : public ReliesOn<Geo_T, Collider<Geo_T>>
	{
		static_assert(hasSDLTag_v<Geo_T>, "Type requires a \"using Tags\" alias");

		using base1 = ReliesOn<Geo_T, Collider>;

	public:
		using value_t = Geo_T;

		/**
		* @summary constructs empty ptr
		*/
		Collider() noexcept = default;

		/**
		* @summary checks if object is inside this
		* @param "col" other Collider
		* @returns if "col" is inside this
		*/
		template<typename T>
		constexpr bool inside(const Collider<T>& col) const noexcept
		{
			return collision(*this->get<Geo_T>(), *col.get<Geo_T>());
		}

		using base1::set;
	};


	/**********************************************************
	* Implementation
	**********************************************************/

	template<typename T1, typename T2>
	template<typename U1, typename U2>
	inline constexpr bool _impl_<T1, T2>::_(const U1& d, const U2& r) noexcept
	{
		static_assert(true, "collision for type not supported.");
		return false;
	}

	template<typename U1, typename U2>
	inline constexpr bool _impl_<Tags::isPoint, Tags::isRect>::_(const U1& d, const U2& r) noexcept
	{
		return !(d.x < r.x ||
			d.x > r.x + r.w ||
			d.y < r.y ||
			d.y > r.y + r.h);
	}
	template<typename U1, typename U2>
	inline constexpr bool _impl_<Tags::isRect, Tags::isCircle>::_(const U1& r, const U2& c) noexcept
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
	template<typename U1, typename U2>
	inline constexpr bool _impl_<Tags::isRect, Tags::isRect>::_(const U1& r1, const U2& r2) noexcept
	{
		return !(r1.y + r1.h <= r2.y ||
			r1.y >= r2.y + r2.h ||
			r1.x + r1.w <= r2.x ||
			r1.x >= r2.x + r2.w);
	}
	template<typename U1, typename U2>
	inline constexpr bool _impl_<Tags::isCircle, Tags::isCircle>::_(const U1& c1, const U2& c2) noexcept
	{
		return power2(c1.x - c2.x) + power2(c1.y - c2.y) < power2(c1.r + c2.r);
	}
	template<typename U1, typename U2>
	inline constexpr bool _impl_<Tags::isPoint, Tags::isPoint>::_(const U1& d1, const U2& d2) noexcept
	{
		return d1 == d2;
	}
	template<typename U1, typename U2>
	inline constexpr bool _impl_<Tags::isPoint, Tags::isCircle>::_(const U1& d, const U2& c) noexcept
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

}