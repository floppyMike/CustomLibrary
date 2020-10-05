#include <iostream>
#include <chrono>

#include <CustomLibrary/Parse.h>
#include <CustomLibrary/Equation.h>

using namespace ctl;

class MetaCompiler
{
};

//

auto main() -> int
{
	std::string_view	  t = "Hello There Me";
	par::SequentialParser p(t);

	std::cout << p.extract() << '\n' << p.extract() << '\n' << p.extract() << '\n';

	std::string_view equ = "sqrt(4)";
	std::cout << mth::solve(equ) << '\n';

	return 0;
}