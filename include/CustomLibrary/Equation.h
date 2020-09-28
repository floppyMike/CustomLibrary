#pragma once

#include <vector>
#include <variant>
#include <cmath>
#include <charconv>

#include "Error.h"
#include "utility.h"
#include "Parse.h"

namespace ctl::mth
{
	/*
	template<std::integral T>
	auto factorial(T x)
	{
		assert(x > 0 && "Number has a point or it's negative.");

		double ret = 1.;
		for (uint64_t i = 1; i <= x; ++i) ret *= i;
		return ret;
	}

	template<arithmetic T>
	auto root(T x, T lvl)
	{
		assert(x > 0 && "Number under root is negative");
		return std::pow(x, 1. / lvl);
	}

	class Math
	{
	public:
		Math()					= default;
		Math(const Math &)		= default;
		Math(Math &&)			= default;
		constexpr Math &operator=(const Math &) = default;
		constexpr Math &operator=(Math &&) = default;

		Math(const double &d)
			: m_dat(d)
		{
		}
		Math(double (*func)(double))
			: m_dat(func)
		{
		}
		Math(double (*func)(double, double))
			: m_dat(func)
		{
		}
		Math(const char &c)
			: m_dat(c)
		{
		}
		Math(const bool &b)
			: m_dis(b)
		{
		}

		Math &operator=(double &&d)
		{
			m_dat = d;
			return *this;
		}

		template<typename T>
		constexpr bool operator==(const T &x) const
		{
			if (auto val = std::get_if<T>(&m_dat))
				return *val == x;
			return false;
		}
		template<>
		constexpr bool operator==<>(const Math &x) const
		{
			return m_dat == x.m_dat && m_dis == x.m_dis;
		}

		template<typename T>
		constexpr bool is_Type() const
		{
			return std::get_if<T>(&m_dat) ? true : false;
		}

		template<typename T>
		constexpr const T &get() const
		{
			return *std::get_if<T>(&m_dat);
		}
		constexpr const auto &varant() const { return m_dat; }

	private:
		std::variant<double, char, double (*)(double), double (*)(double, double)> m_dat;
		bool																	   m_dis = false;
	};

	class Equation_Base
	{
	public:
		virtual bool   parse(std::vector<Math> &, const std::string &) = 0;
		virtual double evaluate(std::vector<Math>)					   = 0;
	};

	class Equation : public Equation_Base
	{
	public:
		template<typename T>
		bool _parseSample_(std::vector<Math> &list, const std::string &buf, const T &sample)
		{
			if constexpr (LOG)
				err::Log("Equation2.0: Parse: parseSample called.");

			const auto iter_sample = std::find_if(
				sample.begin(), sample.end(), [&buf](const T::value_type &x) constexpr { return x.first == buf; });
			if (iter_sample != sample.end())
			{
				list.emplace_back(iter_sample->second);
				return true;
			}
			return false;
		};

		bool parseModifer(std::vector<Math> &list, const std::string &buf)
		{
			if constexpr (ctl::LOG)
				ctl::err::Log("Equation2.0: Parse: parseModifer called.");

			constexpr std::array<std::pair<std::string_view, double (*)(double)>, 9> modif1{
				std::pair<std::string_view, double (*)(double)>{ "sin", dsin },
				{ "cos", dcos },
				{ "tan", dtan },
				{ "arcsin", dasin },
				{ "arccos", dacos },
				{ "arctan", datan },
				{ "!", factorial },
				{ "log", std::log },
				{ "sqrt", std::sqrt }
			};
			if (_parseSample_(list, buf, modif1))
				return true;
			return false;
		}

		bool parseJoiner(std::vector<Math> &list, const std::string &buf)
		{
			if constexpr (ctl::LOG)
				ctl::err::Log("Equation2.0: Parse: parseJoiner called.");

			constexpr std::array<std::pair<std::string_view, double (*)(double, double)>, 3> modif2{
				std::pair<std::string_view, double (*)(double, double)>{ "mod", modulus },
				{ "root", root },
				{ "pow", std::pow }
			};
			if (_parseSample_(list, buf, modif2))
				return true;
			return false;
		}

		bool parseConstant(std::vector<Math> &list, const std::string &buf)
		{
			if constexpr (ctl::LOG)
				ctl::err::Log("Equation2.0: Parse: parseConstant called.");

			constexpr std::array<std::pair<std::string_view, double>, 2> constant{
				std::pair<std::string_view, double>{ "e", 2.7182818 }, { "pi", 3.1415926 }
			};
			if (_parseSample_(list, buf, constant))
				return true;
			return false;
		}

		bool parse(std::vector<Math> &list, const std::string &buf) override
		{
			if constexpr (LOG)
				err::Log("Equation2.0: parse called.");

			if (parseModifer(list, buf))
				return true;

			if (parseJoiner(list, buf))
				return true;

			if (parseConstant(list, buf))
				return true;

			return false;
		}

		// Handles double(*)(double)
		void evaluateModifer(std::vector<Math> &subEqu)
		{
			if constexpr (LOG)
				err::Log("Equation2.0: Evaluate: modifer started.");

			for (auto iter = subEqu.begin(), end = subEqu.end(); iter != subEqu.end(); ++iter)
				if (iter->is_Type<double (*)(double)>())
				{
					*iter = iter->get<double (*)(double)>()((iter + 1)->get<double>());
					iter  = subEqu.erase(iter + 1, iter + 2) - 1;
					end	  = subEqu.end();
				}
		}

		// Handles double(*)(double, double)
		void evaluatejoiner(std::vector<Math> &subEqu)
		{
			if constexpr (LOG)
				err::Log("Equation2.0: Evaluate: joiner started.");

			for (auto iter = subEqu.begin(), end = subEqu.end(); iter != end; ++iter)
			{
				const auto isFunc = iter->is_Type<double (*)(double, double)>();
				const auto isMult = *iter == '*';
				const auto isDivi = *iter == '/';

				if (isFunc || isMult || isDivi)
				{
					if (isFunc)
						*(iter - 1) = iter->get<double (*)(double, double)>()((iter - 1)->get<double>(),
																			  (iter + 1)->get<double>());
					else if (isMult)
						*(iter - 1) = (iter - 1)->get<double>() * (iter + 1)->get<double>();
					else
					{
						const auto numerus = (iter + 1)->get<double>();
						if (numerus == 0.)
							throw err::Log("Equation2.0: Evaluate: numerus is 0.", Log::Type::ERROR);
						*(iter - 1) = (iter - 1)->get<double>() / (iter + 1)->get<double>();
					}

					iter = subEqu.erase(iter, iter + 2) - 1;
					end	 = subEqu.end();
				}
			}
		}

		double evaluate(std::vector<Math> subEqu) override
		{
			if constexpr (LOG)
				err::Log("Equation2.0: evaluation started.");

			// Erase disabled spots
			subEqu.erase(std::remove(subEqu.begin(), subEqu.end(), Math(true)), subEqu.end());

			// Step 1: Modifer
			evaluateModifer(subEqu);

			// Step 2: Joiner
			evaluatejoiner(subEqu);

			// Step 3: Sum up
			auto sum = 0.;
			for (auto &i : subEqu) sum += i.get<double>();

			return sum;
		}
	};
	*/

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
			const auto n = p.extract();

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
			if (const auto c = p.get(); c == '/')
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

			if (const auto c = p.get(); c == '-')
				num = -1.;
			else if (c != '+')
				p.mov(-1);

			if (auto n = O(p); n)
			{
				num *= *n;
				for (auto v = X(p); v; v = X(p))
					num *= *v;

				return num;
			}

			return std::nullopt;
		}

		constexpr auto T(par::SequentialParser &p) -> std::optional<double>
		{
			if (auto n = R(p); n)
			{
				for (auto v = R(p); v; v = R(p))
					*n += *v;

				return *n;
			}

			return std::nullopt;
		}

		constexpr auto B(par::SequentialParser &p) -> std::optional<double>
		{
			if (p.current() != '(')
				return std::nullopt;
			p.mov(1);

			if (auto v = T(p); v)
			{
				if (p.get() != ')')
					throw std::runtime_error("Missing bracket.");
				return *v;
			}

			throw std::runtime_error("Expression inside bracket doesn't make sense.");
		}

	} // namespace detail

	constexpr auto solve(std::string_view equ_str) -> double
	{
		// Parser Rules
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