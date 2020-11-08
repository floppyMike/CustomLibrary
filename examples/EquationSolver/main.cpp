#include <iostream>
#include <chrono>

#include <CustomLibrary/Parse.h>
#include <CustomLibrary/Equation.h>

using namespace ctl;

auto main() -> int
{
	std::cout << mth::solve("2+2") << '\n'
			  << mth::solve("2*2") << '\n'
			  << mth::solve("2/2") << '\n'
			  << mth::solve("2 + 2") << '\n'
			  << mth::solve("2 * 2") << '\n'
			  << mth::solve("sqrt( 4 ) ") << '\n'
			  << mth::solve("2 + 90") << '\n'
			  << mth::solve("2 * 4.5") << '\n'
			  << mth::solve("sqrt((6.67 * 10^(-11) * 6 * 10^24) / (2 * 10^7)) *"
							" (sqrt((2 * 4 * 10^7) / (4 * 10^7 + 2 * 10^7)) - 1)")
			  << '\n'
			  << mth::solve("2^3") << '\n';

	return 0;
}