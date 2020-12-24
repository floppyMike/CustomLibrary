#include <iostream>
#include <CustomLibrary/Graph.h>

using namespace ctl;

int main()
{
	// --------------------------------- Graphs -----------------------------------------

	// clang-format off
	gph::Graph<gph::Edge> graph1({  // Each row represents a node and it's neighbors marked with their rows
		{ 1 }, 
		{ 2 }, 
		{ 1, 3, 5 }, 
		{ 2, 4 }, 
		{ 5 }, 
		{} 
	});

	gph::Graph<gph::WeightedEdge<uint8_t>> graph2({
		{ { 1, 4 }, { 7, 8 } },
		{ { 7, 11 }, { 2, 8 } },
		{ { 8, 2 }, { 5, 4 }, { 3, 7 } },
		{ { 4, 9 }, { 5, 14 } },
		{ { 5, 10 } },
		{ { 6, 2 } },
		{ { 8, 6 }, { 7, 1 } },
		{ { 8, 7 } },
		{}
	});
	// clang-format on

	// --------------------------------- breadth first -----------------------------------------

	std::cout << "Reachable from 0 in graph1\n";
	for (size_t from : gph::breadth_first_search(graph1, 0)) std::cout << from << '\n';

	std::cout << "Reachable from 0 in graph2\n";
	for (size_t from : gph::breadth_first_search(graph2, 0)) std::cout << from << '\n';

	std::cout << std::endl;

	// --------------------------------- Dijkstra -----------------------------------------

	std::cout << "Shortest path from 0 in graph2\n";
	const auto [route, weights] = gph::dijkstra_search(graph2, 0);
	for (size_t i = 0; i < route.size(); ++i) std::cout << i << " -> " << route[i] << " with " << +weights[i] << '\n';

	return 0;
}