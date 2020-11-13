#include <iostream>
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

	auto test = m1 + m2;

	std::cout << test << "\n\n";
	
	const mth::Matrix<double> m3(2, 2, { 4, 2, 0, 3 });
	const mth::Matrix<double> m4(2, 2, { 4, 0, 1, 4 });

	auto test1 = m3.transpose();

    std::cout << m3 << "\n\n" << test1 << "\n\n" << m3.dot_product(m4);

	return 0;
}