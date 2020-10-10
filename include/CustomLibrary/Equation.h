#pragma once

#include <cmath>
#include <charconv>

#include "Error.h"
#include "utility.h"
#include "Parse.h"

namespace ctl::mth
{
	using Constant = std::pair<std::string_view, double>;
	using Function = std::pair<std::string_view, double (*)(double)>;

	static constexpr std::array constants = { Constant{ "e", 2.718281828459045 }, Constant{ "pi", 3.141592653589793 } };

	static constexpr std::array functions = { Function{ "sin", std::sin }, Function{ "cos", std::cos },
											  Function{ "tan", std::tan }, Function{ "sqrt", std::sqrt } };

	class EquationParser : par::SequentialParser
	{
		constexpr auto _skip_() noexcept -> bool
		{
			this->skip_space();
			return this->at_end();
		}

	public:
		using par::SequentialParser::SequentialParser;

		constexpr auto extract() noexcept -> std::optional<std::string_view>
		{
			const auto s = _skip_();
			m_back		 = this->current_loc();

			return s ? std::nullopt : std::optional(par::SequentialParser::take());
		}

		constexpr auto next() noexcept -> std::optional<char>
		{
			const auto s = _skip_();
			m_back		 = this->current_loc();

			return s ? std::nullopt : std::optional(par::SequentialParser::get());
		}

		constexpr void abort() noexcept { this->seek(m_back); }

		using par::SequentialParser::at_end;
		using par::SequentialParser::current;
		using par::SequentialParser::dump;
		using par::SequentialParser::mov;
		using par::SequentialParser::skip_space;

	private:
		size_t m_back = 0;
	};

	namespace detail
	{
		/**
		 * @brief Returns if check or skip is matched or nothing
		 *
		 * @param p parser
		 * @param check character to check
		 * @param skip character to skip
		 * @return the operator performed: 0 ^= Error or no match; 1 ^= skip character found; 2 ^= check character found
		 */
		enum class Ex
		{
			CHECK,
			SKIP,
			NOTHING
		};
		constexpr auto extract_skip(EquationParser &p, char check, char skip) -> Ex
		{
			const auto c = p.next();

			if (!c)
				return Ex::NOTHING;

			if (*c == check)
				return Ex::CHECK;
			if (*c != skip)
			{
				p.abort();
				return Ex::NOTHING;
			}

			return Ex::SKIP;
		}

		constexpr auto brackets(EquationParser &p) -> std::optional<double>;
		constexpr auto object(EquationParser &p) -> std::optional<double>;

		template<size_t n, typename T>
		constexpr auto begins_with(EquationParser &p, const std::array<std::pair<std::string_view, T>, n> &l)
			-> std::optional<T>
		{
			const auto c = p.extract();

			if (!c)
				return std::nullopt;

			const auto res =
				std::find_if(std::begin(l), std::end(l), [c](const auto &e) { return c->starts_with(e.first); });

			if (res == std::end(l))
			{
				p.abort();
				return std::nullopt;
			}

			p.mov(res->first.size() - c->size());
			return res->second;
		}

		constexpr auto constant(EquationParser &p) -> std::optional<double> { return begins_with(p, constants); }

		constexpr auto number(EquationParser &p) -> std::optional<double>
		{
			const auto n = p.extract();

			if (!n)
				return std::nullopt;

			if (!ctl::is_number(n->front()))
			{
				p.abort();
				return std::nullopt;
			}

			char *	   end;
			const auto num = strtod(n->data(), &end);
			p.mov(end - n->data() - n->size());

			return num;
		}

		constexpr auto function(EquationParser &p) -> std::optional<double>
		{
			const auto f = begins_with(p, functions);

			if (!f)
				return std::nullopt;

			const auto n = object(p);

			if (!n)
				throw std::runtime_error("Couldn't evaluate function parameter.");

			return f.value()(*n);
		}

		constexpr auto object(EquationParser &p) -> std::optional<double>
		{
			constexpr std::array unary = { number, brackets, constant, function };

			for (auto f : unary)
				if (const auto v = f(p); v)
				{
					if (const auto c = extract_skip(p, '^', '^'); c == Ex::CHECK)
						if (const auto z = object(p); z)
							return std::pow(*v, *z);
						else
							throw std::runtime_error("Power equation synthax incorrect.");

					return *v;
				}

			return std::nullopt;
		}

		constexpr auto bind(EquationParser &p) -> std::optional<double>
		{
			const auto div = extract_skip(p, '/', '*');

			const auto n = object(p);

			if (!n)
				if (div == Ex::NOTHING)
					return std::nullopt;
				else
					throw std::runtime_error("Missing number after * or /.");

			return div == Ex::CHECK ? 1. / *n : *n;
		}

		constexpr auto term(EquationParser &p) -> std::optional<double>
		{
			const auto sign = extract_skip(p, '-', '+');

			auto n = object(p);

			if (!n)
				if (sign == Ex::NOTHING)
					return std::nullopt;
				else
					throw std::runtime_error("Missing number after + or -.");

			while (const auto v = bind(p)) *n *= *v;

			return sign == Ex::CHECK ? -*n : *n;
		}

		constexpr auto equation(EquationParser &p) -> std::optional<double>
		{
			auto n = term(p);

			if (!n)
				return std::nullopt;

			while (const auto v = term(p)) *n += *v;
			return *n;
		}

		constexpr auto brackets(EquationParser &p) -> std::optional<double>
		{
			if (p.next() != '(')
			{
				p.abort();
				return std::nullopt;
			}

			const auto v = equation(p);

			if (!v)
				throw std::runtime_error("Expression inside bracket doesn't make sense.");

			if (p.next() != ')')
				throw std::runtime_error("Missing bracket.");

			return *v;
		}

	} // namespace detail

	auto solve(std::string_view equ_str) -> double // Todo: Wait until from_chars becomes available for floating point
												   // 		numbers ~> constexpr becomes viable
	{
		// Parser Rules (Whitespaces are skipped on character demand)
		// Equation T = R | L R
		// Bracket	B = '(' T ')'
		// Term     R = ('+' O | '-' O | O) | L X
		// Term Obj X = '*' O | '/' O | O
		// Object   O = (N | F | C | B) | ((N | F | C | B) '^' (O | B))
		// Function F = U (O | B)
		// Name		U
		// Number 	N
		// Constant	C

		EquationParser p(equ_str);
		if (const auto v = detail::equation(p); v)
			return *v;

		throw std::runtime_error("Invalid Synthax at " + std::string(p.dump()));
	}
} // namespace ctl::mth