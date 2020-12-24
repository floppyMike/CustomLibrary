#pragma once

#include <array>
#include <string_view>
#include <string>
#include <bitset>
#include <fstream>
#include <tuple>

#include "Traits.h"
#include "Error.h"

#include <cmath>

namespace ctl
{
	// -----------------------------------------------------------------------------
	// Math
	// -----------------------------------------------------------------------------

	static constexpr double PI = 3.1415926535897932;

	/**
	 * @brief Maps values from its previous range to new range
	 *
	 * @param val value
	 * @param old_min previous range minimum
	 * @param old_max previous range maximum
	 * @param new_min new range minimum
	 * @param new_max new range maximum
	 * @return Value type
	 */
	template<arithmetic _T, arithmetic _U, arithmetic _Z, arithmetic _I, arithmetic _O>
	constexpr auto map_val(const _T &val, const _U &old_min, const _Z &old_max, const _I &new_min, const _O &new_max)
		-> std::common_type_t<_T, _U, _Z, _I, _O>
	{
		return new_min + (new_max - new_min) * val / (old_max - old_min);
	}

	/**
	 * @brief Turn radians to degrees
	 *
	 * @param val Value to turn
	 * @return Value type
	 */
	template<arithmetic _T>
	constexpr auto rad_to_deg(const _T &val) noexcept
	{
		return val / PI * 180;
	}
	/**
	 * @brief Turn degrees to radians
	 *
	 * @param val Value to turn
	 * @return Value type
	 */
	template<arithmetic _T>
	constexpr auto deg_to_rad(const _T &val) noexcept
	{
		return val / 180. * PI;
	}

	/**
	 * @brief Check if char represents a number
	 *
	 * @param ch Character to look at
	 * @return bool
	 */
	constexpr auto is_number(char ch) -> bool { return (ch >= '0' && ch <= '9') || ch == '.'; }

	/**
	 * @brief Checks if string is a number
	 *
	 * @tparam _Str String type
	 * @param s String
	 * @return bool
	 */
	template<string _Str>
	constexpr auto is_number(const _Str &s) -> bool
	{
		for (auto i = std::begin(s), end = std::end(s); i != end; ++i)
			if (!is_number(*i))
				return false;

		return true;
	}

	/**
	 * @brief Sigmoid function
	 *
	 * @tparam _T Arithmetic number
	 * @param x number
	 * @return result
	 */
	template<arithmetic _T>
	auto sigmoid(_T x) noexcept -> _T
	{
		return 1 / (1 + std::exp(-x));
	}

	/**
	 * @brief Converts integer to binary
	 *
	 * @tparam _T integer type
	 * @param num number
	 * @return bitset
	 */
	template<std::integral _T>
	auto to_binary(const _T &num)
	{
		return std::bitset<sizeof(_T) * 8>(num);
	}

	/**
	 * @brief Rounds number to multiple
	 *
	 * @param num number to round
	 * @param multiple multiple to use
	 * @return result
	 */
	template<typename _T, typename _U>
	auto round_up(_T num, _U multiple)
	{
		if (multiple == 0)
			return num;

		return std::round(num / multiple) * multiple;
	}

	// -----------------------------------------------------------------------------
	// Iteratable Adaptor
	// -----------------------------------------------------------------------------

	/**
	 * @brief Provides iterator methods for
	 * @tparam Container Adaptor
	 */
	template<typename Adaptor>
	class IteratableAdaptor : public Adaptor
	{
	public:
		using iterator		 = typename Adaptor::container_type::iterator;
		using const_iterator = typename Adaptor::container_type::const_iterator;

		auto begin() { return this->c.begin(); }
		auto end() { return this->c.end(); }

		auto begin() const { return this->c.begin(); }
		auto end() const { return this->c.end(); }
	}; // namespace ctl

	// -----------------------------------------------------------------------------
	// Array Cast
	// -----------------------------------------------------------------------------

