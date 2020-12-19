#include <iostream>

#include <CustomLibrary/NeuralNet.h>

using namespace ctl;

rnd::Random<rnd::Mersenne> g_rand;

int main()
{
	mcl::BasicNeuralNetwork nn({ 2, 1 }, g_rand);

	mth::Matrix<double> input[]	 = { { 2, 1, { .5, .5 } }, { 2, 1, { 1, 1 } },
									 { 2, 1, { 1, 0 } },  { 2, 1, { 0, 1 } },	{ 2, 1, { .5, 1 } },
									 { 2, 1, { 0, .5 } }, { 2, 1, { .5, 0 } },	{ 2, 1, { 1, .5 } } };
	mth::Matrix<double> output[] = { { 1, 1, { 0 } }, { 1, 1, { 1 } },
									 { 1, 1, { 0 } }, { 1, 1, { 0 } }, { 1, 1, { 1 } },
									 { 1, 1, { 0 } }, { 1, 1, { 0 } }, { 1, 1, { 1 } } };

	for (auto in = input; in != input + 8; ++in) std::cout << nn.query(*in)(0, 0) << "\t";

	for (size_t i = 0; i < 1000; ++i)
	{
		for (auto in = input, out = output; in != input + 8; ++in, ++out) mcl::fit(nn, *in, *out, 0.1);

		if (i % 100 == 0)
			std::cout << "i = " << i << "\tCost: " << cost(nn, input, input + 8, output) << std::endl;
	}

	for (auto in = input; in != input + 8; ++in) std::cout << nn.query(*in)(0, 0) << "\t";

	return 0;
}