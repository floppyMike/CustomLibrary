#include <charconv>
#include <iostream>
#include <fstream>

#include <CustomLibrary/NeuralNet.h>
#include <CustomLibrary/RandomGenerator.h>

#include <CustomLibrary/Streamer.h>

using namespace ctl;

rnd::Random<rnd::Mersenne> g_rand;

using MNIST_Entry = std::pair<mth::Matrix<double>, mth::Matrix<double>>;

auto load_mnist(std::string_view name)
{
	std::ifstream			 mnist_file(name.data(), std::ios::in);
	std::vector<MNIST_Entry> entries;

	assert(mnist_file && "File couldn't open.");

	for (char c; mnist_file.get(c);)
	{
		auto &entry = entries.emplace_back();

		entry.first			 = mth::Matrix<double>(10, 1, 0.);
		entry.first[c - '0'] = 1.;

		entry.second = mth::Matrix<double>(28 * 28, 1);

		while (mnist_file.get() != '\n')
		{
			std::string b;
			while (mnist_file.peek() != ',' && mnist_file.peek() != '\n') b.push_back(mnist_file.get());

			unsigned char c;
			std::from_chars(b.data(), b.data() + b.size(), c);

			entry.second.emplace_back(c / 255.);
		}
	}

	return entries;
}

auto highest(const mth::Matrix<double> &e) { return std::distance(e.begin(), std::max_element(e.begin(), e.end())); }

auto operator<<(std::ostream &o, const MNIST_Entry &e) -> std::ostream &
{
	for (size_t y = 0; y < 28; ++y)
	{
		for (size_t x = 0; x < 28; ++x) o.put(e.second[y * 28 + x] > 0 ? '#' : ' ');
		o.put('\n');
	}

	o << "Correct Value: " << highest(e.first);
	return o;
}

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

	// --------------------------------- MNIST -----------------------------------------

	mcl::BasicNeuralNetwork nn2({ 28 * 28, 200, 10 }, init);

	std::vector<std::pair<mth::Matrix<double>, mth::Matrix<double>>> set;

	if constexpr (false) // true ~> train the network; false ~> import trained network
	{
		set = load_mnist("mnist_train.csv");

		// Train the network
		std::clog << "Started training...\n";
		for (size_t epoch = 0; epoch < 5; ++epoch)
		{
			for (const auto &e : set) mcl::fit(nn2, e.second, e.first, .1);
			std::clog << "Finished epoch " << epoch << '\n';
		}

		std::ofstream file_out("data.nn", std::ios::binary);
		file_out << nn2;
		file_out.close();
	}
	else
	{
		std::ifstream file_in("data.nn", std::ios::binary);
		file_in >> nn2;
		file_in.close();
	}

	set = load_mnist("mnist_test.csv");

	// Evaluate network
	for (const auto &e : set)
	{
		const auto res = nn2.query(e.second);
		std::cout << e << "\nPredicted value: " << highest(res) << "\n\n Next?";

		::getchar();
	}

	return 0;
}