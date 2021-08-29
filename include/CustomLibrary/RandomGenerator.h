#ifndef RANDOMGEN
#define RANDOMGEN

#include <random>
#include <type_traits>
#include <cassert>

#include "Traits.h"

namespace ctl::rnd
{
	using Mersenne = std::mt19937;
	using Linear   = std::minstd_rand;
	using SubWCar  = std::ranlux24_base;

	template<typename T>
	concept random_generator = std::same_as<T, Mersenne> || std::same_as<T, Linear> || std::same_as<T, SubWCar>;

	/**
	 * @brief Manages a random generator device
	 * @tparam Random generator to use
	 */
	template<random_generator G>
	class Random
	{
	public:
		Random() noexcept
			: m_gen{ rd() }
		{
		}

		/**
		 * @brief Generate a random arithmetic number
		 *
		 * @param min Arithmetic minium value
		 * @param max Arithmetic maximum value
		 * @return Generated value
		 */
		template<arithmetic Type>
		constexpr auto rand_number(Type min, Type max) -> Type
		{
			assert(min < max && "Random: min is larger or equal to max.");

			if (min == max)
				return min;

			if constexpr (std::is_floating_point_v<Type>)
				return static_cast<Type>(std::uniform_real_distribution<>(min, max)(m_gen));
			else
				return static_cast<Type>(std::uniform_int_distribution<>(min, max)(m_gen));
		}

		/**
		 * @brief Chooses a random element from a range
		 *
		 * @param first First iterator of an array
		 * @param last Last iterator of an array
		 *
		 * @return
		 */
		template<typename Iter>
		constexpr auto rand_iter(Iter first, const Iter &last) -> Iter
		{
			std::advance(first, rand_number<size_t>(0, std::distance(first, last) - 1));
			return first;
		}

	private:
		std::random_device rd;
		G				   m_gen;
	};
} // namespace ctl::rnd
#endif // !RANDOMGEN
