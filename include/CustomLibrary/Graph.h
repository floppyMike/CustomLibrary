#pragma once

#include <vector>
#include <queue>
#include <tuple>
#include <iostream>

#include "Traits.h"

namespace ctl::gph
{
	// -----------------------------------------------------------------------------
	// Traits
	// -----------------------------------------------------------------------------

	template<typename T>
	concept SimpleGraph = requires(const T a)
	{
		typename T::Edge_t;

		{
			std::get<0>(a.neighbors((size_t)0)[0])
		}
		->std::same_as<const size_t &>;
		{
			a.node_amount()
		}
		->std::same_as<size_t>;
	};

	// -----------------------------------------------------------------------------
	// Edge Types
	// -----------------------------------------------------------------------------

	template<typename... T>
	using EdgeWith = std::tuple<size_t, T...>;

	using Edge = EdgeWith<>;

	template<arithmetic Weight, typename... T>
	using WeightedEdgeWith = EdgeWith<Weight, T...>;

	template<arithmetic Weight>
	using WeightedEdge = WeightedEdgeWith<Weight>;

	// -----------------------------------------------------------------------------
	// Graph
	// -----------------------------------------------------------------------------

	template<typename Edge>
	class Graph
	{
	public:
		using Edge_t = Edge;

		constexpr Graph()				   = default;
		constexpr Graph(const Graph &)	   = default;
		constexpr Graph(Graph &&) noexcept = default;

		constexpr auto operator=(const Graph &) -> Graph & = default;
		constexpr auto operator=(Graph &&) noexcept -> Graph & = default;

		constexpr Graph(std::initializer_list<std::initializer_list<Edge>> &&init)
		{
			m_edges.reserve(init.size());
			for (auto i = init.begin(); i != init.end(); ++i) m_edges.emplace_back(std::move(*i));
		}

		// constexpr void bidirectionilize() noexcept
		// {
		// 	for (size_t i = 0; i < m_edges.size(); ++i)
		// 		for (Edge e : m_edges[i])
		// 			if (const auto idx = std::exchange(std::get<0>(e), i);
		// 				std::find_if(m_edges[idx].begin(), m_edges[idx].end(),
		// 							 [idx](const Edge &e) { return std::get<0>(e) == idx; })
		// 				== m_edges[idx].end())
		// 				m_edges[idx].emplace_back(e);
		// }

		[[nodiscard]] constexpr auto neighbors(size_t id) const noexcept -> const auto & { return m_edges[id]; }
		[[nodiscard]] constexpr auto node_amount() const noexcept -> size_t { return m_edges.size(); }

		constexpr void push(size_t id, Edge &&e) { m_edges.emplace_back(std::move(e)); }

	private:
		std::vector<std::vector<Edge>> m_edges;
	};

	// -----------------------------------------------------------------------------
	// Algorithms
	// -----------------------------------------------------------------------------

	/**
	 * @brief Uses the breadth first search algorithm to map out a graph and return a vector for the shortest path for
	 * each node.
	 *
	 * @tparam Graph Type satisfying SimpleGraph
	 * @tparam F Unary predicate for early exiting (size_t index)
	 * @param g Graph to seach on
	 * @param start_node Node index to start mapping from
	 * @param early_exit Predicate
	 * @return Row of indexes each pointing to another index in direction of the start node
	 */
	template<SimpleGraph Graph, std::predicate<size_t> F>
	[[nodiscard]] auto breadth_first_search(const Graph &g, size_t start_node, F early_exit) noexcept
		-> std::vector<size_t>
	{
		std::queue<size_t> front; // Store nodes to query
		front.push(start_node);

		std::vector<size_t> came_from(g.node_amount(), -1); // Map routes
		came_from[start_node] = start_node;

		while (!front.empty())
		{
			const auto c = front.front();
			front.pop();

			if (early_exit(c))
				break;

			for (const auto &i : g.neighbors(c)) // Visit all neighbors
			{
				const auto next = std::get<0>(i);

				if (came_from[next] == -1) // Mark visited and store route
				{
					front.push(next);
					came_from[next] = c;
				}
			}
		}

		return came_from;
	}

	/**
	 * @brief Uses the breadth first search algorithm to map out a graph and return a vector for the shortest path for
	 * each node.
	 *
	 * @tparam Graph Type satisfying SimpleGraph
	 * @param g Graph to seach on
	 * @param start_node Node index to start mapping from
	 * @return Row of indexes each pointing to another index in direction of the start node
	 */
	template<SimpleGraph Graph>
	[[nodiscard]] auto breadth_first_search(const Graph &g, size_t start_node) noexcept -> std::vector<size_t>
	{
		return breadth_first_search(
			g, start_node, [](size_t) constexpr { return false; });
	}

