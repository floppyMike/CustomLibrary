#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <cassert>

#include "Traits.h"

namespace ctl::par
{
	// -----------------------------------------------------------------------------
	// Additional getline options
	// -----------------------------------------------------------------------------

	// std::getline, but with multiple delimiters
	template<class CharT, class Traits, class Allocator>
	auto getline(std::basic_istream<CharT, Traits> &input, std::basic_string<CharT, Traits, Allocator> &str,
				 const std::initializer_list<CharT> delim) -> auto &
	{
		str.erase();
		for (CharT ch; input.get(ch);)
			if (std::any_of(delim.begin(), delim.end(), ch))
				return input;
			else if (str.size() == str.max_size())
			{
				input.setstate(std::ios_base::failbit);
				return input;
			}
			else
				str.push_back(ch);

		return input;
	}

	// ctl::getline, but string includes ending
	template<class CharT, class Traits, class Allocator>
	auto getline_End(std::basic_istream<CharT, Traits> &input, std::basic_string<CharT, Traits, Allocator> &str,
					 const std::initializer_list<CharT> &delim) -> auto &
	{
		str.erase();
		for (CharT ch; input.get(ch);)
		{
			str.push_back(ch);

			if (std::any_of(delim.begin(), delim.end(), ch))
				return input;

			if (str.size() == str.max_size())
			{
				input.setstate(std::ios_base::failbit);
				return input;
			}
		}

		return input;
	};

	// -----------------------------------------------------------------------------
	// Parsers
	// -----------------------------------------------------------------------------

	/**
	 * @brief Basic parser used to analize strings.
	 */
	class SequentialParser
	{
	public:
		SequentialParser() = default;

		/**
		 * @brief Change parsed string and reset
		 * @param dat New string to use
		 */
		void data(std::string_view dat)
		{
			reset();
			m_data = dat;
		}

		/**
		 * @brief Go back to the beginning
		 */
		void reset() noexcept { m_loc = 0U; }

		/**
		 * @brief Find a character inside the string
		 * @param delim Character to find
		 * @return size_t, npos on not found
		 */
		auto find(char delim) noexcept { return m_data.find(delim, m_loc); }

		/**
		 * @brief Get the string until the delimiter is found. Moves the start ptr to after the delimiter
		 * @param delim Character to get till
		 * @return A optional type of string_view
		 */
		auto get_until(char delim) noexcept -> std::optional<std::string_view>
		{
			if (const auto loc = find(delim); loc != std::string_view::npos)
				return get_until(loc - m_loc);

			return std::nullopt;
		}
		/**
		 * @brief Get the string until one of the delimiters is found. Moves the start ptr to after the delimiter
		 * @param begin Delimiter array begin
		 * @param end Delimiter array end
		 * @return A optional type of string_view
		 */
		template<typename _Iter>
		auto get_until(_Iter begin, const _Iter end) noexcept -> std::optional<std::string_view>
		{
			for (auto iter = m_data.begin() + m_loc; iter != m_data.end(); ++iter)
				for (; begin != end; ++begin)
					if (*iter == *begin)
						return get_until(static_cast<size_t>(std::distance(m_data.begin() + m_loc, iter)));

			return std::nullopt;
		}
		/**
		 * @brief Get the string until the count is reached. Moves the start ptr to after the count
		 * @param count Number of character to get
		 * @return A optional type of string_view
		 */
		auto get_until(size_t count) noexcept -> std::string_view
		{
			assert(m_loc + count <= m_data.size() && "count is too large for the string");

			auto &&sub_data = m_data.substr(m_loc, count);
			m_loc += count;

			return sub_data;
		}

		/**
		 * @brief Skip a specific part of the parsed string
		 *
		 * @param delim delimiter to skip to
		 * @return true delimiter not found
		 * @return false delimiter found
		 */
		auto skip(char delim) noexcept -> bool
		{
			if (const auto loc = find(delim); loc != std::string_view::npos)
			{
				m_loc = loc + 1;
				return false;
			}

			return true;
		}
		/**
		 * @brief Skip a specific part of the parsed string
		 *
		 * @param num amount ot skip
		 * @return true not amount possible
		 * @return false amount possible
		 */
		auto skip(size_t num) noexcept -> bool
		{
			if (m_loc + num > m_data.size())
				return true;

			m_loc += num;
			return false;
		}

		/**
		 * @brief Checks if string is the same as the specified string. Moves ptr
		 *
		 * @param str String to compare to
		 * @return true String is equal. Moves ptr to after the end
		 * @return false String isn't equal.
		 */
		auto is_same(std::string_view str) noexcept -> bool
		{
			if (m_data.size() - m_loc < str.size())
				return false;

			const auto res = m_data.size() - m_loc >= str.size()
				&& std::equal(&m_data[m_loc], &m_data[m_loc + str.size() - 1], str.data());

			if (res)
				skip(str.size());

			return res;
		}

		/**
		 * @brief Get the current string ptr position
		 * @return pos
		 */
		[[nodiscard]] auto current_loc() const noexcept { return m_loc; }

		/**
		 * @brief Get the next character without going ahead
		 * @return std::optional<char>
		 */
		auto peek() noexcept -> std::optional<char>
		{
			if (m_loc + 1 >= m_data.size())
				return std::nullopt;

			return m_data[m_loc + 1];
		}

		/**
		 * @brief Check if parser is at the end
		 *
		 * @return true Is at the end
		 * @return false Isn't at the end
		 */
		[[nodiscard]] auto at_end() const noexcept -> bool { return m_loc == m_data.size(); }

		/**
		 * @brief Move the ptr to a different location
		 * @param dis difference of location
		 */
		void mov(int dis) noexcept
		{
			m_loc += dis;
			assert((m_loc < 0 || m_loc > m_data.size()) && "String ptr out of bounds");
		}

	private:
		std::string_view m_data;
		size_t			 m_loc = 0U;
	};

	// -----------------------------------------------------------------------------
	// Input filters
	// -----------------------------------------------------------------------------

	/**
	 * @brief Get the input represented as a number
	 * @param prompt
	 * @return Number inputed
	 */
	template<arithmetic T>
	constexpr auto input_number(std::string_view str)
	{
		T input;

		while (true)
		{
			std::cout << str;
			std::cin >> input;

			if (std::cin.fail())
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "Invalid." << std::endl;
			}
			else
			{
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return input;
			}
		}
	}

	/**
	 * @brief Get the input of 1 of the list of given strings
	 *
	 * @param begin List begin
	 * @param end List end
	 * @param head Prompt
	 * @return Chosen string
	 */
	template<typename _Iter>
	auto input_string(_Iter begin, const _Iter end, const std::string_view head)
	{
		std::string str;

		while (true)
		{
			std::cout << head;
			std::getline(std::cin, str);

			if (str == "help")
				for (auto b = begin; b != end; ++b) std::cout << "\"" << *b << "\"\n";

			else if (std::find(begin, end, str) != end)
				return str;

			else
				std::cerr << "Invalid.\n";
		}

		return str;
	}

} // namespace ctl::par