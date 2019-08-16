#pragma once

#include <string_view>
#include <variant>
#include <any>
#include <unordered_map>

namespace ctl
{
	template<typename ...T>
	class ResManagerStatic
	{
	public:
		ResManagerStatic() = default;

		template<typename Type>
		auto& get(const std::string_view& var);

		template<typename Type>
		auto& construct(const std::string_view& var, Type& dat)
		{
			return construct<Type>(var, std::move(dat));
		}
		template<typename Type>
		auto& construct(const std::string_view &var, Type &&dat);

		auto& erase(const std::string_view& var);

	private:
		std::unordered_map<std::string_view, std::variant<T...>> m_res;
	};

	class ResManager
	{
	public:
		ResManager() = default;

		/**
		* @summary get a value
		* @tparam "T" variable Type
		* @param "var" variable name
		* @remarks Type must match
		* @returns variable of Type
		*/
		template<typename Type>
		auto& get(const std::string_view& var) noexcept;

		/**
		* @summary construct a variable in place
		* @tparam "T" variable Type
		* @param "var" variable name
		* @param "args" arguments of the variable
		* @returns variable of Type
		*/
		template<typename T, typename... Arg>
		auto& emplace(const std::string_view& var, Arg&&... args);

		/**
		* @summary construct a variable in place
		* @tparam "T" variable Type
		* @param "var" variable name
		* @param "dat" variable to move in
		* @returns variable of Type
		*/
		template<typename T>
		auto& construct(const std::string_view& var, T& dat)
		{
			return construct<T>(var, std::move(dat));
		}
		template<typename T>
		auto& construct(const std::string_view& var, T&& dat);

		/**
		* @summary erase a variable
		* @param "var" variable name
		*/
		ResManager& erase(const std::string_view& var);

	private:
		std::unordered_map<std::string_view, std::any> m_res;
	};

	template<typename... T>
	template<typename Type>
	inline auto& ResManagerStatic<T...>::get(const std::string_view& var)
	{
		auto* const val = std::get_if<Type>(&m_res[var]);
		assert(val != nullptr && "SDL: ResourceManager: get: Wrong type stored.");
		return *val;
	}

	template<typename ...T>
	template<typename Type>
	inline auto& ResManagerStatic<T...>::construct(const std::string_view& var, Type&& dat)
	{
		static_assert(std::disjunction_v<std::is_same<std::decay_t<Type>, T>...>, "Type must be of ResManager");

		if (!m_res.emplace(var, std::move(dat)).second)
			Log::note("SDL: ResourceManager: emplace: Variable is replaced.", Log::Sev::WARNING);

		return *this;
	}

	template<typename... T>
	inline auto& ResManagerStatic<T...>::erase(const std::string_view& var)
	{
		m_res.erase(var);
		return *this;
	}

	template<typename T>
	inline auto& ResManager::get(const std::string_view& var) noexcept
	{
		auto* const val = std::any_cast<T>(&m_res[var]);
		assert(val != nullptr && "SDL: ResourceManager: get: Wrong type requested.");
		return *val;
	}

	template<typename T, typename ...Arg>
	inline auto& ResManager::emplace(const std::string_view& var, Arg&& ...args)
	{
		const auto val = m_res.try_emplace(var, std::in_place_type<T>, args...);
		if (!val.second)
			Log::logWrite("SDL: ResourceManager: emplace: Variable is ignored.", Log::Sev::WARNING);

		return *std::any_cast<T>(&val.first->second);
	}

	template<typename T>
	inline auto& ResManager::construct(const std::string_view& var, T&& dat)
	{
		const auto val = m_res.try_emplace(var, std::move(dat));
		if (!val.second)
			Log::logWrite("SDL: ResourceManager: emplace: Variable is replaced.", Log::Sev::WARNING);

		return *std::any_cast<T>(&val.first->second);
	}
}