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
			  << mth::solve("2 * 4.5") << '\n';

	return 0;
}