#pragma once

#include <cmath>
#include <charconv>

#include "Error.h"
#include "utility.h"
#include "Parse.h"

namespace ctl::mth
{
	static constexpr std::array constants = { std::pair<std::string_view, double>{ "e", 2.718281828459045 },
											  std::pair<std::string_view, double>{ "pi", 3.141592653589793 } };

	static constexpr std::array functions = { std::pair<std::string_view, double (*)(double)>{ "sin", std::sin },
											  std::pair<std::string_view, double (*)(double)>{ "cos", std::cos },
											  std::pair<std::string_view, double (*)(double)>{ "tan", std::tan } };

	namespace detail
	{
		constexpr auto B(par::SequentialParser &p) -> std::optional<double>;

		constexpr auto C(par::SequentialParser &p) -> std::optional<double>
		{
			const auto s = p.current_loc();
			const auto c = p.extract();

			for (const auto &con : constants)
				if (c.starts_with(con.first))
				{
					p.mov(con.first.size() - c.size());
					return con.second;
				}

			p.seek(s);
			return std::nullopt;
		}

		constexpr auto N(par::SequentialParser &p) -> std::optional<double>
		{
			const auto s = p.current_loc();
			const auto n = p.take();

			if (ctl::is_number(n.front()))
			{
				char *	   end;
				const auto num = strtod(n.data(), &end);
				p.mov(end - n.data() - n.size());
				return num;
			}

			p.seek(s);
			return std::nullopt;
		}

		constexpr auto U(par::SequentialParser &p) -> std::optional<double (*)(double)>
		{
			const auto s = p.current_loc();
			const auto f = p.extract();

			for (const auto &con : functions)
				if (f.starts_with(con.first))
				{
					p.mov(con.first.size() - f.size());
					return con.second;
				}

			p.seek(s);
			return std::nullopt;
		}

		constexpr auto F(par::SequentialParser &p) -> std::optional<double>
		{
			if (const auto f = U(p); f)
			{
				if (const auto n = B(p); n)
					return f.value()(*n);

				throw std::runtime_error("Couldn't evaluate function parameter.");
			}

			return std::nullopt;
		}

		constexpr auto O(par::SequentialParser &p) -> std::optional<double>
		{
			p.skip_space();

			if (p.at_end())
				return std::nullopt;

			if (auto v = N(p); v)
				return *v;

			if (auto v = B(p); v)
				return *v;

			if (auto v = C(p); v)
				return *v;

			if (auto v = F(p); v)
				return *v;

			return std::nullopt;
		}

		constexpr auto X(par::SequentialParser &p) -> std::optional<double>
		{
			if (p.at_end())
				return std::nullopt;

			bool div = false;
			if (const auto c = p.next(); c == '/')
				div = true;
			else if (c != '*')
				p.mov(-1);

			if (auto n = O(p); n)
				return div ? 1. / *n : *n;

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

		constexpr auto B(par::SequentialParser &p) -> std::optional<double>
		{
			if (p.next() != '(')
			{
				p.mov(-1);
				return std::nullopt;
			}

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
		// Object   O = N | F | C | B
		// Function F = U (O | B)
		// Name		U
		// Number 	N
		// Constant	C

		par::SequentialParser p(equ_str);
		if (const auto v = detail::T(p); v)
			return *v;

		return INFINITY;
	}
} // namespace ctl::mth