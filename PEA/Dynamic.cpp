#include "pch.h"
#include "Dynamic.h"
#include <deque>
using std::vector;
using std::deque;
using std::pair;
using std::tuple;
using std::bitset;

namespace Algorithm
{
	unsigned long long * Dynamic::getMinimalCycle(std::shared_ptr<Graph> graph)
	{
		if (graph && graph->getSize() <= maxCities)
		{
			const unsigned long long graphSize_1 = graph->getSize() - 1;
			// Lookup buffer for already computed shortest paths
			vector<vector<pair<long long, vector<unsigned long long>>>> bestBuffer((1 << (graphSize_1)) - 1,
				std::move(vector<pair<long long, vector<unsigned long long>>>(graphSize_1,
					pair<long long, vector<unsigned long long>>(LLONG_MAX, std::move(vector<unsigned long long>())))));
			// Queue for next vertices to check: current vertex, cost so far, visited till now, current path
			deque<tuple<unsigned long long, long long, bitset<maxCities - 1>, vector<unsigned long long>>> vertexQueue;
			for (unsigned long long i = graphSize_1; i > 0; --i)
				vertexQueue.push_front(std::make_tuple(i, graph->getCost(0, i), 1 << (i - 1), std::move(vector<unsigned long long>({ i }))));

			while (vertexQueue.size())
			{
				auto next = vertexQueue.front();
				vertexQueue.pop_front();
				if (std::get<2>(next).count() == graphSize_1 || bestBuffer.at(std::get<2>(next).to_ullong() - 1).at(std::get<0>(next) - 1).first < std::get<1>(next))
					continue;
				for (unsigned long long i = graphSize_1; i > 0; --i)
				{
					if (!std::get<2>(next)[i - 1])
					{
						vector<unsigned long long> path = std::get<3>(next);
						path.emplace_back(i);
						tuple<unsigned long long, long long, bitset<maxCities - 1>, vector<unsigned long long>> node =
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
			const unsigned long long cycleBytes = graphSize_1 * sizeof(unsigned long long);
			long long bestCost = LLONG_MAX;
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			bestCycle[0] = 0;
			for (auto & item : bestBuffer.back())
			{
				long long cost = item.first + graph->getCost(item.second.back(), 0);
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
}
