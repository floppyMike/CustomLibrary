#include <iostream>
#include "CustomLibrary/Equation.h"
#include "CustomLibrary/Matrix.h"

using namespace ctl;

template<typename T, typename Alloc>
auto operator<<(std::ostream &out, const mth::Matrix<T, Alloc> &m) noexcept -> auto &
{
	for (size_t i = 0, delim = m.dim().w; i < m.dim().area(); ++i)
	{
		if (i == delim)
			delim += m.dim().w, out << '\n';

		out << m[i] << '\t';
	}

	return out;
}

auto main() -> int
{
	const mth::Matrix<int> m1(3, 3, 1);
	const mth::Matrix<int> m2(3, 3, 4);

	const auto test = m1 + m2;

	std::cout << test << "\n\n";
	
	const mth::Matrix<double> m3(2, 2, { 4, 2, 0, 3 });
	const mth::Matrix<double> m4(2, 2, { 4, 0, 1, 4 });

	const auto test1 = m3.transpose();

    std::cout << m3 << "\n\n" << test1 << "\n\n" << m3.dot_product(m4) << "\n\n";

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