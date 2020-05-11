#pragma once

#include <array>
#include <numeric>
#include "utility.h"

namespace ctl
{
	//Numeric vector for equations
	template<class TypeNum, size_t sizeArray>
	class NumVec
	{
	public:
		constexpr NumVec() = default;

		template<typename... S, typename = typename std::enable_if_t<std::conjunction_v<std::is_arithmetic<S>...>>>
		constexpr NumVec(const S&... ts) : m_m{ static_cast<TypeNum>(ts)... } {}

		template<typename S, typename = typename std::enable_if_t<std::is_arithmetic_v<S>>>
		constexpr NumVec(const ctl::NumVec<S, sizeArray> &e) : m_m(arrayCast<TypeNum, S, sizeArray>(e.data())) {}

		constexpr NumVec(const ctl::NumVec<TypeNum, sizeArray> &) = default;
		constexpr NumVec& operator=(const ctl::NumVec<TypeNum, sizeArray> &) = default;

		constexpr const auto& data() const noexcept { return m_m; }

		constexpr auto& operator[](const size_t &x) { return m_m.at(x); }
		constexpr const auto& operator[](const size_t &x) const { return m_m.at(x); }

		constexpr auto& operator+=(const NumVec<TypeNum, sizeArray> &v) noexcept
		{
			auto a = v.m_m.begin();
			for (auto& i : m_m)
				i += *(a++);

			return *this;
		}
		constexpr auto& operator-=(const NumVec<TypeNum, sizeArray>& v) noexcept
		{
			auto a = v.m_m.begin();
			for (auto& i : m_m)
				i -= *(a++);

			return *this;
		}

		constexpr auto operator==(const NumVec<TypeNum, sizeArray> &v) const noexcept { return m_m == v.m_m; }
		constexpr auto operator!=(const NumVec<TypeNum, sizeArray> &v) const noexcept { return m_m != v.m_m; }

		template<int y>
		constexpr NumVec<TypeNum, sizeArray + y> append(const NumVec<TypeNum, y> &v) const noexcept
		{
			NumVec<TypeNum, sizeArray + y> result;
			std::copy(m_m.begin(), m_m.end(), result.data().begin());
			std::copy(v.data().begin(), v.data().end(), result.data().begin() + sizeArray);
			return result;
		}

		constexpr auto sum() const noexcept { return std::accumulate(m_m.begin(), m_m.end(), static_cast<TypeNum>(0)); }
		constexpr auto product() const noexcept { return std::accumulate(m_m.begin(), m_m.end(), static_cast<TypeNum>(1), std::multiplies<>()); }

	private:
		std::array<TypeNum, sizeArray> m_m;
	};
}