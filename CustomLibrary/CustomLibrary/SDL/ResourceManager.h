#pragma once

#include <string_view>
#include <variant>
#include <unordered_map>

namespace ctl
{
	template<typename ...T>
	class ResourceManager
	{
		template<typename Type>
		using included_in_T = std::enable_if_t<std::disjunction_v<std::is_same<std::decay_t<Type>, T>...>>;

	public:
		ResourceManager() = default;

		template<typename Type>
		auto& get(const std::string_view& var)
		{
			auto *const val = std::get_if<Type>(&m_res[var]);
			assert(val != nullptr && "SDL: ResourceManager: get: Wrong type stored.");
			return *val;
		}

		template<typename Type, typename = typename included_in_T<Type>>
		auto& construct(const std::string_view& var, Type& dat)
		{
			return construct<Type>(var, std::move(dat));
		}
		template<typename Type, typename = typename included_in_T<Type>>
		auto& construct(const std::string_view &var, Type &&dat)
		{
			if (!m_res.emplace(var, std::move(dat)).second)
				Log::note("SDL: ResourceManager: emplace: Variable is replaced.", Log::Sev::WARNING);

			return *this;
		}

		inline auto& erase(const std::string_view& var) 
		{ 
			m_res.erase(var); 
			return *this; 
		}

	private:
		std::unordered_map<std::string_view, std::variant<T...>> m_res;
	};
}