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

	namespace detail
	{
		template<typename F>
		constexpr auto symbol(par::SequentialParser &p, F &&f)
		{
			const auto s = p.current_loc();

			if (const auto v = f(p); v)
				return v;

			p.seek(s);
			return std::nullopt;
		}

		template<typename F>
		constexpr auto symbol(par::SequentialParser &p, F &&f) requires requires(F f, par::SequentialParser p)
		{
			{
				f(p)
			}
			->std::same_as<bool>;
		}
		{
			const auto s = p.current_loc();

			if (const auto v = f(p); v)
				return true;

			p.seek(s);
			return false;
		}

		template<size_t n, typename T>
		constexpr auto symbol_string(par::SequentialParser &p, const std::array<std::pair<std::string_view, T>, n> &arr)
			-> std::optional<T>
		{
			const auto c = p.extract();

			for (const auto &con : arr)
				if (c.starts_with(con.first))
				{
					p.mov(con.first.size() - c.size());
					return con.second;
				}

			return std::nullopt;
		}

		constexpr auto bracket(par::SequentialParser &p) -> std::optional<double>;
		constexpr auto symbol(par::SequentialParser &p) -> std::optional<double>;

		constexpr auto constant(par::SequentialParser &p) -> std::optional<double>
		{
			return symbol_string(p, constants);
		}

		constexpr auto number(par::SequentialParser &p) -> std::optional<double>
		{
			const auto n = p.extract();

			if (ctl::is_number(n.front()))
			{
				char *	   end;
				const auto num = strtod(n.data(), &end);
				p.mov(end - n.data() - n.size());
				return num;
			}

			return std::nullopt;
		}

		constexpr auto function(par::SequentialParser &p) -> std::optional<double (*)(double)>
		{
			return symbol_string(p, functions);
		}

		constexpr auto function_pair(par::SequentialParser &p) -> std::optional<double>
		{
			if (const auto f = function(p); f)
			{
				if (const auto n = symbol(p); n)
					return f.value()(*n);

				throw std::runtime_error("Couldn't evaluate function parameter.");
			}

			return std::nullopt;
		}

		static constexpr std::array unary = { number, bracket, constant, function_pair };

		constexpr auto symbol(par::SequentialParser &p) -> std::optional<double>
		{
			p.skip_space();

			if (p.at_end())
				return std::nullopt;

			for (auto f : unary)
				if (const auto v = symbol(p, f); v)
				{
					if (!p.at_end())
						if (symbol(p, [](par::SequentialParser &p) { return p.next() == '^'; }))
							if (const auto z = symbol(p); z)
								return std::pow(*v, *z);
							else
								throw std::runtime_error("Exponent is faulty.");

					return *v;
				}

			return std::nullopt;
		}

		constexpr auto X(par::SequentialParser &p) -> std::optional<double>
		{
			p.skip_space();

			if (p.at_end())
				return std::nullopt;

			if (const auto [v, n] = std::pair(symbol(p,
													 [](par::SequentialParser &p) -> std::optional<char> {
														 const auto c = p.get();
														 return c == '*' || c == '/' ? std::optional<char>(c)
																					 : std::nullopt;
													 }),
											  symbol(p));
				n)
				return !v || *v == '*' ? *n : 1. / *n;

			return std::nullopt;
		}

		constexpr auto R(par::SequentialParser &p) -> std::optional<double>
		{
			if (p.at_end())
				return std::nullopt;

			double num = 1.;

			if (const auto c = p.next(); c == '-')
				num = -1.;
			else if (c != '+')
				p.mov(-1);

			if (auto n = O(p); n)
			{
				num *= *n;
				for (auto v = X(p); v; v = X(p)) num *= *v;

				return num;
			}

			return std::nullopt;
		}

		constexpr auto T(par::SequentialParser &p) -> std::optional<double>
		{
			if (auto n = R(p); n)
			{
				for (auto v = R(p); v; v = R(p)) *n += *v;
				return *n;
			}

			return std::nullopt;
		}

		constexpr auto bracket(par::SequentialParser &p) -> std::optional<double>
		{
			if (!symbol(p, [](par::SequentialParser &p) { return p.next() == '('; }))
				return std::nullopt;

			if (auto v = T(p); v)
			{
				if (p.next() != ')')
					throw std::runtime_error("Missing bracket.");

				return *v;
			}

			throw std::runtime_error("Expression inside bracket doesn't make sense.");
		}

	} // namespace detail

	constexpr auto solve(std::string_view equ_str) -> double
	{
		// Parser Rules (Whitespaces are skipped on character demand)
		// Equation T = R | L R
		// Bracket	B = '(' T ')'
		// Term     R = ('+' | '-') O | L X
		// Term Obj X = '*' O | '/' O | O
		// Object   O = (N | F | C | B) | ((N | F | C | B) '^' (O | B))
		// Function F = U (O | B)
		// Name		U
		// Number 	N
		// Constant	C

		par::SequentialParser p(equ_str);
		if (const auto v = detail::T(p); v)
			return *v;

		throw std::runtime_error("Invalid Synthax at " + std::string(p.dump()));
	}
} // namespace ctl::mth