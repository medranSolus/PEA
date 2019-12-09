#include "pch.h"
#include "BranchAndBound.h"
#include "PriorityQueue.h"
#include <deque>
#include <algorithm>
using std::vector;
using std::deque;
using std::tuple;
using std::get;

namespace Algorithm
{
	bool BranchAndBound::isBetterMST(unsigned long long current, long long cost, long long bestCost, std::vector<unsigned long long> & path, std::shared_ptr<Graph> graph)
	{
		//http://radoslaw.grymin.staff.iiar.pwr.edu.pl/publications/BandB.pdf
		path.push_back(current);
		cost += graph->getMST(path) + graph->getShortests(current);//MST * 2 ?
		path.pop_back();
		return cost < bestCost;
	}

	unsigned long long * BranchAndBound::getMinimalCycle(std::shared_ptr<Graph> graph, bool depth)
	{
		if (graph)
		{
			const unsigned long long graphSize_1 = graph->getSize() - 1;
			const unsigned long long cycleBytes = graphSize_1 * sizeof(unsigned long long);
			long long bestCost = graph->getUpperBound();
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			bestCycle[0] = 0;

			// Queue for next vertices to check: current vertex, cost so far, current path
			deque<tuple<unsigned long long, long long, vector<unsigned long long>>> vertexQueue;
			for (unsigned long long i = graphSize_1; i > 0; --i)
				vertexQueue.push_front(std::make_tuple(i, graph->getCost(0, i), std::move(vector<unsigned long long>({ i }))));
			tuple<unsigned long long, long long, vector<unsigned long long>> next;
			unsigned long long i;
			auto checkChild = [&]() -> bool
			{
				long long cost = graph->getCost(get<0>(next), i) + get<1>(next);
				if (isBetterMST(i, cost, bestCost, get<2>(next), graph)) // Check bound
				{
					vector<unsigned long long> path = get<2>(next);
					path.emplace_back(i);
					if (path.size() == graphSize_1)
					{
						cost += graph->getCost(i, 0);
						if (cost < bestCost)
						{
							bestCost = cost;
							memmove_s(bestCycle + 1, cycleBytes, path.data(), cycleBytes);
						}
						return true;
					}
					if (depth)
						vertexQueue.push_front(std::move(std::make_tuple(i, cost, std::move(path))));
					else
						vertexQueue.push_back(std::move(std::make_tuple(i, cost, std::move(path))));
				}
				return false;
			};
		leafFound:
			while (vertexQueue.size())
			{
				next = vertexQueue.front();
				vertexQueue.pop_front();
				if (get<1>(next) < bestCost) // Check cost of current node (previous iteration could change bestCost)
				{
					auto sorted = get<2>(next);
					std::sort(sorted.begin(), sorted.end());
					i = 0;
					for (auto vertex : sorted)
						for (++i; i < vertex; ++i)
							if (checkChild())
								goto leafFound;
					for (++i; i <= graphSize_1; ++i)
						if (checkChild())
							break;
				}
			}
			return bestCycle;
		}
		return nullptr;
	}

	unsigned long long * BranchAndBound::getMinimalCycleBS(std::shared_ptr<Graph> graph)
	{
		if (graph)
		{
			const unsigned long long graphSize_1 = graph->getSize() - 1;
			const unsigned long long cycleBytes = graphSize_1 * sizeof(unsigned long long);
			long long bestCost = graph->getUpperBound();
			unsigned long long * bestCycle = new unsigned long long[graph->getSize()];
			bestCycle[0] = 0;

			// Queue for next vertices to check: current vertex, cost so far, current path
			PriorityQueue<tuple<unsigned long long, long long, vector<unsigned long long>>> vertexQueue(1ULL << (graph->getSize() >> 1));
			for (unsigned long long i = graphSize_1; i > 0; --i)
				vertexQueue.push(std::make_tuple(i, graph->getCost(0, i), std::move(vector<unsigned long long>({ i }))));
			tuple<unsigned long long, long long, vector<unsigned long long>> next;
			unsigned long long i;
			auto checkChild = [&]() -> bool
			{
				long long cost = graph->getCost(get<0>(next), i) + get<1>(next);
				if (isBetterMST(i, cost, bestCost, get<2>(next), graph)) // Check bound
				{
					vector<unsigned long long> path = get<2>(next);
					path.emplace_back(i);
					if (path.size() == graphSize_1)
					{
						cost += graph->getCost(i, 0);
						if (cost < bestCost)
						{
							bestCost = cost;
							memmove_s(bestCycle + 1, cycleBytes, path.data(), cycleBytes);
						}
						return true;
					}
					vertexQueue.push(std::move(std::make_tuple(i, cost, std::move(path))));
				}
				return false;
			};
		leafFound:
			while (vertexQueue.size())
			{
				next = vertexQueue.top();
				if (get<1>(next) < bestCost) // Check cost of current node (previous iteration could change bestCost)
				{
					auto sorted = get<2>(next);
					std::sort(sorted.begin(), sorted.end());
					i = 0;
					for (auto vertex : sorted)
						for (++i; i < vertex; ++i)
							if (checkChild())
								goto leafFound;
					for (++i; i <= graphSize_1; ++i)
						if (checkChild())
							break;
				}
				else
					break;
			}
			return bestCycle;
		}
		return nullptr;
	}
}
