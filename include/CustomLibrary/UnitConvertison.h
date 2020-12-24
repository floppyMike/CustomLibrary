#ifndef UNITCONVERSION
#define UNITCONVERSION

#include "Traits.h"

namespace ctl::mth
{
	enum PrefixIndex
	{
		YOCTO,
		ZEPTO,
		ATTO,
		FEMTO,
		PICO,
		NANO,
		MICRO,
		MILLI,
		CENTI,
		DECI,

		BASE,

		DECA,
		HECTO,
		KILO,
		MEGA,
		GIGA,
		TERA,
		PETA,
		EXA,
		ZETTA,
		YOTTA
	};

	static constexpr double prefixes[] =
	{
		1e-24, 1e-21, 1e-18, 1e-15, 1e-12, 1e-9, 1e-6, 
		1e-3,  1e-2,  1e-1,  1.,	1e1,   1e2,  1e3,   
		1e6,   1e9,   1e12,  1e15,  1e18,  1e21, 1e24,
	};

	/**
	 * @brief Convert a arithmetic number to a different prefix
	 * 
	 * @param num Number to convert
	 * @param from Original prefix
	 * @param to New prefix
	 * @return Number with new prefix 
	 */
	template<arithmetic Type>
	auto convert(Type num, PrefixIndex from, PrefixIndex to) -> Type
	{
		return num * prefixes[from] / prefixes[to];
	}
}
#endif // !UNITCONVERSION