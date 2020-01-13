#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class AntColony
	{
	public:
		AntColony() = delete;

		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph, unsigned long long alpha, unsigned long long beta,
			unsigned long long pheromoneFactor, unsigned long long pheromoneLevel);
	};
}
