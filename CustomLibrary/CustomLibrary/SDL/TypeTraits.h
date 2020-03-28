#pragma once

#include <tuple>

namespace ctl::sdl
{
	namespace detail
	{
		template<template<typename> class Outer, typename Inner>
		auto _peel_(Outer<Inner>)->std::tuple<Inner, Outer<void>>;

		template<typename Inner>
		auto _peel_(Inner)->std::tuple<void, Inner>;

		template<template<typename, typename> class Util, typename Impl, typename Outer>
		struct _UnPeeler_
			: Util<Impl, std::tuple_element_t<1, decltype(_peel_(std::declval<Outer>()))>>
			, _UnPeeler_<Util, Impl, std::tuple_element_t<0, decltype(_peel_(std::declval<Outer>()))>>
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

}