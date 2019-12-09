#include "pch.h"
#include "SimulatedAnnealing.h"

namespace Algorithm
{
	bool SimulatedAnnealing::shouldMove(std::mt19937_64 & engine, long long cost, long long localBestCost, float T)
	{
		return std::uniform_real_distribution<float>(0.0f, 1.0f)(engine) < (1.0f / (1.0f + pow(M_E, static_cast<float>(cost - localBestCost) / T)));
	}

	unsigned long long * SimulatedAnnealing::getMinimalCycle(std::shared_ptr<Graph> graph, MoveType moveType,
		float T, float tFactor, float minT, unsigned long long localSearchTime)
	{
		// Random generate, after some time decrease T (smaller T == more searching in local optima than traveling)
		const unsigned long long size = graph->getSize();
		const unsigned long long size_1 = size - 1;
		const unsigned long long cycleBytes = size * sizeof(unsigned long long);
		std::mt19937_64 engine(std::random_device{}());

		unsigned long long * bestCycle = nullptr;
		long long bestCost = graph->getInitialCycleRandom(&bestCycle);
		long long cost = bestCost, localBestCost = bestCost;

		unsigned long long * cycle = new unsigned long long[size];
		memcpy_s(cycle, cycleBytes, bestCycle, cycleBytes);

		unsigned long long * localBestCycle = new unsigned long long[size];
		memcpy_s(localBestCycle, cycleBytes, bestCycle, cycleBytes);

		while (T >= minT)
		{
			for (unsigned long long j = 0; j < localSearchTime; ++j)
			{
				Pair pair = getRandom(engine, size_1, moveType);
				const long long dCost = graph->getCost(cycle, pair, moveType);
				move(pair, cycle, moveType);
				if(dCost == -1)
					cost = graph->getCost(cycle);
				else
					cost += graph->getCost(cycle, pair, moveType, false) - dCost;
				if (cost < localBestCost || shouldMove(engine, cost, localBestCost, T))
				{
					localBestCost = cost;
					memcpy_s(localBestCycle, cycleBytes, cycle, cycleBytes);
					if (localBestCost < bestCost)
					{
						bestCost = localBestCost;
						memcpy_s(bestCycle, cycleBytes, localBestCycle, cycleBytes);
					}
				}
			}
			float alpha = 1.0f / static_cast<float>(abs(log(sqrtf(tFactor * log(T) * sqrtf((static_cast<float>(bestCost) / localBestCost) / cost) * minT) / log(minT))));
			if (alpha < 1.0f)
				T *= alpha;
			else
				T /= alpha;
		}
		delete[] cycle;
		delete[] localBestCycle;
		return bestCycle;
	}
}
