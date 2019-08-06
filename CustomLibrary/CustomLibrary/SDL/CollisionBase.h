#pragma once

#include <CustomLibrary/utility.h>
#include <type_traits>

namespace ctl
{
	template<size_t size>
	class CollisionBase
	{
	public:
		CollisionBase() = default;

		template<typename ...T, typename = typename std::enable_if_t<std::conjunction_v<std::is_pointer<T>...>>>
		constexpr CollisionBase(const T & ... v)
			: m_dim{ reinterpret_cast<const int* const>(v)... }
		{}
		constexpr CollisionBase(const std::array<const int* const, size>& v)
			: m_dim{ v }
		{}

		template<typename ...T, typename = typename std::enable_if_t<std::conjunction_v<std::is_pointer<T>...>>>
		constexpr void set(const T & ... v) { m_dim = { reinterpret_cast<const int* const>(v)... }; }

		constexpr const auto& shapePtrs() const { return m_dim; }

		static constexpr auto dotRect(const std::array<const int* const, 2> & d, const std::array<const int* const, 4> & r) noexcept
		{
			return !(*d[0] < *std::get<0>(r) ||
				*d[0] > * std::get<0>(r) + *std::get<2>(r) ||
				*d[1] < *std::get<1>(r) ||
				*d[1] > * std::get<1>(r) + *std::get<3>(r));
		}

		static constexpr bool rectCir(const std::array<const int* const, 4> & r, const std::array<const int* const, 3> & c)
		{
			const auto halfRad = *c[2] >> 1;

			//Find closest x offset
			const auto xEdge = *c[0] + halfRad;

			const auto cX = xEdge < *r[0] ? *r[0] :
				xEdge > *r[0] + *r[2] ? *r[0] + *r[2] :
				xEdge;

			//Find closest y offset
			const auto yEdge = *c[1] + halfRad;

			const auto cY = yEdge < *r[1] ? *r[1] :
				yEdge > *r[1] + *r[3] ? *r[1] + *r[3] :
				yEdge;

			//If the closest point is inside the circle
			return power2(cX - *c[0] - halfRad) + power2(cY - *c[1] - halfRad) < power2(*c[2]) >> 2;
		}

		static constexpr bool rectRect(const std::array<const int* const, 4> & r1, const std::array<const int* const, 4> & r2) noexcept
		{
			return !(*r1[1] + *r1[3] <= *r2[1] || *r1[1] >= *r2[1] + *r2[3] || *r1[0] + *r1[2] <= *r2[0] || *r1[0] >= *r2[0] + *r2[2]);
		}

		static constexpr bool cirCir(const std::array<const int* const, 3> & c1, const std::array<const int* const, 3> & c2) noexcept
		{
			//If the distance between the centres of the circles is less than the sum of their rad
			//return power2(*c1[0] + (*c1[2] >> 1) - *c2[0] - (*c2[2] >> 1)) + power2(*c1[1] + (*c1[2] >> 1) - *c2[1] - (*c2[2] >> 1)) < power2(*c1[2] + *c2[2]) >> 2;

			return power2(*c1[0] - *c2[0]) + power2(*c1[1] - *c2[1]) < power2(*c1[2] + *c2[2]);
		}

		static constexpr bool dotDot(const std::array<const int* const, 2> & d1, const std::array<const int* const, 2> & d2) noexcept
		{
			return *d1[0] == *d2[0] && *d1[1] == *d2[1];
		}

		static constexpr bool dotCir(const std::array<const int* const, 2> & d, const std::array<const int* const, 3> & c) noexcept
		{
			const auto dx = std::abs(*d[0] - *c[0]);
			const auto dy = std::abs(*d[1] - *c[1]);

			if (dx > * c[2])
				return false;
			else if (dy > * c[2])
				return false;
			else if (dx + dy <= *c[2])
				return true;
			else if (power2(dx) + power2(dy) <= power2(*c[2]))
				return true;
			else
				return false;
		}

	protected:
		std::array<const int* const, size> m_dim;
	};

}