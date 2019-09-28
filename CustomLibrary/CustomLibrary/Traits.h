#pragma once

#include <type_traits>

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


	namespace detail 
	{
		struct Nonesuch 
		{
			Nonesuch(const Nonesuch&) = delete;
			~Nonesuch() = delete;
			void operator=(const Nonesuch&) = delete;
		};

		template <typename Default, typename AlwaysVoid, template<typename...> typename Op, typename... Args>
		struct Detector 
		{
			using value_t = std::false_type;
			using type = Default;
		};

		template <typename Default, template<typename...> typename Op, typename... Args>
		struct Detector<Default, std::void_t<Op<Args...>>, Op, Args...> 
		{
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

	}

	template <template<typename...> typename Op, typename... Args>
	using is_detected = typename detail::Detector<detail::Nonesuch, void, Op, Args...>::value_t;

	template <template<typename...> typename Op, typename... Args>
	constexpr auto is_detected_v = is_detected<Op, Args...>::value;

	template <template<typename...> typename Op, typename... Args>
	using detected_t = typename detail::Detector<detail::Nonesuch, void, Op, Args...>::type;

	template <typename Default, template<typename...> typename Op, typename... Args>
	using detected_or = detail::Detector<Default, void, Op, Args...>;
}