	template<std::size_t... Is>
	struct _indices_
	{
	};

	template<std::size_t N, std::size_t... Is>
	struct _build_indices_ : _build_indices_<N - 1, N - 1, Is...>
	{
	};

	template<std::size_t... Is>
	struct _build_indices_<0, Is...> : _indices_<Is...>
	{
	};

	template<typename T, typename _U, size_t _i, size_t... Is>
	constexpr auto _array_cast_helper_(const std::array<_U, _i> &a, _indices_<Is...>) -> std::array<T, _i>
	{
		return { static_cast<T>(std::get<Is>(a))... };
	}

	/**
	 * @brief Cast whole std::array
	 *
	 * @tparam T New type
	 * @param a Array to cast
	 * @return std::array<T, i>
	 */
	template<typename T, typename _U, size_t _i>
	constexpr auto array_cast(const std::array<_U, _i> &a) -> std::array<T, _i>
	{
		return _array_cast_helper_<T>(a, _build_indices_<_i>());
	}

	// -----------------------------------------------------------------------------
	// General
	// -----------------------------------------------------------------------------

	/**
	 * @brief Dereference ptr
	 *
	 * @param ptr Pointer to dereference
	 * @return _T&
	 */
	template<typename _T>
	constexpr auto deref(_T *ptr) -> _T &
	{
		return *ptr;
	}
	/**
	 * @brief Return reference back
	 *
	 * @param ptr Reference to return
	 * @return _T&
	 */
	template<typename _T>
	constexpr auto deref(_T &ptr) -> _T &
	{
		return ptr;
	}

	/**
	 * @brief Turn r-value reference to l-value reference
	 *
	 * @param t r-value to turn
	 * @return _T&
	 */
	template<typename _T>
	constexpr auto unmove(_T &&t) noexcept -> auto &
	{
		return t;
	}

	/**
	 * @brief Optimizes removal from continous container
	 *
	 * @tparam Container
	 * @param c Container value
	 * @param i Container iterator
	 */
	template<typename Container>
	requires requires(Container c)
	{
		{
			std::begin(c)
		}
		->std::contiguous_iterator;
	}
	void fast_remove(Container &c, decltype(std::begin(std::declval<Container>())) i)
	{
		assert(i != std::end(c) && "End iterator cannot be deleted.");

		std::iter_swap(i, std::end(c) - 1);
		c.erase(std::end(c) - 1);
	}

	template<typename _T, typename _U, std::predicate Pred>
	constexpr auto if_true(_T &val1, const _U &val2, Pred p)
	{
		const bool cond = p(val1, val2);
		if (cond)
			val1 = val2;
		return cond;
	}

	/**
	 * @brief Quickly turn istream to string
	 *
	 * @param in istream object
	 * @return string
	 */
	template<typename _Ele, typename _Traits>
	auto fast_stream_extract(std::basic_istream<_Ele, _Traits> &in)
	{
		std::string content;

		assert(in && "Stream is empty.");

		in.seekg(0, std::ios::end);
		content.resize(static_cast<size_t>(in.tellg()));
		in.seekg(0, std::ios::beg);

		in.read(&content[0], content.size());

		return content;
	}

	/**
	 * @brief Turn value into hex string
	 * 
	 * @param val value to turn
	 * @return std::string 
	 */
	auto to_hex(int val) -> std::string
	{
		static constexpr std::string &(*hex)(int &, std::string &) =
			[](int &val, std::string &str) constexpr->std::string &
		{
			if (val == 0)
				return str;

			const auto rem = val % 16;
			val /= 16;

			hex(val, str);

			if (rem > 9)
				str.push_back(rem - 10 + 'a');
			else
				str.push_back(rem + '0');

			return str;
		};

		if (val == 0)
			return std::string(1, '0');

		std::string buf;
		return hex(val, buf);
	}
} // namespace ctl
