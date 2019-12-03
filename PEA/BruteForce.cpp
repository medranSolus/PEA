#include "pch.h"
#include "BruteForce.h"
#include <deque>
using std::vector;
using std::deque;
using std::tuple;
using std::pair;

pair<int, vector<unsigned int>> Algorithm::BruteForce::getMinimalCycleRecur(std::shared_ptr<Graph> graph, 
	unsigned int vertex, int cost, vector<unsigned int> path, vector<bool> notVisited)
{
	const unsigned int graphSize_1 = graph->getSize() - 1;
	if (path.size() == graphSize_1)
		return { cost + graph->getCost(vertex, 0), std::move(path) };
	unsigned int i = 0;
	pair<int, vector<unsigned int>> best(INT_MAX, {});
	for (unsigned int i = 1; i <= graphSize_1; ++i)
	{
		if (notVisited.at(i - 1))
		{
			vector<unsigned int> nextPath = path;
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

unsigned int * Algorithm::BruteForce::getMinimalCycleRecur(std::shared_ptr<Graph> graph)
{
	if (graph)
	{
		const unsigned int cycleBytes = (graph->getSize() - 1) * sizeof(unsigned int);
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		bestCycle[0] = 0;
		auto best = getMinimalCycleRecur(graph, 0, 0, {}, std::move(vector<bool>(graph->getSize() - 1, true)));
		memmove_s(bestCycle + 1, cycleBytes, best.second.data(), cycleBytes);
		return bestCycle;
	}
	return nullptr;
}

unsigned int * Algorithm::BruteForce::getMinimalCycleSlow(std::shared_ptr<Graph> graph)
{
	if (graph)
	{
		const unsigned int graphSize_1 = graph->getSize() - 1;
		const unsigned int cycleBytes = graphSize_1 * sizeof(unsigned int);
		int bestCost = INT_MAX;
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		bestCycle[0] = 0;

		// Queue for next vertices to check: current vertex, cost so far, visited till now, current path, not visited vertices
		deque<tuple<unsigned int, int, vector<unsigned int>, vector<bool>>> vertexQueue;
		for (unsigned int i = graphSize_1; i > 0; --i)
		{
			vector<bool> notVisited(graphSize_1, true);
			notVisited.at(i - 1) = false;
			vertexQueue.push_front(std::make_tuple(i, graph->getCost(0, i), std::move(vector<unsigned int>({ i })), std::move(notVisited)));
		}
		while (vertexQueue.size())
		{
			auto next = vertexQueue.front();
			vertexQueue.pop_front();
			if (std::get<2>(next).size() == graphSize_1)
			{
				int cost = std::get<1>(next) + graph->getCost(std::get<0>(next), 0);
				if (cost < bestCost)
				{
					bestCost = cost;
					memmove_s(bestCycle + 1, cycleBytes, std::get<2>(next).data(), cycleBytes);
				}
				continue;
			}
			for (unsigned int i = 1; i <= graphSize_1; ++i)
			{
				if (std::get<3>(next).at(i - 1))
				{
					vector<unsigned int> path = std::get<2>(next);
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

unsigned int * Algorithm::BruteForce::getMinimalCycle(std::shared_ptr<Graph> graph)
{
	if (graph)
	{
		unsigned int * cycle = generateCycle(graph->getSize(), false);
		int cost = graph->getCost(cycle);
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		int bestCost = cost;
		std::shared_ptr<PermutationSeed> seed = getNewSeed(graph->getSize());
		const unsigned int cycleSize = graph->getSize() * sizeof(unsigned int);
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

unsigned int * Algorithm::BruteForce::getMinimalCycleOpt(std::shared_ptr<Graph> graph)
{
	if (graph)
	{
		unsigned int * cycle = generateCycle(graph->getSize(), false);
		int cost = graph->getCost(cycle);
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		int bestCost = cost;
		std::shared_ptr<PermutationSeed> seed = getNewSeed(graph->getSize() - 1);
		const unsigned int cycleSize = graph->getSize() * sizeof(unsigned int);
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
