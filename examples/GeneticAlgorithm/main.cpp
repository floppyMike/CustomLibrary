#include <CustomLibrary/GeneticAlgorithm.h>
#include <CustomLibrary/RandomGenerator.h>
#include <iostream>

using namespace ctl;

static rnd::Random<rnd::Mersenne> g_rand;

class Agent
{
public:
	Agent() = default;

	Agent(size_t n)
		: m_str(n, 'a')
	{
		std::generate(m_str.begin(), m_str.end(), [] { return g_rand.rand_number('a', 'z'); });
	}

	[[nodiscard]] auto fitness() const noexcept { return m_fitness; }

	void score_fitness(std::string_view target)
	{
		assert(target.size() == m_str.size());

		m_fitness = 0.;
		for (auto [i, j] = std::pair{ m_str.begin(), target.begin() }; i != m_str.end(); ++i, ++j)
			if (*i == *j)
				++m_fitness;
	}

	constexpr auto str() const noexcept -> const auto & { return m_str; }

	auto begin() noexcept { return m_str.begin(); }
	auto begin() const noexcept { return m_str.begin(); }

	auto end() noexcept { return m_str.end(); }
	auto end() const noexcept { return m_str.end(); }

	void push(char c) { m_str.push_back(c); }

private:
	std::string m_str;
	double		m_fitness = 0.;
};

auto breed(const Agent &a, const Agent &b) -> Agent
{
	Agent o;

	constexpr size_t mutation = 1;
	for (auto ia = a.begin(), ib = b.begin(); ia != a.end(); ++ia, ++ib)
	{
		char c = g_rand.rand_number(0u, 1u) == 1 ? *ia : *ib;

		if (g_rand.rand_number(0u, 100u) <= mutation)
			c = g_rand.rand_number('a', 'z');

		o.push(c);
	}

	return o;
}

auto main() -> int
{
	constexpr size_t		   pop_size	  = 20;
	constexpr std::string_view target_str = "hellothere";

	std::vector<Agent> pop;
	pop.reserve(pop_size);
	for (size_t i = 0; i < pop.capacity(); ++i) pop.emplace_back(10).score_fitness(target_str);

	while (true)
	{
		std::vector<Agent> new_pop(pop_size);
		mcl::select(pop.begin(), pop.end(), new_pop.begin(), g_rand, breed);
		pop = std::move(new_pop);

		auto total = 0.;
		for (size_t i = 0; i < pop.size(); ++i)
		{
			std::cout << "i: " << i << '\t' << pop[i].str() << '\n';
			pop[i].score_fitness(target_str);
			total += pop[i].fitness();
		}

		std::cout << "Fitness " << total << "\nNext generation?\n\n";
		::getchar();
	}

	return 0;
}