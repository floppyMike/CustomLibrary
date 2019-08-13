#pragma once

#include <type_traits>
#include <array>

#include <CustomLibrary/utility.h>

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
				xEdge > * r[0] + *r[2] ? *r[0] + *r[2] :
				xEdge;

			//Find closest y offset
			const auto yEdge = *c[1] + halfRad;

			const auto cY = yEdge < *r[1] ? *r[1] :
				yEdge > * r[1] + *r[3] ? *r[1] + *r[3] :
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

	class DotCol : public CollisionBase<2>
	{
		using CollisionBase<2>::dotRect;
		using CollisionBase<2>::dotCir;
		using CollisionBase<2>::dotDot;
		using CollisionBase<2>::rectRect;
		using CollisionBase<2>::rectCir;
		using CollisionBase<2>::cirCir;

	public:
		using CollisionBase::CollisionBase;

		auto inside(const CollisionBase<4>& r) const noexcept { return dotRect(m_dim, r.shapePtrs()); }
		auto inside(const CollisionBase<3>& c) const noexcept { return dotCir(m_dim, c.shapePtrs()); }
		auto inside(const CollisionBase<2>& d) const noexcept { return dotDot(m_dim, d.shapePtrs()); }
	};

	class BoxCol : public CollisionBase<4>
	{
		using CollisionBase<4>::dotRect;
		using CollisionBase<4>::dotCir;
		using CollisionBase<4>::dotDot;
		using CollisionBase<4>::rectRect;
		using CollisionBase<4>::rectCir;
		using CollisionBase<4>::cirCir;

	public:
		using CollisionBase::CollisionBase;

		bool inside(const CollisionBase<4>& r) const noexcept { return rectRect(m_dim, r.shapePtrs()); }
		bool inside(const CollisionBase<3>& c) const noexcept { return rectCir(m_dim, c.shapePtrs()); }
		bool inside(const CollisionBase<2>& d) const noexcept { return dotRect(d.shapePtrs(), m_dim); }
	};

	class CirCol : public CollisionBase<3>
	{
		using CollisionBase<3>::dotRect;
		using CollisionBase<3>::dotCir;
		using CollisionBase<3>::dotDot;
		using CollisionBase<3>::rectRect;
		using CollisionBase<3>::rectCir;
		using CollisionBase<3>::cirCir;

	public:
		using CollisionBase::CollisionBase;

		bool inside(const CollisionBase<4> & r) const noexcept { return rectCir(r.shapePtrs(), m_dim); }
		bool inside(const CollisionBase<3> & c) const noexcept { return cirCir(m_dim, c.shapePtrs()); }
		bool inside(const CollisionBase<2> & d) const noexcept { return dotCir(d.shapePtrs(), m_dim); }
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

	using DotColVar = _ColVar_<ctl::DotCol, ctl::SDLPoint>;
	using BoxColVar = _ColVar_<ctl::BoxCol, ctl::SDLRect>;
	using CirColVar = _ColVar_<ctl::CirCol, ctl::SDLCircle>;
}