	/**
	 * @brief Uses the dijkstra search algorithm to map out a graph and return a vector for the shortest path
	 *
	 * @tparam Graph Type satisfying SimpleGraph
	 * @tparam F Binary predicate for early exiting (size_t index, Weight weight)
	 * @param g Graph to seach on
	 * @param start_node Node index to start mapping from
	 * @param early_exit Predicate
	 * @return Pair of vectors: Row of indexes towards the start_node; Total weigh for each destination
	 */
	template<SimpleGraph Graph, std::predicate<size_t, std::tuple_element_t<1, typename Graph::Edge_t>> F>
	[[nodiscard]] auto dijkstra_search(const Graph &g, size_t start_node, F early_exit)
	{
		using Weight	= std::tuple_element_t<1, typename Graph::Edge_t>;
		using PQElement = std::pair<Weight, size_t>;

		std::priority_queue<PQElement> front; // Always take shortest route
		front.emplace((Weight)0, start_node);

		auto route = std::make_pair(
			std::vector<size_t>(g.node_amount(), -1),
			std::vector<Weight>(g.node_amount(),
								std::numeric_limits<Weight>::max())); // Visited node route and total weight
		route.first[start_node]	 = start_node;
		route.second[start_node] = (Weight)0;

		while (!front.empty())
		{
			const auto [w, c] = front.top();
			front.pop();

			if (early_exit(c, w))
				break;

			for (const auto &i : g.neighbors(c)) // Go through all neighbors of the current node
			{
				const auto [next, weight] = i;
				const auto cost			  = route.second[c] + weight;

				if (route.second[next] == std::numeric_limits<Weight>::max()
					|| cost < route.second[next]) // If not visited or previous route to node weighs more
				{
					route.second[next] = cost;
					route.first[next]  = c;
					front.emplace(cost, next);
				}
			}
		}

		return route;
	}

	/**
	 * @brief Uses the dijkstra search algorithm to map out a graph and return a vector for the shortest path
	 *
	 * @tparam Graph Type satisfying SimpleGraph
	 * @param g Graph to seach on
	 * @param start_node Node index to start mapping from
	 * @return Pair of vectors: Row of indexes towards the start_node; Total weigh for each destination
	 */
	template<SimpleGraph Graph>
	[[nodiscard]] auto dijkstra_search(const Graph &g, size_t start)
	{
		return dijkstra_search(g, start, [](size_t, auto) { return false; });
	}

	/**
	 * @brief Uses the A* search algorithm to map out a graph and return a vector for the shortest path. Similar to
	 * Dijkstra's algorithm.
	 *
	 * @tparam Graph Type satisfying SimpleGraph
	 * @tparam F1 Binary predicate for early exiting (size_t index, Weight weight)
	 * @tparam F2 Unary heuristic for calculating score to goal (size_t index)
	 * @param g Graph to seach on
	 * @param start_node Node index to start mapping from
	 * @param early_exit Predicate
	 * @param heuristic heuristic for goal
	 * @return Row of indexes towards the start_node
	 */
	template<SimpleGraph Graph, std::predicate<size_t, std::tuple_element_t<1, typename Graph::Edge_t>> F1,
			 std::predicate<size_t> F2>
	[[nodiscard]] auto a_star(const Graph &g, size_t start_node, F1 early_exit, F2 heuristic)
	{
		using Weight	= std::tuple_element_t<1, typename Graph::Edge_t>;
		using PQElement = std::pair<Weight, size_t>;

		std::priority_queue<PQElement> front; // Always take shortest route
		front.emplace((Weight)0, start_node);

		std::vector<size_t> route(g.node_amount(), -1);
		std::vector<Weight> distance(g.node_amount(),
									 std::numeric_limits<Weight>::max()); // Visited node route and total weight
		route[start_node]	 = start_node;
		distance[start_node] = (Weight)0;

		while (!front.empty())
		{
			const auto [w, c] = front.top();
			front.pop();

			if (early_exit(c, w))
				break;

			for (const auto &i : g.neighbors(c)) // Go through all neighbors of the current node
			{
				const auto [next, weight] = i;
				const auto cost			  = distance[c] + weight;

				if (distance[next] == std::numeric_limits<Weight>::max()
					|| cost < distance[next]) // If not visited or previous route to node weighs more
				{
					distance[next] = cost;
					route[next]	   = c;
					front.emplace(cost + heuristic(next), next);
				}
			}
		}

		return route;
	}

	/**
	 * @brief Uses the A* search algorithm to map out a graph and return a vector for the shortest path. Similar to
	 * Dijkstra's algorithm.
	 *
	 * @tparam Graph Type satisfying SimpleGraph
	 * @tparam F Unary heuristic for calculating score to goal (size_t index)
	 * @param g Graph to seach on
	 * @param start_node Node index to start mapping from
	 * @param early_exit Predicate
	 * @param heuristic heuristic for goal
	 * @return Row of indexes towards the start_node
	 */
	template<SimpleGraph Graph, std::predicate<size_t> F>
	[[nodiscard]] auto a_star(const Graph &g, size_t start, size_t goal, F heuristic)
	{
		return a_star(
			g, start, [&goal](size_t c, auto) { return c == goal; }, heuristic);
	}

