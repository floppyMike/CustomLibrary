#pragma once

#include <type_traits>

namespace ctl
{
	namespace SDLTags
	{
		struct isPoint {};
		struct isDim {};
		struct isRect {};
		struct isCircle {};
	}

	template<typename T, typename = void>
	struct hasSDLTag : std::false_type {};

	template<typename T>
	struct hasSDLTag<T, std::void_t<typename T::Tag>> : std::true_type {};

	template<typename T>
	inline constexpr bool hasSDLTag_v = hasSDLTag<T>::value;
}