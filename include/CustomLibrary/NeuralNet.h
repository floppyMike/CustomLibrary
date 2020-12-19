#pragma once

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
				m_layers.push_back(
					{ mth::Matrix<double>(*(i + 1), *i, init()), mth::Matrix<double>(*(i + 1), 1, init()) });
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
	 * @brief Calculates the total cost/loss of the network to the given output array using the input array. Input and output array length must be the same!
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

	/*
		class NeuralNet
		{
		public:
			using Data = std::pair<std::vector<double>, std::vector<bool>>;

			//---------------------------------------------------------
			//--------------------Constructors-------------------------
			//---------------------------------------------------------

			NeuralNet() = default;
			NeuralNet(const NeuralNet &) = default;
			NeuralNet(NeuralNet &&) = default;

			template<typename Gen>
			NeuralNet(const std::initializer_list<size_t> &neurons, const ctl::NumVec<double, 2> &initRange,
	   RandomGen<Gen> &rand) : m_neurons(neurons.begin(), neurons.end())
			{
				m_connections.reserve(m_neurons.size() - 1);

				for (auto i = m_neurons.begin(), length = m_neurons.end() - 1; i != length; ++i)
					m_connections.push_back({
						Matrix<double>({ *i, *(i + 1) }, 0.).randomize(rand, initRange),
						Matrix<double>({ 1, *(i + 1) }, 0.).randomize(rand, initRange) });
			}
			NeuralNet(const std::initializer_list<size_t> &neurons)
				: m_neurons(neurons.begin(), neurons.end())
			{
				m_connections.reserve(m_neurons.size() - 1);

				for (size_t i = 0, length = m_neurons.size() - 1; i < length; ++i)
					m_connections.push_back({ Matrix<double>(), Matrix<double>() });
			}

			//---------------------------------------------------------
			//----------------------Operators--------------------------
			//---------------------------------------------------------

			NeuralNet& operator=(const NeuralNet &) = default;
			NeuralNet& operator=(NeuralNet &&) = default;

			const auto& neurons() const { return m_neurons; }
			const auto& connections() const { return m_connections; }

			//---------------------------------------------------------
			//------------------------Methods--------------------------
			//---------------------------------------------------------

			auto& train(const Data &d, const double &learnRate)
			{
				if (d.first.size() != m_neurons.front() || d.second.size() != m_neurons.back())
					throw err::Log("Neural Network: train: Data is inconsistant with amount of neurons.",
	   Log::Severity::ERR0R);

				//Calculate output and store it
				std::vector<Matrix<double>> neuronOutput;
				neuronOutput.reserve(m_neurons.size());

				neuronOutput.emplace_back(d.first.begin(), d.first.end());

				for (size_t i = 0, length = neuronOutput.capacity() - 1; i < length; ++i)
					neuronOutput.emplace_back((m_connections[i][0].dotProduct(neuronOutput[i]) +
	   m_connections[i][1]).apply(sigmoid));


				//Calculate output error
				Matrix<double> o_Error(NumVec<size_t, 2>{ 1, m_neurons.back() });
				for (size_t i = 0; i < m_neurons.back(); ++i)
					o_Error.emplace_back(neuronOutput.back()(0, i) - d.second[i]);


				//Calculate output errors
				std::vector<Matrix<double>> output_errors;
				output_errors.reserve(m_connections.size());

				output_errors.emplace_back(std::move(o_Error));

				{
					auto iterCon = m_connections.rbegin();
					auto iterOut = output_errors.begin();

					for (; std::distance(output_errors.begin(), iterOut) + 1 <
	   static_cast<ptrdiff_t>(output_errors.capacity()); ++iterOut, ++iterCon)
						output_errors.emplace_back((*iterCon)[0].transpose().dotProduct(*iterOut));
				}


				//Calculate delta
				{
					auto iterErrOut = output_errors.begin();
					auto iterNeuOut = neuronOutput.rbegin();
					auto iterCon	= m_connections.rbegin();

					for (; iterCon != m_connections.rend(); ++iterErrOut, ++iterNeuOut, ++iterCon)
					{
						Matrix<double> biasDelta(*iterErrOut * learnRate * *iterNeuOut * (1. - *iterNeuOut));
						Matrix<double> weightDelta(biasDelta.dotProduct((iterNeuOut + 1)->transpose()));

						//Sum delta
						(*iterCon)[0] += weightDelta;
						(*iterCon)[1] += biasDelta;
					}
				}

				return *this;
			}

			auto query(const std::vector<double> &d) const
			{
				//Check
				if (d.size() != m_neurons.front())
					throw err::Log("Neural Network: query: Data is inconsistant with amount of neurons.",
	   Log::Severity::ERR0R);

				//Feedforward
				Matrix<double> pred(d.begin(), d.end());
				for (const auto& i : m_connections)
					pred = (i[1] + i[0].dotProduct(pred)).apply(sigmoid);

				return pred;
			}

			template<typename Iter, typename = typename std::enable_if_t<std::is_same_v<typename
	   std::iterator_traits<Iter>::value_type, Data>>> auto cost(Iter begin, const Iter &end) const
			{
				double cost = 0.;
				for (; begin != end; ++begin)
				{
					const auto pred = query(begin->first);
					for (size_t i = 0; i < m_neurons.back(); ++i)
						cost += std::pow(pred(0, i) - begin->second[i], 2);
				}

				return cost;
			}

			auto& save(const std::string &fileName) const
			{
				std::ofstream file(fileName, std::ios::binary | std::ios::out);
				if (!file)
					throw err::Log("NeuralNet: save: file couldn't be created.", Log::Severity::ERR0R);

				for (const auto& iter : m_connections)
				{
					for (const auto& iterArr : iter)
					{
						//Store data
						file << iterArr.dim()[0] << ' ' << iterArr.dim()[1] << '\n';
						for (const auto& i : iterArr.data())
							file << i << ' ';
					}

					file << '\n';
				}

				return *this;
			}

			auto& open(const std::string &fileName)
			{
				std::ifstream file(fileName, std::ios::binary | std::ios::in);
				if (!file)
					throw err::Log("NeuralNet: open: file is empty or non existant.", Log::Severity::ERR0R);

				for (auto& conn : m_connections)
					for (auto& wei_bia : conn)
					{
						//Extract width and height
						NumVec<size_t, 2> dim;
						for (char i = 0; i < 2; ++i)
							file >> dim[i];

						wei_bia = Matrix<double>(dim);

						for (size_t i = 0, length = wei_bia.dim().product(); i < length; ++i)
						{
							double dat;
							file >> dat;

							wei_bia.emplace_back(std::move(dat));
						}
					}

				return *this;
			}

		private:
			using Weight_Bias = std::array<Matrix<double>, 2>;

			std::vector<Weight_Bias> m_connections;
			std::vector<size_t> m_neurons;
		};
		*/
} // namespace ctl::mcl