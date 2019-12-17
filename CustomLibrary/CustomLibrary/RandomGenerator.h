#ifndef RANDOMGEN
#define RANDOMGEN

#include <random>
#include <type_traits>

#include "Error.h"

namespace ctl::rnd
{
	using Mersenne = std::mt19937;
	using Linear = std::minstd_rand;
	using SubWCar = std::ranlux24_base;

	//Gen::
	template<typename G, typename = typename std::enable_if_t<std::is_same_v<Mersenne, G> || std::is_same_v<Linear, G> || std::is_same_v<SubWCar, G>>>
	class RandomGen
	{
	public:
		RandomGen() : m_gen{ rd() } {}

		template<typename Type, typename = typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type>
		constexpr Type rand_number(const Type &min, const Type &max)
		{
			assert(min < max && "RandomGenerator: min is larger or equal to max.");

			if constexpr (std::is_floating_point_v<Type>)
				return static_cast<Type>(std::uniform_real_distribution<>(min, max)(m_gen));
			else
				return static_cast<Type>(std::uniform_int_distribution<>(min, max)(m_gen));
		}

		template<typename Iter/*, typename = typename std::enable_if_t<!std::is_same_v<typename std::iterator_traits<Iter>::type_value, void>>*/>
		constexpr Iter rand_iter(Iter first, const Iter &last)
		{
			std::advance(first, randNumber<size_t>(0, std::distance(first, last) - 1));
			return first;
		}
	private:
		std::random_device rd;
		G m_gen;
	};
}
#endif // !RANDOMGEN