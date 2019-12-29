#pragma once

#include <type_traits>

namespace ctl::sdl
{
	namespace Tags
	{
		struct isPoint {};
		struct isDim {};
		struct isRect {};
		struct isCircle {};
		struct isLine {};

		struct isFont {};
		struct isTexture {};
	}

	/**
	* @summary template for checking if "Tag" is defined in class
	* @tparam "T" object to check
	*/
	template<typename T, typename = void>
	struct hasSDLTag : std::false_type {};

	template<typename T>
	struct hasSDLTag<T, std::void_t<typename T::tag>> : std::true_type {};

	template<typename T>
	constexpr bool hasSDLTag_v = hasSDLTag<T>::value;
}