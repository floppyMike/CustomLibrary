#pragma once

#include <CustomLibrary/RandomGenerator.h>
#include <CustomLibrary/Traits.h>

namespace ctl::mcl
{
	/**
	 * @brief Ensures Type has a fitness accessor
	 */
	template<typename T>
	concept selectable = std::movable<T> &&requires(T a)
	{
		a.fitness();
	};

	/**
	 * @brief A Genetic Algorithm implementation that selects sample parents from the input iterators using a rolette
	 * style selection process. Moves the new generation to the destination iterator.
	 *
	 * @tparam Mix Function which has the type the iterator is pointing to.
	 * @param begin Population begin
	 * @param end Population end
	 * @param dest_begin New generation begin (Must have enough space such as the difference between begin and end)
	 * @param rand Randomization generator
	 * @param breed The breed function taking in 2 parameters
	 */
	template<typename Iter1, typename Iter2, typename Gen, typename Mix>
	void select(Iter1 begin, Iter1 end, Iter2 dest_begin, rnd::Random<Gen> &rand,
				Mix breed) requires selectable<typename std::iterator_traits<Iter1>::value_type>
		&&std::invocable<Mix, typename std::iterator_traits<Iter1>::value_type,
						 typename std::iterator_traits<Iter1>::value_type>
	{
		using Type = typename std::iterator_traits<Iter1>::value_type;

		// Initilize score
		auto total_score = 0.;
		for (auto iter = begin; iter != end; ++iter)
			total_score += iter->fitness() + 1.; // avoid the same parent being selected all the time

		for (size_t i = 0, n = std::distance(begin, end); i < n; ++i)
		{
			Iter1 parents[2];

			for (auto &parent : parents)
			{
				// Rolette style picking
				auto i_pop = begin;
				for (double peak = rand.rand_number(0., total_score); peak >= 0.; ++i_pop)
					peak -= i_pop->fitness() + 1.;

				--i_pop; // Adjust to the element that entered minus

				parent = i_pop;
			}

			*(dest_begin++) = std::move(breed(*parents[0], *parents[1]));
		}
	}

} // namespace ctl::mcl