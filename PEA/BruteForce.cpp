#include "pch.h"
#include "BruteForce.h"
#include <deque>
using std::vector;
using std::deque;
using std::tuple;
using std::pair;

namespace Algorithm
{
	pair<long long, vector<unsigned long long>> BruteForce::getMinimalCycleRecur(std::shared_ptr<Graph> graph,
		unsigned long long vertex, long long cost, vector<unsigned long long> path, vector<bool> notVisited)
	{
		const unsigned long long graphSize_1 = graph->getSize() - 1;
		if (path.size() == graphSize_1)
			return { cost + graph->getCost(vertex, 0), std::move(path) };
		unsigned long long i = 0;
		pair<long long, vector<unsigned long long>> best(LLONG_MAX, {});
		for (unsigned long long i = 1; i <= graphSize_1; ++i)
		{
			if (notVisited.at(i - 1))
			{
				vector<unsigned long long> nextPath = path;
				nextPath.emplace_back(i);
				vector<bool> nextNotVisited = notVisited;
				nextNotVisited.at(i - 1) = false;
				auto answer = getMinimalCycleRecur(graph, i, cost + graph->getCost(vertex, i), std::move(nextPath), std::move(nextNotVisited));
				if (answer.first < best.first)
					best = std::move(answer);
			}
		}
		return best;
	}

	unsigned long long * BruteForce::getMinimalCycleRecur(std::shared_ptr<Graph> graph)
	{
		if (graph)
		{
			const unsigned long long cycleBytes = (graph->getSize() - 1) * sizeof(unsigned long long);
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			bestCycle[0] = 0;
			auto best = getMinimalCycleRecur(graph, 0, 0, {}, std::move(vector<bool>(graph->getSize() - 1, true)));
			memmove_s(bestCycle + 1, cycleBytes, best.second.data(), cycleBytes);
			return bestCycle;
		}
		return nullptr;
	}

	unsigned long long * BruteForce::getMinimalCycleSlow(std::shared_ptr<Graph> graph)
	{
		if (graph)
		{
			const unsigned long long graphSize_1 = graph->getSize() - 1;
			const unsigned long long cycleBytes = graphSize_1 * sizeof(unsigned long long);
			long long bestCost = LLONG_MAX;
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			bestCycle[0] = 0;

			// Queue for next vertices to check: current vertex, cost so far, visited till now, current path, not visited vertices
			deque<tuple<unsigned long long, long long, vector<unsigned long long>, vector<bool>>> vertexQueue;
			for (unsigned long long i = graphSize_1; i > 0; --i)
			{
				vector<bool> notVisited(graphSize_1, true);
				notVisited.at(i - 1) = false;
				vertexQueue.push_front(std::make_tuple(i, graph->getCost(0, i), std::move(vector<unsigned long long>({ i })), std::move(notVisited)));
			}
			while (vertexQueue.size())
			{
				auto next = vertexQueue.front();
				vertexQueue.pop_front();
				if (std::get<2>(next).size() == graphSize_1)
				{
					long long cost = std::get<1>(next) + graph->getCost(std::get<0>(next), 0);
					if (cost < bestCost)
					{
						bestCost = cost;
						memmove_s(bestCycle + 1, cycleBytes, std::get<2>(next).data(), cycleBytes);
					}
					continue;
				}
				for (unsigned long long i = 1; i <= graphSize_1; ++i)
				{
					if (std::get<3>(next).at(i - 1))
					{
						vector<unsigned long long> path = std::get<2>(next);
						path.emplace_back(i);
						vector<bool> notVisited = std::get<3>(next);
						notVisited.at(i - 1) = false;
						vertexQueue.push_front(std::move(std::make_tuple(i, graph->getCost(std::get<0>(next), i) + std::get<1>(next), std::move(path), std::move(notVisited))));
					}
				}
			}
			return bestCycle;
		}
		return nullptr;
	}

	unsigned long long * BruteForce::getMinimalCycle(std::shared_ptr<Graph> graph)
	{
		if (graph)
		{
			unsigned long long * cycle = generateCycle(graph->getSize(), false);
			long long cost = graph->getCost(cycle);
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			long long bestCost = cost;
			std::shared_ptr<PermutationSeed> seed = getNewSeed(graph->getSize());
			const unsigned long long cycleSize = graph->getSize() * sizeof(unsigned long long);
			while (seed->index != seed->size)
			{
				getNextPermutation(cycle, seed);
				cost = graph->getCost(cycle);
				if (cost < bestCost)
				{
					memcpy_s(bestCycle, cycleSize, cycle, cycleSize);
					bestCost = cost;
				}
			}
			freeSeed(seed);
			return bestCycle;
		}
		return nullptr;
	}

	unsigned long long * BruteForce::getMinimalCycleOpt(std::shared_ptr<Graph> graph)
	{
		if (graph)
		{
			unsigned long long * cycle = generateCycle(graph->getSize(), false);
			long long cost = graph->getCost(cycle);
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			long long bestCost = cost;
			std::shared_ptr<PermutationSeed> seed = getNewSeed(graph->getSize() - 1);
			const unsigned long long cycleSize = graph->getSize() * sizeof(unsigned long long);
			while (seed->index != seed->size)
			{
				getNextPermutation(cycle + 1, seed);
				cost = graph->getCost(cycle);
				if (cost < bestCost)
				{
					memcpy_s(bestCycle, cycleSize, cycle, cycleSize);
					bestCost = cost;
				}
			}
			freeSeed(seed);
			return bestCycle;
		}
		return nullptr;
	}
}
