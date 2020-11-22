#include <iostream>
#include <CustomLibrary/Graph.h>

using namespace ctl;

int main()
{
	// clang-format off
	gph::Graph<gph::Edge> graph({  // Each row represents a node and it's neighbors marked with their rows
		{ 1 }, 
		{ 2 }, 
		{ 1, 3, 5 }, 
		{ 2, 4 }, 
		{ 5 }, 
		{} 
	});
	// clang-format on

	std::cout << "Reachable from 0\n";
	for (size_t from : gph::breadth_first_search(graph, 0))
		std::cout << from << '\n';

	


	return 0;
}