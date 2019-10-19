#pragma once

#include <array>
#include <string_view>
#include <string>
#include <bitset>
#include <fstream>

#include "Error.h"

namespace ctl
{
	//----------------------------------------Math Extensions---------------------------------------------

	constexpr double PI = 3.1415926535897932;

	template<typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr T mapVal(const T &val, const T &oldMin, const T &oldMax, const T &newMin, const T &newMax) { return newMin + (newMax - newMin) * val / (oldMax - oldMin); }

	template<typename T, typename = typename std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T to_degrees(const T &val) noexcept { return val / PI * 180; }
	template<typename T, typename = typename std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T to_radians(const T &val) noexcept { return val / 180 * PI; }

	template<class CharT, class Traits, class Allocator>
	bool is_number(const std::basic_string<CharT, Traits, Allocator>& str) { return !str.empty() && str.find_first_not_of("0123456789.") == std::string::npos; }
	//bool is_number(const char& ch) { constexpr std::string_view nums = "0123456789."; return std::find(nums.begin(), nums.end(), ch) != nums.end(); }

	//double& sigmoid(double &x) noexcept { return x = 1 / (1 + std::exp(-x)); }

	template<typename Type, typename = typename std::enable_if_t<std::is_integral_v<Type>>>
	auto toBinary(const Type &num) { return std::bitset<sizeof(Type) * 8>(num); }

	template<typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr auto power2(const T &val) noexcept { return val * val; }

	template<typename T1, typename T2>
	auto roundUp(const T1& num, const T2& multiple)
	{
		if (multiple == 0)
			return num;

		const auto remainder = std::abs(num) % multiple;
		if (remainder == 0)
			return num;

		return num < 0 ? remainder - std::abs(num) : num + multiple - remainder;
	}

	//--------------------------Iterator Interface for container adaptors------------------------------

	template<typename Adaptor>
	class IteratableAdaptor : public Adaptor
	{
	public:
		//using iterator = Adaptor::container_type::iterator;
		//using const_iterator = Adaptor::container_type::const_iterator;

		auto begin() { return this->c.begin(); }
		auto end() { return this->c.end(); }

		auto begin() const { return this->c.begin(); }
		auto end() const { return this->c.end(); }
	};

	//--------------------------------------General----------------------------------------------------

	template <std::size_t... Is>
	struct _indices_ {};

	template <std::size_t N, std::size_t... Is>
	struct _build_indices_ : _build_indices_<N - 1, N - 1, Is...> {};

	template <std::size_t... Is>
	struct _build_indices_<0, Is...> : _indices_<Is...> {};

	template<typename T, typename U, size_t i, size_t... Is>
	constexpr std::array<T, i> _array_cast_helper_(const std::array<U, i>& a, _indices_<Is...>) 
	{ 
		return { static_cast<T>(std::get<Is>(a))... }; 
	}

	//Constexpr array-wide cast
	template<typename T, typename U, size_t i>
	constexpr std::array<T, i> arrayCast(const std::array<U, i>& a) 
	{ 
		return _array_cast_helper_<T>(a, _build_indices_<i>()); 
	}

	//Dereference ptr even if already dereferenced
	template<typename T>
	constexpr T& deref_ptr(T* ptr) { return *ptr; }
	//Dereference ptr even if already dereferenced
	template<typename T>
	constexpr T& deref_ptr(T& ptr) { return ptr; }

	using char8_t = unsigned char;
	using u8string = std::basic_string<char8_t>;

	template<class T>
	constexpr auto& unmove(T&& t) noexcept { return t; }

	template<typename Container, typename Iter>
	void fastRemove(Container& c, Iter i)
	{
		assert(i != c.end() && "End iterator cannot be deleted.");

		std::iter_swap(i, c.end() - 1);
		c.erase(c.end() - 1);
	}

	template<typename T, typename Pred>
	bool ifTrueBecome(T& val1, const T& val2, const Pred& p)
	{
		const bool cond = p(val1, val2);
		if (cond)
			val1 = val2;
		return cond;
	}

	template<typename Ele, typename Traits>
	auto getInputStreamContent(std::basic_istream<Ele, Traits>& in)
	{
		std::string content;

		assert(in && "getInputStreamContent: file couldn't be accessed.");

		in.seekg(0, std::ios::end);
		content.resize(static_cast<size_t>(in.tellg()));
		in.seekg(0, std::ios::beg);

		in.read(&content[0], content.size());

		return content;
	}

	std::string toHexadecimal(int val)
	{
		static constexpr std::string& (*hex)(int&, std::string&) =
			[](int& val, std::string& str) constexpr->std::string &
		{
			if (val == 0)
				return str;

			const char& rem = val % 16;
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

	template<typename To, typename... Args>
	auto multiStaticCast(const Args&... args)
	{
		return std::make_tuple(static_cast<To>(args)...);
	}
}