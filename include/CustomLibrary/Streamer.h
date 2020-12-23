#pragma once

#include "CustomLibrary/Traits.h"

#include <ostream>
#include <istream>

// -----------------------------------------------------------------------------
// Matrix Streams
// -----------------------------------------------------------------------------

#ifdef _CTL_MATRIX_
template<typename T, typename A>
auto operator<<(std::ostream &o, const ctl::mth::Matrix<T, A> &m) noexcept -> std::ostream &
{
	const auto dim = m.dim();
	o << dim.h << ' ' << dim.w << ' ';

	for (auto i : m) o << i << ' ';

	return o;
}

template<typename T, typename A>
auto operator>>(std::istream &in, ctl::mth::Matrix<T, A> &m) -> std::istream &
{
	size_t r, c;
	in >> r >> c;

	m = ctl::mth::Matrix<T, A>(r, c);

	for (size_t i = 0; i < m.size(); ++i)
	{
		double b;
		in >> b;
		m.emplace_back(b);
	}

	return in;
}
#endif

// -----------------------------------------------------------------------------
// Neural Network Streams
// -----------------------------------------------------------------------------

#ifdef _CTL_NEURAL_NET_
auto operator<<(std::ostream &o, const ctl::mcl::BasicNeuralNetwork &nn) noexcept -> std::ostream &
{
	o << nn.layers_n() << ' ';
	for (auto i = nn.begin(); i != nn.end(); ++i) o << i->biases << ' ' << i->weights << ' ';

	return o;
}

auto operator>>(std::istream &in, ctl::mcl::BasicNeuralNetwork &nn) noexcept -> std::istream &
{
	ctl::mcl::BasicNeuralNetwork new_nn;

	size_t n;
	in >> n;

	for (; n > 0 + 1; --n) // Account for 1st layer
	{
		ctl::mth::Matrix<double> w, b;
		in >> b >> w;

		new_nn.add_layer(w, b);
	}

	nn = std::move(new_nn);

	return in;
}
#endif