#if not defined _CTL_GRAPH_
#define _CTL_GRAPH_

#include <vector>
#include <queue>
#include <tuple>
#include <iostream>

#include "Traits.h"

namespace ctl::gph
{
	// -----------------------------------------------------------------------------
	// Structures
	// -----------------------------------------------------------------------------

	struct Edge
	{
		size_t dest;
	};

	struct WeighedEdge
	{
		size_t	 dest;
		uint32_t weight;
	};

	// -----------------------------------------------------------------------------
	// Graph
	// -----------------------------------------------------------------------------

	template<iter_matches<Edge, WeighedEdge> IterEdge, iter_matches<size_t> IterIdx>
	struct Graph
	{
		IterEdge edges; // Edges begin of graph
		IterIdx	 idx;	// Range separators of graph
		size_t	 size;	// Size of graph
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
	template<typename T, typename U, std::predicate<size_t> F>
	[[nodiscard]] auto breadth_first_search(const Graph<T, U> &g, size_t start_node, F early_exit) noexcept
		-> std::vector<size_t>
	{
		std::queue<size_t> front; // Store nodes to query
		front.push(start_node);

		std::vector<size_t> came_from(g.size, -1); // Map routes
		came_from[start_node] = start_node;

		while (!front.empty())
		{
			const auto c = front.front();
			front.pop();

			if (early_exit(c))
				break;

			for (auto i = g.edges + g.idx[c], e = g.edges + g.idx[c + 1]; i != e; ++i) // Visit all neighbors
			{
				const auto next = i->dest;

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
	template<typename T, typename U>
	[[nodiscard]] auto breadth_first_search(const Graph<T, U> &g, size_t start_node) noexcept -> std::vector<size_t>
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
	template<typename T, typename U, std::predicate<size_t, uint32_t> F>
	[[nodiscard]] auto dijkstra_search(const Graph<T, U> &g, size_t start_node, F early_exit)
	{
		std::priority_queue<std::pair<uint32_t, size_t>> front; // Always take shortest route
		front.emplace(0u, start_node);

		auto route = std::make_pair(std::vector<size_t>(g.size, -1),
									std::vector<uint32_t>(g.size, -1)); // Visited node route and total weight

		route.first[start_node]	 = start_node;
		route.second[start_node] = 0u;

		while (!front.empty())
		{
			const auto [w, idx] = front.top();
			front.pop();

			if (early_exit(idx, w))
				break;

			for (auto i = &g.edges[g.idx[idx]]; i != &g.edges[g.idx[idx + 1]];
				 ++i) // Go through all neighbors of the current node
			{
				const auto [next, weight] = *i;
				const auto cost			  = route.second[idx] + weight;

				if (route.second[next] == -1
					|| cost < route.second[next]) // If not visited or previous route to node weighs more
				{
					route.second[next] = cost;
					route.first[next]  = idx;
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
	template<typename T, typename U>
	[[nodiscard]] auto dijkstra_search(const Graph<T, U> &g, size_t start)
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
	template<typename T, typename U, std::predicate<size_t, uint32_t> F1, std::predicate<size_t> F2>
	[[nodiscard]] auto a_star(const Graph<T, U> &g, size_t start_node, F1 early_exit, F2 heuristic)
	{
		std::priority_queue<std::pair<uint32_t, size_t>> front; // Always take shortest route
		front.emplace(0u, start_node);

		auto route = std::make_pair(std::vector<size_t>(g.size, -1),
									std::vector<uint32_t>(g.size, -1)); // Visited node route and total weight

		route.first[start_node]	 = start_node;
		route.second[start_node] = 0u;

		while (!front.empty())
		{
			const auto [w, idx] = front.top();
			front.pop();

			if (early_exit(idx, w))
				break;

			for (const auto &i : g.neighbors(idx)) // Go through all neighbors of the current node
			{
				const auto [next, weight] = i;
				const auto cost			  = route.second[idx] + weight;

				if (route.first[next] == -1
					|| cost < route.second[next]) // If not visited or previous route to node weighs more
				{
					route.first[next]  = idx;
					route.second[next] = cost;
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
	template<typename T, typename U, std::predicate<size_t> F>
	[[nodiscard]] auto a_star(const Graph<T, U> &g, size_t start, size_t goal, F heuristic)
	{
		return a_star(
			g, start, [&goal](size_t c, auto) { return c == goal; }, heuristic);
	}

} // namespace ctl::gph

#endif