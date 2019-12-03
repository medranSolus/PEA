#include "pch.h"
#include "BranchAndBound.h"
#include "PriorityQueue.h"
#include <deque>
#include <algorithm>
using std::vector;
using std::deque;
using std::tuple;
using std::get;

bool Algorithm::BranchAndBound::isBetterMST(unsigned int current, int cost, int bestCost, std::vector<unsigned int> & path, std::shared_ptr<Graph> graph)
{
	//http://radoslaw.grymin.staff.iiar.pwr.edu.pl/publications/BandB.pdf
	path.push_back(current);
	cost += graph->getMST(path) + graph->getShortests(current);//MST * 2 ?
	path.pop_back();
	return cost < bestCost;
}

unsigned int * Algorithm::BranchAndBound::getMinimalCycle(std::shared_ptr<Graph> graph, bool depth)
{
	if (graph)
	{
		const unsigned int graphSize_1 = graph->getSize() - 1;
		const unsigned int cycleBytes = graphSize_1 * sizeof(unsigned int);
		int bestCost = graph->getUpperBound();
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		bestCycle[0] = 0;

		// Queue for next vertices to check: current vertex, cost so far, current path
		deque<tuple<unsigned int, int, vector<unsigned int>>> vertexQueue;
		for (unsigned int i = graphSize_1; i > 0; --i)
			vertexQueue.push_front(std::make_tuple(i, graph->getCost(0, i), std::move(vector<unsigned int>({ i }))));
		tuple<unsigned int, int, vector<unsigned int>> next;
		unsigned int i;
		auto checkChild = [&]() -> bool
		{
			int cost = graph->getCost(get<0>(next), i) + get<1>(next);
			if (isBetterMST(i, cost, bestCost, get<2>(next), graph)) // Check bound
			{
				vector<unsigned int> path = get<2>(next);
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

unsigned int * Algorithm::BranchAndBound::getMinimalCycleBS(std::shared_ptr<Graph> graph)
{
	if (graph)
	{
		const unsigned int graphSize_1 = graph->getSize() - 1;
		const unsigned int cycleBytes = graphSize_1 * sizeof(unsigned int);
		int bestCost = graph->getUpperBound();
		unsigned int * bestCycle = new unsigned int[graph->getSize()];
		bestCycle[0] = 0;

		// Queue for next vertices to check: current vertex, cost so far, current path
		PriorityQueue<tuple<unsigned int, int, vector<unsigned int>>> vertexQueue(1ULL << (graph->getSize() >> 1));
		for (unsigned int i = graphSize_1; i > 0; --i)
			vertexQueue.push(std::make_tuple(i, graph->getCost(0, i), std::move(vector<unsigned int>({ i }))));
		tuple<unsigned int, int, vector<unsigned int>> next;
		unsigned int i;
		auto checkChild = [&]() -> bool
		{
			int cost = graph->getCost(get<0>(next), i) + get<1>(next);
			if (isBetterMST(i, cost, bestCost, get<2>(next), graph)) // Check bound
			{
				vector<unsigned int> path = get<2>(next);
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
