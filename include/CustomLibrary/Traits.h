#pragma once

#include <type_traits>
#include <utility>
#include <cassert>

// -----------------------------------------------------------------------------
// Method Detection Preperation
// -----------------------------------------------------------------------------

#define HAS_METHOD_TYPE(method)           \
	template<typename T, typename... Arg> \
	using has_##method = decltype(std::declval<T &>().method(std::declval<Arg>()...));

// -----------------------------------------------------------------------------
// Forward Member Function
// -----------------------------------------------------------------------------

#define FORWARDING_MEMBER_FUNCTION(Inner, inner, function, qualifiers)                                               \
	template<typename... Args,                                                                                       \
			 typename return_type = decltype(std::declval<Inner qualifiers>().function(std::declval<Args &&>()...))> \
	constexpr decltype(auto) function(Args &&... args) qualifiers noexcept(                                          \
		noexcept(std::declval<Inner qualifiers>().function(std::forward<Args>(args)...))                             \
		and (std::is_reference<return_type>::value or std::is_nothrow_move_constructible<return_type>::value))       \
	{                                                                                                                \
		return /*static_cast<Inner qualifiers>*/ (inner).function(std::forward<Args>(args)...);                      \
	}

#define FORWARDING_MEMBER_FUNCTIONS_C(Inner, inner, function, reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, reference)        \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const reference)

#define FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, reference)  \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, reference)          \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const reference)    \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, volatile reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const volatile reference)

#define FORWARDING_MEMBER_FUNCTIONS(Inner, inner, function)   \
	FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, &) \
	FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, &&)

//#define ALIAS_TEMPLATE_FUNCTION(highLevelF, lowLevelF) \
//template<typename... Args> \
//inline auto highLevelF(Args&&... args) -> decltype(lowLevelF(std::forward<Args>(args)...)) \
//{ \
//    return lowLevelF(std::forward<Args>(args)...); \
//}

namespace ctl
{
	/**
	 * @brief Class used as a Tag in Template Metaprogramming
	 */
	struct Nonesuch
	{
		Nonesuch(Nonesuch &&)	   = delete;
		Nonesuch(const Nonesuch &) = delete;

		~Nonesuch() = delete;

		void operator=(const Nonesuch &) = delete;
		void operator=(Nonesuch &&) = delete;
	};

	template<typename T>
	concept complete_type = requires(T t)
	{
		sizeof(t);
	};

	template<typename T>
	concept arithmetic = std::is_arithmetic_v<std::remove_reference_t<T>>;

	template<typename T>
	concept string = std::is_same_v<std::decay_t<decltype(*std::begin(std::declval<T>()))>, char>;

	template<typename T, typename U>
	concept aligned_with = std::alignment_of_v<T> == std::alignment_of_v<U>;

	template<typename T, typename... EqualTypes>
	concept matches = std::disjunction_v<std::is_same<std::remove_cv_t<T>, EqualTypes>...>;

	template<typename T>
	using strip_t = std::remove_cv_t<std::decay_t<T>>;

	// -----------------------------------------------------------------------------
	// CRTP
	// -----------------------------------------------------------------------------

	/**
	 * @brief Provides cast to derived in CRTP
	 *
	 * @tparam T Derived class
	 * @tparam crtpType Base class
	 */
	template<typename T, template<typename> class crtpType>
	struct crtp
	{
		[[deprecated("Please use \"underlying\"")]] T &		 _() noexcept { return static_cast<T &>(*this); }
		[[deprecated("Please use \"underlying\"")]] const T &_() const noexcept
		{
			return static_cast<const T &>(*this);
		}

		auto underlying() noexcept -> T * { return static_cast<T *>(this); }
		auto underlying() const noexcept -> const T * { return static_cast<const T *>(this); }

	private:
		crtp() = default;
		friend crtpType<T>;
	};

	// -----------------------------------------------------------------------------
	// CRTP like Mixins
	// -----------------------------------------------------------------------------

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

	// -----------------------------------------------------------------------------
	// Method or Function Detection Pattern
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename Default, typename AlwaysVoid, template<typename...> class Op, typename... Args>
		struct Detector
		{
			using value_t = std::false_type;
			using type	  = Default;
		};

		template<typename Default, template<typename...> class Op, typename... Args>
		struct Detector<Default, std::void_t<Op<Args...>>, Op, Args...>
		{
			using value_t = std::true_type;
			using type	  = Op<Args...>;
		};

	} // namespace detail

	template<template<typename...> class Op, typename... Args>
	using is_detected = typename detail::Detector<Nonesuch, void, Op, Args...>::value_t;

	template<template<typename...> class Op, typename... Args>
	constexpr auto is_detected_v = is_detected<Op, Args...>::value;

	template<template<typename...> class Op, typename... Args>
	using detected_t = typename detail::Detector<Nonesuch, void, Op, Args...>::type;

	template<typename Default, template<typename...> class Op, typename... Args>
	using detected_or = detail::Detector<Default, void, Op, Args...>;

	// -----------------------------------------------------------------------------
	// Functor Mixin using Concepts
	// -----------------------------------------------------------------------------

	template<template<typename...> class _T, typename _U, typename... _Z>
	requires requires()
	{
		typename _T<_U, _Z...>::base;
	}
	auto seperate(_T<_U, _Z...>) -> std::pair<_T<Nonesuch, _Z...>, typename _T<_U, _Z...>::base>;

	template<typename _T>
	requires requires()
	{
		typename _T::base;
	}
	auto seperate(_T) -> std::pair<_T, typename _T::base>;

	template<typename _T>
	auto seperate(_T) -> std::pair<_T, Nonesuch>;

	/**
	 * @brief Combines Mixins from Functor using given Mixins
	 * Extensions (Class<typename, typename, typename>) must inherit the last template parameter. CurrSec is the whole
	 * type to be unpacked. If a CurrSec type contains a 'using base = base_type' it will follow it. If a CurrSec type
	 * doesn't contain a 'using base = base_type' it will stop following it.
	 * @tparam F Functor having the template parameters: Tag, Remaining, Base Functor
	 * @tparam Base Base of the Functors
	 * @tparam CurrSec Highest Mixin type to operate from
	 */
	template<template<typename, typename, typename> class F, typename Base, typename CurrSec>
	struct Filter
	{
		using s		 = decltype(seperate(std::declval<CurrSec>()));
		using Fil	 = typename Filter<F, Base, typename s::second_type>::type;
		using F_Full = F<typename s::first_type, CurrSec, Fil>;
		using type	 = typename std::conditional<complete_type<F_Full>, F_Full, Fil>::type;
	};

	template<template<typename, typename, typename> class F, typename Base>
	struct Filter<F, Base, Nonesuch>
	{
		using type = Base;
	};

	// -----------------------------------------------------------------------------
	// Functor base
	// -----------------------------------------------------------------------------

	/**
	 * @brief Default Functor for Mixins construction
	 * @tparam Object Type
	 */
	template<typename T>
	class Functor
	{
	public:
		constexpr Functor() = default;
		constexpr explicit Functor(T *o)
			: m_o(o)
		{
		}

		constexpr auto obj(T *o) noexcept { m_o = o; }
		constexpr auto obj() const noexcept
		{
			assert(m_o != nullptr && "Object isn't assigned.");
			return m_o;
		}

	private:
		T *m_o;
	};

} // namespace ctl