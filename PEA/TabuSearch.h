#pragma once
#include "Algorithm.h"
#include "Graph.h"
#include <random>

namespace Algorithm
{
	class TabuSearch
	{
		TabuSearch() {}

		static void swap(std::mt19937 & engine, unsigned int size, unsigned int * cycle);

	public:
		static unsigned int * getMinimalCycle(std::shared_ptr<Graph> graph, unsigned int iterationCount);
	};
}
