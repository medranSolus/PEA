#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class BruteForce
	{
		static std::pair<long long, std::vector<unsigned long long>> getMinimalCycleRecur(std::shared_ptr<Graph> graph, 
			unsigned long long vertex, long long cost, std::vector<unsigned long long> path, std::vector<bool> visited);

	public:
		BruteForce() = delete;

		static unsigned long long * getMinimalCycleRecur(std::shared_ptr<Graph> graph);
		static unsigned long long * getMinimalCycleSlow(std::shared_ptr<Graph> graph);
		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph);
		static unsigned long long * getMinimalCycleOpt(std::shared_ptr<Graph> graph);
	};
}
