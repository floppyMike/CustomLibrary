#include <charconv>
#include <iostream>
#include <fstream>

#include <CustomLibrary/NeuralNet.h>
#include <CustomLibrary/RandomGenerator.h>

#include <CustomLibrary/Streamer.h>

using namespace ctl;

rnd::Random<rnd::Mersenne> g_rand;

auto highest(const mth::Matrix<double> &e) { return std::distance(e.begin(), std::max_element(e.begin(), e.end())); }

int main(int argc, char **argv)
{
	const auto init = [] { return g_rand.rand_number(-1., 1.); };

	// --------------------------------- Basic -----------------------------------------

	// Very basic neural network
	mcl::BasicNeuralNetwork nn1({ 2, 1 }, init);

	// Test data
	mth::Matrix<double> input[] = {
		{ 2, 1, { .5, .5 } }, { 2, 1, { 1, 1 } },  { 2, 1, { 1, 0 } },	{ 2, 1, { 0, 1 } },
		{ 2, 1, { .5, 1 } },  { 2, 1, { 0, .5 } }, { 2, 1, { .5, 0 } }, { 2, 1, { 1, .5 } }
	};
	mth::Matrix<double> output[] = { { 1, 1, { 0 } }, { 1, 1, { 1 } }, { 1, 1, { 0 } }, { 1, 1, { 0 } },
									 { 1, 1, { 1 } }, { 1, 1, { 0 } }, { 1, 1, { 0 } }, { 1, 1, { 1 } } };

	// See start situation
	for (auto in = input; in != input + 8; ++in) std::cout << nn1.query(*in)(0, 0) << "\t";
	std::cout << std::endl;

	// Train the network
	for (size_t i = 0; i < 1000; ++i)
	{
		for (auto in = input, out = output; in != input + 8; ++in, ++out) mcl::fit(nn1, *in, *out, 0.1);

		if (i % 100 == 0)
			std::cout << "i = " << i << "\tCost: " << cost(nn1, input, input + 8, output) << std::endl;
	}

	// Look at the results
	for (auto in = input; in != input + 8; ++in) std::cout << nn1.query(*in)(0, 0) << "\t";
	std::cout.put('\n') << std::endl;


	return 0;
}