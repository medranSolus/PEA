#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class BranchAndBound
	{
		BranchAndBound() {}

		static bool isBetterMST(unsigned long long current, long long cost, long long bestCost, std::vector<unsigned long long> & path, std::shared_ptr<Graph> graph);

	public:
		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph, bool depth = true);
		static unsigned long long * getMinimalCycleBS(std::shared_ptr<Graph> graph);
	};
}
