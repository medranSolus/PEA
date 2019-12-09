#pragma once
#define _USE_MATH_DEFINES
#include "Algorithm.h"
#include "Graph.h"
#include <cmath>
#include <random>

namespace Algorithm
{
	class SimulatedAnnealing
	{
		SimulatedAnnealing() {}

		static bool shouldMove(std::mt19937_64 & engine, long long cost, long long localBestCost, float T);

	public:
		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph, MoveType moveType,
			float T, float tFactor, float minT, unsigned long long localSearchTime);
	};
}