	/*
	struct isDirected {};
	struct isUndirected {};
	struct isBoth {};

	using Vertex = size_t;

	struct Weighted
	{
		struct Edge
		{
			Vertex source;
			Vertex dest;
			double weight;
		};
		struct Link
		{
			Vertex dest;
			double weight;
		};

	protected:
		void _push_(const Edge& e) { m_adjList[e.source].push_back({ e.dest, e.weight }); }
		void _pushRev_(const Edge& e) { m_adjList[e.dest].push_back({ e.source, e.weight }); }

		std::vector<std::vector<Link>> m_adjList;
	};

	struct NoWeight
	{
		struct Edge
		{
			Vertex source;
			Vertex dest;
		};
		struct Link
		{
			Vertex dest;
		};

	protected:
		void _push_(const Edge& e) { m_adjList[e.source].push_back({ e.dest }); }
		void _pushRev_(const Edge& e) { m_adjList[e.dest].push_back({ e.source }); }

		std::vector<std::vector<Link>> m_adjList;
	};


	template<typename Weight>
	class Direct : protected Weight
	{
	public:
		using direction_t = isDirected;
		using edge_t = typename Weight::Edge;
		using link_t = typename Weight::Link;

		Direct() = default;

		template<typename Iter>
		Direct(Iter begin, Iter end, size_t size)
		{
			static_assert(std::is_same_v<std::iterator_traits<Iter>::value_type, edge_t>, "Must be of type Edge");

			this->m_adjList.resize(size);
			for (; begin != end; ++begin)
				this->_push_(*begin);
		}

		auto& push(const edge_t& e)
		{
			if (this->m_adjList.size() < e.source + 1)
				this->m_adjList.resize(e.source + 1);

			this->_push_(e);

			return *this;
		}

	};

	template<typename Weight>
	class Undirect : protected Weight
	{
	public:
		using direction_t = isUndirected;
		using edge_t = typename Weight::Edge;
		using link_t = typename Weight::Link;

		Undirect() = default;

		template<typename Iter>
		Undirect(Iter begin, Iter end, size_t size)
		{
			static_assert(std::is_same_v<std::iterator_traits<Iter>::value_type, edge_t>, "Must be of type Edge");

			this->m_adjList.resize(size);
			for (; begin != end; ++begin)
				this->_push_(*begin),
				this->_pushRev_(*begin);
		}

		auto& push(const edge_t& e)
		{
			const auto max = std::max(e.source + 1, e.dest + 1);
			if (this->m_adjList.size() < max)
				this->m_adjList.resize(max);

			this->_push_(e);
			this->_pushRev_(e);

			return *this;
		}

	};


	template<typename Direction, template<typename> class... Func>
	class Graph : public Direction, public Func<Graph<Direction, Func...>>...
	{
	public:
		using direction_t = typename Direction::direction_t;
		using edge_t = typename Direction::edge_t;
		using link_t = typename Direction::link_t;

		using Direction::Direction;

		auto begin() const
		{
			return this->m_adjList.begin();
		}
		auto end() const
		{
			return this->m_adjList.end();
		}

		const auto& linesFrom(size_t n) const
		{
			return this->m_adjList.at(n);
		}

		size_t size() const noexcept
		{
			return this->m_adjList.size();
		}

		void extendTo(size_t size)
		{
			this->m_adjList.resize(size);
		}
	};


	template<typename ImplGraph>
	class Connected : public crtp<ImplGraph, Connected>
	{
		//Mark all unchecked nodes
		std::vector<bool>& _goThrough_(std::vector<bool>& check, Vertex nextNode) const
		{
			check[nextNode] = true;
			//Find node that's not been activated and recurse to it. If none found go back in stack.
			for (auto& i : this->_().linesFrom(nextNode))
				if (!check[i.dest])
					_goThrough_(check, i.dest);
			return check;
		}

	public:
		bool connected(Vertex start = 0) const
		{
			std::vector<bool> table(this->_().size(), false);
			for (const auto& i : _goThrough_(table, start))
				if (!i)
					return false;
			return true;
		}

		bool connected(Vertex start, Vertex with, std::vector<bool>&& checkList = std::vector<bool>(this->_().size(),
	false))
		{
			return _goThrough_(checkList, start)[with];
		}
	};


	template<typename ImplGraph>
	class Dijkstra : public crtp<ImplGraph, Dijkstra>
	{
	public:
		auto dijkstra(Vertex start) const
		{
			using link_t = typename ImplGraph::link_t;

			std::priority_queue<link_t, std::vector<link_t>, bool (*)(const link_t&, const link_t&)> pq
			([](const link_t& l, const link_t& r) { return l.weight > r.weight; });

			std::vector<double> dist(this->_().size(), std::numeric_limits<double>::max());
			std::vector<bool> visited(this->_().size(), false);

			pq.push({ start, 0. });
			dist[start] = 0.;

			while (!pq.empty())
			{
				const auto current = pq.top().dest;
				pq.pop();
				visited[current] = true;

				//Check if vertex is marked and new calculated distance is lower than the currently saved one.
				for (const auto& x : this->_().linesFrom(current))
					if (!visited[x.dest] && dist[x.dest] > dist[current] + x.weight)
					{
						dist[x.dest] = dist[current] + x.weight;
						pq.push({ x.dest, dist[x.dest] });
					}
			}

			return dist;
		}

		//auto dijkstraWPath(Vertex start) const
		//{
		//	std::priority_queue<mth::Line, std::vector<mth::Line>, bool (*)(const mth::Line&, const mth::Line&)> pq
		//	([](const mth::Line& l, const mth::Line& r) { return l.weight > r.weight; });

		//	std::vector<double> dist(this->_().size(), std::numeric_limits<double>::max());
		//	std::vector<bool> visited(this->_().size(), false);

		//	std::vector<size_t> path(this->_().size());

		//	pq.emplace(0., start);
		//	dist[start] = 0.;
		//	//0 vertex points to nothing
		//	path.front() = -1;

		//	while (!pq.empty())
		//	{
		//		const auto current = pq.top().dest;
		//		pq.pop();
		//		visited[current] = true;

		//		for (const auto& x : this->_()[current])
		//			if (!visited[x.dest] && dist[x.dest] > dist[current] + x.weight)
		//			{
		//				dist[x.dest] = dist[current] + x.weight;
		//				pq.emplace(dist[x.dest], x.dest);

		//				path[x.dest] = current;
		//			}
		//	}

		//	return make_pair(dist, path);
		//}
	};


	template<typename ImplGraph>
	class Transpose : public crtp<ImplGraph, Transpose>
	{
	public:
		static_assert(!std::is_same_v<typename ImplGraph::direction_t, isUndirected>, "Graph must be directed to use
	this.");

		//Reserse directions of edges
		ImplGraph transpose() const
		{
			ImplGraph out;
			out.extendTo(this->_().size());

			for (size_t i = 0; i < this->_().size(); ++i)
				for (const auto& iter : this->_().linesFrom(i))
					out.push({ iter.dest, i, iter.weight });

			return out;
		}
	};


	//Graph must have the connected function
	template<typename ImplGraph>
	class Eulerian : public crtp<ImplGraph, Eulerian>
	{
	public:
		static_assert(!std::is_same_v<typename ImplGraph::direction_t, isDirected>, "Graph must be undirected to use
	this.");

		enum { NONE, PATH, CIRCLE };

		Eulerian eulerian() const
		{
			if (!this->_().connected())
				return NONE;

			uint8_t odds = 0;
			for (const auto& i : this->_())
				if (i.size() & 1)
				{
					++odds;
					if (odds > 2) //no point continuing
						return NONE;
				}

			switch (odds)
			{
			case 0:  return CIRCLE;
			case 2:  return PATH;
			default: return NONE;
			}
		}
	};


	template<typename ImplGraph>
	class MST : public crtp<ImplGraph, MST>
	{
	public:
		static_assert(!std::is_same_v<typename ImplGraph::direction_t, isDirected>, "Graph must be undirected to use
	this.");

		auto minimumSpanningTree() const
		{
			using link_t = typename ImplGraph::link_t;

			std::priority_queue<link_t, std::vector<link_t>, bool (*)(const link_t&, const link_t&)> pq
			([](const link_t& l, const link_t& r) { return l.weight > r.weight; });

			std::vector<double> key(this->_().size(), std::numeric_limits<double>::max());
			std::vector<bool> inMST(this->_().size(), false);

			//Path for new graph
			std::vector<link_t> parent(this->_().size());

			pq.push({ 0, 0. });
			key[0] = 0.;

			while (!pq.empty())
			{
				const auto current = pq.top().dest;
				pq.pop();

				inMST[current] = true;

				for (const auto& i : this->_().linesFrom(current))
					if (!inMST[i.dest] && key[i.dest] > i.weight)
					{
						key[i.dest] = i.weight;
						pq.push({ i.dest, i.weight });

						parent[i.dest] = { current, i.weight };
					}
			}

			ImplGraph mst;
			mst.extendTo(this->_().size());
			for (size_t i = 1; i < parent.size(); ++i)
				mst.push({ i, parent[i].dest, parent[i].weight });

			return mst;
		}
	};
*/
} // namespace ctl::gph