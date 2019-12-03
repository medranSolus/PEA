#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class BruteForce
	{
		BruteForce() {}

		static std::pair<int, std::vector<unsigned int>> getMinimalCycleRecur(std::shared_ptr<Graph> graph, 
			unsigned int vertex, int cost, std::vector<unsigned int> path, std::vector<bool> visited);

	public:
		static unsigned int * getMinimalCycleRecur(std::shared_ptr<Graph> graph);
		static unsigned int * getMinimalCycleSlow(std::shared_ptr<Graph> graph);
		static unsigned int * getMinimalCycle(std::shared_ptr<Graph> graph);
		static unsigned int * getMinimalCycleOpt(std::shared_ptr<Graph> graph);
	};
}
