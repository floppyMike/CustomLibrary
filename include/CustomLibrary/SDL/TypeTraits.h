#pragma once

#include <cassert>
#include <tuple>
#include <type_traits>

namespace ctl::sdl
{
	namespace detail
	{
		template<template<typename, typename> class Util, typename Impl, typename... T>
		struct _Stick_ : Util<Impl, T>...
		{
		};

		template<template<typename, typename> class Util, typename Impl, template<typename...> class Tuple,
				 typename... _Types>
		auto _peel_(Tuple<_Types...>) -> _Stick_<Util, Impl, _Types...>;

		template<template<typename, typename> class Util, typename Impl, typename _Type>
		auto _peel_(_Type) -> Util<Impl, _Type>;

		template<template<typename, typename> class Util, typename Impl, typename Outer>
		struct _UnPeeler_
			: decltype(_peel_<Util, Impl>(std::declval<typename Outer::tag_t>()))
			, _UnPeeler_<
				  Util, Impl,
				  std::conditional_t<std::is_same_v<typename Outer::base_t, Outer>, void, typename Outer::base_t>>
		{
		};

		template<template<typename, typename> class Util, typename Impl>
		struct _UnPeeler_<Util, Impl, void>
		{
		};
	} // namespace detail

	namespace tag
	{
		struct isUnassigned
		{
		};

		struct isFont
		{
		};
		struct isTexture
		{
		};
		struct isText
		{
		};
		struct isRenderer
		{
		};
		struct isRenderDelay
		{
		};
		struct isMusic
		{
		};
		struct isMultiShape
		{
		};
		struct isAnimation
		{
		};

		template<typename T, typename = void>
		struct contains_tag : std::false_type
		{
		};

		template<typename T>
		struct contains_tag<T, std::void_t<typename T::tag>> : std::true_type
		{
		};

		template<typename... T>
		constexpr bool contains_tag_v = std::conjunction_v<contains_tag<T>...>;
	} // namespace tag

	template<typename T, template<typename, typename...> class crtp_t, typename... other_t>
	struct crtp
	{
		T *		 underlying() noexcept { return static_cast<T *>(this); }
		const T *underlying() const noexcept { return static_cast<const T *>(this); }

	private:
		crtp() {}
		friend crtp_t<T, other_t...>;
	};

	template<typename T, template<typename> class Ex1, template<typename> class... Ex>
	struct MixBuilder
	{
		using type = Ex1<typename MixBuilder<T, Ex...>::type>;
	};

	template<typename T, template<typename> class Ex>
	struct MixBuilder<T, Ex>
	{
		using type = Ex<T>;
	};

	class Renderer;

	template<typename T, template<typename, typename> class Func>
	class FunctionalR : public detail::_UnPeeler_<Func, FunctionalR<T, Func>, std::remove_cv_t<T>>
	{
	public:
		FunctionalR() = default;
		FunctionalR(T *o, Renderer *r)
			: m_r(r)
			, m_o(o)
		{
		}
		FunctionalR(T *o)
			: m_o(o)
		{
		}

		auto *obj() const noexcept { return m_o; }
		auto &obj(T *o) noexcept
		{
			m_o = o;
			return *this;
		}

		constexpr auto &renderer(Renderer *const r) noexcept
		{
			m_r = r;
			return *this;
		}

		constexpr auto *renderer() const noexcept
		{
			assert(m_r != nullptr && "Renderer isn't assigned.");
			return m_r;
		}

	private:
		Renderer *m_r = nullptr;
		T *		  m_o;
	};

	template<typename T, template<typename, typename> class Func>
	class FunctionalO : public detail::_UnPeeler_<Func, FunctionalO<T, Func>, std::remove_cv_t<T>>
	{
	public:
		FunctionalO() = default;
		FunctionalO(T *o)
			: m_o(o)
		{
		}

		auto *obj() noexcept { return m_o; }
		auto &obj(T *o) noexcept
		{
			m_o = o;
			return *this;
		}

	private:
		T *m_o;
	};
} // namespace ctl::sdl
