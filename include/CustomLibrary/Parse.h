#pragma once

#include <array>
#include <iostream>
#include <vector>
#include <optional>
#include <cassert>
#include <span>

#include "Traits.h"

namespace ctl::par
{
	// -----------------------------------------------------------------------------
	// Parsers
	// -----------------------------------------------------------------------------

	/**
	 * @brief Basic parser used to analyze strings.
	 */
	class SequentialParser
	{
	public:
		static constexpr auto WHITESPACES = " \n\t";

		constexpr SequentialParser() = default;

		/**
		 * @brief Initialize the Parser with a string
		 * @param String to parse
		 */
		constexpr explicit SequentialParser(std::string_view dat) { data(dat); }

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
		 * @return location, null object on not found
		 */
		[[nodiscard]] constexpr auto find(char delim) const noexcept -> std::optional<size_t>
		{
			const auto res = m_data.find(delim, m_loc);
			return res == std::string_view::npos ? std::nullopt : std::optional(res);
		}

		/**
		 * @brief Get the string until the delimiter is found. Moves the start ptr to after the delimiter
		 * @param delim Character to get till
		 * @return string or null object when delim not found
		 */
		constexpr auto get_until(char delim) noexcept -> std::optional<std::string_view>
		{
			if (const auto loc = m_data.find(delim, current_loc()); loc != std::string_view::npos)
				return get_until((ptrdiff_t)(loc - m_loc));

			return std::nullopt;
		}

		/**
		 * @brief Get the string until the count is reached. Moves the start ptr to after the count
		 * @param count Number of character to get
		 * @return string
		 */
		constexpr auto get_until(ptrdiff_t count) noexcept -> std::string_view
		{
			const auto npos		= _displace_(count);
			auto &&	   sub_data = m_data.substr(m_loc, count);
			seek(npos);

			return sub_data;
		}

		/**
		 * @brief Skip until the delimiter is found. Doesn't move when delimiter not found.
		 * @param delim delimiter to skip to
		 */
		constexpr void skip_till(char delim) noexcept
		{
			if (const auto loc = m_data.find(delim, m_loc); loc != std::string_view::npos)
				seek(loc + 1);
		}
		/**
		 * @brief Skip a specific part of the parsed string
		 * @param num amount ot skip
		 */
		constexpr void skip_for(size_t num) noexcept { mov(num); }
		/**
		 * @brief Skip whitespace characters. Including '\n', '\t', ' '.
		 */
		constexpr void skip_space() noexcept
		{
			if (const auto i = m_data.find_first_not_of(WHITESPACES, current_loc()); i != std::string_view::npos)
				seek(i);
			else
				seek(total_size());
		}

		/**
		 * @brief Checks if string is the same as the specified string. Moves ptr
		 *
		 * @param str String to compare to
		 * @return true String is equal. Moves ptr to after the end
		 * @return false String isn't equal.
		 */
		constexpr auto is_same(std::string_view str) noexcept -> bool
		{
			if (remaining() < total_size())
				return false;

			const auto res = std::equal(&m_data[current_loc()], &m_data[_displace_(str.size() - 1)], str.data());

			if (res)
				skip_for(str.size());

			return res;
		}

		/**
		 * @brief Get the current string ptr position
		 * @return pos
		 */
		[[nodiscard]] constexpr auto current_loc() const noexcept -> size_t { return m_loc; }

		/**
		 * @brief Get the next character without going ahead
		 * @return character
		 */
		[[nodiscard]] constexpr auto peek() const noexcept -> char { return m_data[_displace_(1)]; }

		/**
		 * @brief Get the next available non whitespace character and move 1 up
		 * @return next character
		 */
		constexpr auto next() noexcept -> char
		{
			skip_space();
			return get();
		}

		/**
		 * @brief Get the current character
		 * @return current character
		 */
		[[nodiscard]] constexpr auto current() const noexcept -> char { return m_data[current_loc()]; }

		/**
		 * @brief Get the current character and move 1 up
		 * @return current character
		 */
		constexpr auto get() noexcept -> char
		{
			const auto c = current();
			m_loc		 = _displace_(1);
			return c;
		}

		/**
		 * @brief Check if parser is at the end
		 * @return true Is at the end
		 * @return false Isn't at the end
		 */
		[[nodiscard]] constexpr auto at_end() const noexcept -> bool { return current_loc() == total_size(); }

		/**
		 * @brief Move the ptr to a different location
		 * @param dis difference of location
		 */
		constexpr void mov(ptrdiff_t dis) noexcept { m_loc = _displace_(dis); }

		/**
		 * @brief Get the total string size
		 * @return The size
		 */
		[[nodiscard]] constexpr auto total_size() const noexcept -> size_t { return m_data.size(); }

		/**
		 * @brief Get the remaining string to be parsed
		 * @return The remainder
		 */
		[[nodiscard]] constexpr auto remaining() const noexcept -> ptrdiff_t
		{
			assert(current_loc() > total_size() && "String ptr out of bounds");
			return total_size() - current_loc();
		}

		/**
		 * @brief Moves the parse ptr to the specified position
		 * @param pos Position
		 */
		constexpr void seek(size_t pos) noexcept
		{
			assert(pos <= total_size());
			m_loc = pos;
		}

		/**
		 * @brief Extract a string until whitespace character. Whitespace at end skipped to next word
		 * @return string until whitespace or end
		 */
		[[nodiscard]] constexpr auto extract() noexcept -> std::string_view
		{
			const auto v = take();
			skip_space();
			return v;
		}

		/**
		 * @brief Extract a string until whitespace character.
		 * @return string until whitespace or end
		 */
		constexpr auto take() noexcept -> std::string_view
		{
			const auto		 res = m_data.find_first_of(WHITESPACES, current_loc());
			std::string_view ret;

			if (res != std::string_view::npos)
			{
				ret = m_data.substr(current_loc(), res - current_loc());
				seek(res);
			}
			else
			{
				ret = m_data.substr(current_loc());
				seek(m_data.size());
			}

			return ret;
		}

	private:
		[[nodiscard]] constexpr auto _displace_(ptrdiff_t diff) const noexcept -> size_t
		{
			assert((diff < 0 && -diff <= m_loc) || diff >= 0);
			const auto res = m_loc + diff;
			assert(res <= total_size());

			return res;
		}

		std::string_view m_data;
		size_t			 m_loc = 0U;
	};

	// -----------------------------------------------------------------------------
	// Input filters
	// -----------------------------------------------------------------------------

	/**
	 * @brief Get the input represented as a number
	 * @param prompt
	 * @return Number inputted
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