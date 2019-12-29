#pragma once

#include <type_traits>


#define HAS_METHOD_TYPE(method) \
	template<typename T, typename... Arg> \
	using has_##method = decltype(std::declval<T&>().method(std::declval<Arg>()...)); 

#define FORWARDING_MEMBER_FUNCTION(Inner, inner, function, qualifiers) \
	template< \
		typename... Args, \
		typename return_type = decltype(std::declval<Inner qualifiers>().function(std::declval<Args &&>()...)) \
	> \
	constexpr decltype(auto) function(Args && ... args) qualifiers noexcept( \
		noexcept(std::declval<Inner qualifiers>().function(std::forward<Args>(args)...)) and \
		( \
			std::is_reference<return_type>::value or \
			std::is_nothrow_move_constructible<return_type>::value \
		) \
	) { \
		return /*static_cast<Inner qualifiers>*/(inner).function(std::forward<Args>(args)...); \
	}

#define FORWARDING_MEMBER_FUNCTIONS_C(Inner, inner, function, reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const reference)

#define FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, volatile reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const volatile reference)

#define FORWARDING_MEMBER_FUNCTIONS(Inner, inner, function) \
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
	template<typename T, template<typename> class crtpType>
	struct crtp
	{
		T& _() noexcept { return static_cast<T&>(*this); }
		const T& _() const noexcept { return static_cast<const T&>(*this); }

	private:
		crtp() = default;
		friend crtpType<T>;
	};


	//template<template<template<typename> class...> class Base, template<typename> class... Ex>
	//struct Extendable : public Ex<Base<Ex...>>...
	//{
	//public:
	//	Extendable() = default;

	//	Extendable(const Extendable&) = default;
	//	Extendable(Extendable&&) = default;

	//	Extendable& operator=(const Extendable&) = default;
	//	Extendable& operator=(Extendable&&) = default;

	//	template<template<typename> class... T>
	//	Extendable(T<Base<Ex...>>&&... arg)
	//		: T<Base<Ex...>>(std::move(arg))...
	//	{
	//	}

	//	template<template<typename> class... T>
	//	Extendable(const Base<T...>& cast)
	//		: T<Base<Ex...>>(static_cast<T<Base<Ex...>>>(*reinterpret_cast<T<Base<Ex...>>*>(static_cast<T<Base<T...>>*>(&cast))))...
	//	{
	//	}

	//	template<template<typename> class... T>
	//	Extendable(Base<T...>&& cast)
	//		: T<Base<Ex...>>(static_cast<T<Base<Ex...>>&&>(*reinterpret_cast<T<Base<Ex...>>*>(static_cast<T<Base<T...>>*>(&cast))))...
	//	{
	//	}
	//};


	namespace detail 
	{
		struct Nonesuch 
		{
			Nonesuch(const Nonesuch&) = delete;
			~Nonesuch() = delete;
			void operator=(const Nonesuch&) = delete;
		};

		template <typename Default, typename AlwaysVoid, template<typename...> class Op, typename... Args>
		struct Detector 
		{
			using value_t = std::false_type;
			using type = Default;
		};

		template <typename Default, template<typename...> class Op, typename... Args>
		struct Detector<Default, std::void_t<Op<Args...>>, Op, Args...> 
		{
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

	}

	template <template<typename...> class Op, typename... Args>
	using is_detected = typename detail::Detector<detail::Nonesuch, void, Op, Args...>::value_t;

	template <template<typename...> class Op, typename... Args>
	constexpr auto is_detected_v = is_detected<Op, Args...>::value;

	template <template<typename...> class Op, typename... Args>
	using detected_t = typename detail::Detector<detail::Nonesuch, void, Op, Args...>::type;

	template <typename Default, template<typename...> class Op, typename... Args>
	using detected_or = detail::Detector<Default, void, Op, Args...>;


	template<typename Type, typename Derived>
	class ReliesOn
	{
	protected:
		constexpr ReliesOn() noexcept = default;
		constexpr ReliesOn(const ReliesOn& v) noexcept = default;
		constexpr ReliesOn& operator=(const ReliesOn& v) noexcept = default;

		constexpr Derived& set(Type* v) noexcept
		{
			m_var = v;
			return static_cast<Derived&>(*this);
		}

		template<typename T, typename = typename std::enable_if_t<std::is_same_v<Type, T>>>
		constexpr Type* get() const noexcept
		{
			return m_var;
		}

	private:
		Type* m_var = nullptr;
	};
}