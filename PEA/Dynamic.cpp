#include "pch.h"
#include "Dynamic.h"
#include <deque>
using std::vector;
using std::deque;
using std::pair;
using std::tuple;
using std::bitset;

unsigned int * Algorithm::Dynamic::getMinimalCycle(std::shared_ptr<Graph> graph)
{
	if (graph && graph->getSize() <= maxCities)
	{
		const unsigned int graphSize_1 = graph->getSize() - 1;
		// Lookup buffer for already computed shortest paths
		vector<vector<pair<int, vector<unsigned int>>>> bestBuffer((1 << (graphSize_1)) - 1,
			std::move(vector<pair<int, vector<unsigned int>>>(graphSize_1,
				pair<int, vector<unsigned int>>(INT_MAX, std::move(vector<unsigned int>()))))); 
		// Queue for next vertices to check: current vertex, cost so far, visited till now, current path
		deque<tuple<unsigned int, int, bitset<maxCities - 1>, vector<unsigned int>>> vertexQueue;
		for (unsigned int i = graphSize_1; i > 0; --i)
			vertexQueue.push_front(std::make_tuple(i, graph->getCost(0, i), 1 << (i - 1), std::move(vector<unsigned int>({ i }))));

		while (vertexQueue.size())
		{
			auto next = vertexQueue.front();
			vertexQueue.pop_front();
			if (std::get<2>(next).count() == graphSize_1 || bestBuffer.at(std::get<2>(next).to_ullong() - 1).at(std::get<0>(next) - 1).first < std::get<1>(next))
				continue;
			for (unsigned int i = graphSize_1; i > 0 ; --i)
			{
				if (!std::get<2>(next)[i - 1])
				{
					vector<unsigned int> path = std::get<3>(next);
					path.emplace_back(i);
					tuple<unsigned int, int, bitset<maxCities - 1>, vector<unsigned int>> node = 
						std::make_tuple(i, graph->getCost(std::get<0>(next), i) + std::get<1>(next), (1ULL << (i - 1)) | std::get<2>(next).to_ullong(), std::move(path));
					if (std::get<1>(node) < bestBuffer.at(std::get<2>(node).to_ullong() - 1).at(std::get<0>(node) - 1).first)
					{
						bestBuffer.at(std::get<2>(node).to_ullong() - 1).at(std::get<0>(node) - 1).first = std::get<1>(node);
						bestBuffer.at(std::get<2>(node).to_ullong() - 1).at(std::get<0>(node) - 1).second = std::get<3>(node);
					}
					vertexQueue.push_front(std::move(node));
				}
			}
		}
		const unsigned int cycleBytes = graphSize_1 * sizeof(unsigned int);
		int bestCost = INT_MAX;
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		bestCycle[0] = 0;
		for (auto & item : bestBuffer.back())
		{
			int cost = item.first + graph->getCost(item.second.back(), 0);
			if (cost < bestCost)
			{
				bestCost = cost;
				memmove_s(bestCycle + 1, cycleBytes, item.second.data(), cycleBytes);
			}
		}
		return bestCycle;
	}
	return nullptr;
}
