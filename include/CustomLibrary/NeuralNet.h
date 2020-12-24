#ifndef _CTL_NEURAL_NET_
#define _CTL_NEURAL_NET_

#include "utility.h"
#include "Matrix.h"

#include <cmath>

namespace ctl::mcl
{
	/**
	 * @brief Simple Layer descriptor
	 */
	struct Layer
	{
		mth::Matrix<double> weights, biases;
	};

	// -----------------------------------------------------------------------------
	// Neural Networks
	// -----------------------------------------------------------------------------

	/**
	 * @brief Basic Neural Network implementation using the feedforward as the query method and backpropogation as the
	 * training method
	 */
	class BasicNeuralNetwork
	{
	public:
		/**
		 * @brief Construct a empty neural network
		 */
		BasicNeuralNetwork() = default;

		/**
		 * @brief Construct a new Basic Neural Network object with a certain structure
		 *
		 * @tparam Init Initializer Functor
		 * @param structure Defines the structure of the neural network. For example { 2, 2, 1 } means 2 inputs, 1
		 * hidden layer with 2 neurons, 1 output
		 * @param init Functor used to initialize the network. Sig.: double func();
		 */
		template<typename Init>
		BasicNeuralNetwork(std::initializer_list<size_t> &&structure, Init &&init) requires std::invocable<Init>
			: m_neurons_n(structure)
		{
			m_layers.reserve(m_neurons_n.size() - 1);

			for (auto i = m_neurons_n.begin(), end = m_neurons_n.end() - 1; i != end; ++i)
				m_layers.push_back({ mth::Matrix<double>(*(i + 1), *i, init), mth::Matrix<double>(*(i + 1), 1, init) });
		}

		/**
		 * @brief Add a additional layer to the neural network
		 *
		 * @param weights Weights to include
		 * @param biases Biases to include
		 */
		void add_layer(const mth::Matrix<double> &weights, const mth::Matrix<double> &biases)
		{
			if (m_neurons_n.empty())
				m_neurons_n.emplace_back(weights.dim().w);

			assert(weights.dim().w == m_neurons_n.back()
				   && "Matrix column size must match the neurons of the previous last layer.");

			m_neurons_n.emplace_back(weights.dim().h);
			m_layers.push_back({ weights, biases });
		}

		/**
		 * @brief Returns a weight matrix of a layer
		 * @param idx layer index
		 * @return weight matrix reference
		 */
		auto weights(size_t idx) noexcept -> auto &
		{
			assert(idx < m_layers.size() && "Weights of specified layer don't exist.");
			return m_layers[idx].weights;
		}

		/**
		 * @brief Returns a bias matrix of a layer
		 * @param idx layer index
		 * @return bias matrix reference
		 */
		auto biases(size_t idx) noexcept -> auto &
		{
			assert(idx < m_layers.size() && "Biases of specified layer don't exist.");
			return m_layers[idx].biases;
		}

		/**
		 * @brief Returns the number of layers within the network
		 * @return layer number
		 */
		[[nodiscard]] auto layers_n() const noexcept -> size_t { return m_neurons_n.size(); }

		/**
		 * @brief Returns the number of neurons in a layer
		 * @param idx Layer index
		 * @return Neuron amount
		 */
		[[nodiscard]] auto nodes(size_t idx) const noexcept -> size_t
		{
			assert(idx < m_layers.size() && "Layer doesn't exist.");
			return m_neurons_n[idx];
		}

		/**
		 * @brief Performs a feedforward query with given input
		 * @param input Matrix input to use
		 * @return result matrix
		 */
		[[nodiscard]] auto query(mth::Matrix<double> input) const
		{
			for (const auto &i : m_layers) // Feed forward through each layer
				input = (i.biases + i.weights.dot_product(input)).apply(sigmoid<double>);
			return input;
		}

		[[nodiscard]] auto begin() const noexcept { return m_layers.begin(); }
		[[nodiscard]] auto begin() noexcept { return m_layers.begin(); }

		[[nodiscard]] auto rbegin() const noexcept { return m_layers.rbegin(); }
		[[nodiscard]] auto rbegin() noexcept { return m_layers.rbegin(); }

		[[nodiscard]] auto end() const noexcept { return m_layers.end(); }
		[[nodiscard]] auto end() noexcept { return m_layers.end(); }

		[[nodiscard]] auto rend() const noexcept { return m_layers.rend(); }
		[[nodiscard]] auto rend() noexcept { return m_layers.rend(); }

	private:
		std::vector<size_t> m_neurons_n;
		std::vector<Layer>	m_layers;
	};

	// -----------------------------------------------------------------------------
	// Actions
	// -----------------------------------------------------------------------------

	/**
	 * @brief Performs the training method of the given neural network using a input and output matrix
	 *
	 * @param nn Neural Network to train on
	 * @param input Input matrix to fit with
	 * @param output Output matrix to fit with
	 * @param learning_rate The learning speed
	 */
	void fit(BasicNeuralNetwork &nn, const mth::Matrix<double> &input, const mth::Matrix<double> &output,
			 double learning_rate = 0.1)
	{
		// Feedforward results
		std::vector<mth::Matrix<double>> feedforward;
		feedforward.reserve(nn.layers_n());

		feedforward.emplace_back(input); // Input is first result

		for (const auto &layer : nn) // Feedforward and store each result
			feedforward.emplace_back(
				(layer.weights.dot_product(feedforward.back()) + layer.biases).apply(sigmoid<double>));

		// Loss calculation
		std::vector<mth::Matrix<double>> loss;
		loss.reserve(nn.layers_n() - 1);

		loss.emplace_back(output - feedforward.back()); // Loss of prediction to output

		for (auto ri = nn.rbegin(); ri != nn.rend(); ++ri) // Create losses using backward weight iteration
			loss.emplace_back(ri->weights.transpose().dot_product(loss.back()));

		// Calculate delta
		for (auto [i_error, i_output, i_layer] = std::tuple{ loss.begin(), feedforward.rbegin(), nn.rbegin() };
			 i_layer != nn.rend(); ++i_error, ++i_output, ++i_layer)
		{
			const auto d_bias	= learning_rate * *i_error * *i_output * (1. - *i_output);
			const auto d_weight = d_bias.dot_product((i_output + 1)->transpose());

			i_layer->weights += d_weight;
			i_layer->biases += d_bias;
		}
	}

	/**
	 * @brief Calculates the total cost/loss of the network to the given output array using the input array. Input and
	 * output array length must be the same!
	 *
	 * @param nn Neural Networt to calculate the cost with
	 * @param input_begin The input array begin address
	 * @param input_end The input array end address
	 * @param output_begin The output array begin address
	 * @return total cost as a double
	 */
	template<typename Iter1, typename Iter2>
	auto cost(
		const BasicNeuralNetwork &nn, Iter1 input_begin, Iter1 input_end,
		Iter2 output_begin) requires std::same_as<mth::Matrix<double>, typename std::iterator_traits<Iter1>::value_type>
		&&std::same_as<mth::Matrix<double>, typename std::iterator_traits<Iter2>::value_type>
	{
		auto cost = 0.;
		for (; input_begin != input_end; ++input_begin, ++output_begin)
		{
			const auto pred = nn.query(*input_begin);
			for (auto v : *output_begin - pred) cost += v * v;
		}

		return cost;
	}

} // namespace ctl::mcl

#endif