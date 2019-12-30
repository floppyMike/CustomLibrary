#pragma once

#include <type_traits>

namespace ctl::tag
{
	struct isPoint {};
	struct isDim {};
	struct isRect {};
	struct isCircle {};
	struct isLine {};

	struct isFont {};
	struct isTexture {};

	template<typename Tag, typename... Tags>
	constexpr bool has_tag_v = std::disjunction_v<std::is_same<Tag, Tags>...>;

	template<typename... T>
	constexpr bool has_shape_v = has_tag_v<isPoint, T...>
		|| has_tag_v<isRect, T...>
		|| has_tag_v<isCircle, T...>
		|| has_tag_v<isLine, T...>;

	template<typename T, typename = void>
	struct contains_tag : std::false_type {};

	template<typename T>
	struct contains_tag<T, std::void_t<typename T::tag>> : std::true_type {};

	template<typename... T>
	constexpr bool contains_tag_v = std::conjunction_v<contains_tag<T>...>;
}