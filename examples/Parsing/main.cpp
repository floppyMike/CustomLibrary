#include <CustomLibrary/Parse.h>

using namespace ctl;

auto main() -> int
{
	std::string_view	  ex1 = "This is a demonstration of the SequentialParser.";
	par::SequentialParser seq_par(ex1);

	// Get string_view till space
	const auto fword = seq_par.get_until(' ');
	std::cout << fword.value_or("Delim not found.") << '\n';

	// Error handling
	const auto sword = seq_par.get_until('/');
	if (sword.has_value())
	{
		std::cout << sword.value() << '\n';
		return 0;
	}

	// See if a dot is available
	std::cout << (seq_par.find('.').has_value() ? "Dot is available" : "Dot isn't available") << '\n';

	return 0;
}