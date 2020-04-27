#pragma once

#include <tuple>

namespace ctl::sdl
{
	namespace detail
	{
		template<template<typename, typename> class Util, typename Impl, typename Outer>
		struct _UnPeeler_
			: Util<Impl, typename Outer::tag_t>
			, _UnPeeler_<Util, Impl, std::conditional_t<std::is_same_v<typename Outer::base_t, Outer>, void, typename Outer::base_t>>
		{
		};

		template<template<typename, typename> class Util, typename Impl>
		struct _UnPeeler_<Util, Impl, void>
		{
		};

		template<typename Real, typename This>
		class _Func_
		{
		public:
			_Func_() = default;
			_Func_(Real* o)
				: m_o(o)
			{
			}

			auto& ref() noexcept { return *m_o; }
			auto& ref(Real* o) noexcept { m_o = o; return *static_cast<This*>(this); }

		protected:
			Real* m_o;
		};
	}

	namespace tag
	{
		struct isUnassigned {};

		struct isFont {};
		struct isTexture {};
		struct isRenderer {};
		struct isRenderDelay {};
		struct isMusic {};
		struct isMultiShape {};

		template<typename T, typename = void>
		struct contains_tag : std::false_type {};

		template<typename T>
		struct contains_tag<T, std::void_t<typename T::tag>> : std::true_type {};

		template<typename... T>
		constexpr bool contains_tag_v = std::conjunction_v<contains_tag<T>...>;
	}

	template <typename T, template<typename, typename...> class crtp_t, typename... other_t>
	struct crtp
	{
		T* underlying() noexcept { return static_cast<T*>(this); }
		const T* underlying() const noexcept { return static_cast<const T*>(this); }

	private:
		crtp() {}
		friend crtp_t<T, other_t...>;
	};
}