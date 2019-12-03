#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class BranchAndBound
	{
		BranchAndBound() {}

		static bool isBetterMST(unsigned int current, int cost, int bestCost, std::vector<unsigned int> & path, std::shared_ptr<Graph> graph);

	public:
		static unsigned int * getMinimalCycle(std::shared_ptr<Graph> graph, bool depth = true);
		static unsigned int * getMinimalCycleBS(std::shared_ptr<Graph> graph);
	};
}
