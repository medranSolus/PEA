#include "pch.h"
#include "TabuSearch.h"

namespace Algorithm
{
	void TabuSearch::decrementTabu(TabuMatrix & tabuList, const TabuMatrix & frequency)
	{
		unsigned long long min = ULLONG_MAX, x = 0, y = 0;
		const unsigned long long size = tabuList.size();
		for (unsigned long long i = 0; i < size; ++i)
			for (unsigned long long j = 0; j < size; ++j)
			{
				if (tabuList.at(i).at(j))
					--tabuList.at(i).at(j);
				if (frequency.at(i).at(j) > 0 && frequency.at(i).at(j) < min)
				{
					min = frequency.at(i).at(j);
					x = i;
					y = j;
				}
			}
		if (min < ULLONG_MAX)
			tabuList.at(x).at(y) >>= 1;
	}

	void TabuSearch::setTabu(TabuMatrix & tabuList, TabuMatrix & frequency, unsigned long long tabuTime, MoveType moveType, const Pair & pair)
	{
		tabuList.at(pair.first).at(pair.second) = tabuTime;
		++frequency.at(pair.first).at(pair.second);
		if (moveType != MoveType::Insert)
		{
			tabuList.at(pair.second).at(pair.first) = tabuTime;
			++frequency.at(pair.second).at(pair.first);
		}
	}

	void TabuSearch::makeMove(std::shared_ptr<Graph> graph, std::mt19937_64 & engine, MoveType moveType,
		TabuMatrix & tabuList, TabuMatrix & frequency, unsigned long long * cycle, long long * cost,
		long long bestCost, unsigned long long tabuTime, unsigned long long tabuRevokeTime)
	{
		Pair pair;
		long long dCostPrev = -1, dCostNext = -1;
		unsigned long long time = 0, size_1 = graph->getSize() - 1;
		unsigned long long start = std::uniform_int_distribution<unsigned long long>(1, size_1)(engine);
		unsigned long long i = start;
		do
		{
			for (unsigned long long j = 1; j <= size_1; ++j)
			{
				if (j == i)
					continue;
				if (!tabuList.at(i).at(j))
				{
					pair = { i,j };
					goto pairFound;
				}
				if (time > tabuRevokeTime)
				{
					pair = { i,j };
					dCostPrev = graph->getCost(cycle, pair, moveType);
					move(pair, cycle, moveType);
					if (dCostPrev == -1)
						*cost = graph->getCost(cycle);
					else
					{
						dCostNext = graph->getCost(cycle, pair, moveType, false);
						*cost += dCostNext - dCostPrev;
					}
					if (*cost < bestCost)
						return setTabu(tabuList, frequency, tabuTime, moveType, pair);
					revertMove(pair, cycle, moveType);
					*cost += dCostPrev - dCostNext;
				}
				++time;
			}
			i = (i + 1) % size_1;
			if (!i)
				i = 1;
		} while (true);
		pairFound:
		dCostPrev = graph->getCost(cycle, pair, moveType);
		move(pair, cycle, moveType);
		if (dCostPrev == -1)
			*cost = graph->getCost(cycle);
		else
			*cost += graph->getCost(cycle, pair, moveType, false) - dCostPrev;
		setTabu(tabuList, frequency, tabuTime, moveType, pair);
	}

	void TabuSearch::makeMoveRandom(std::shared_ptr<Graph> graph, std::mt19937_64 & engine, MoveType moveType,
		TabuMatrix & tabuList, TabuMatrix & frequency, unsigned long long * cycle, long long * cost,
		long long bestCost, unsigned long long tabuTime, unsigned long long tabuRevokeTime)
	{
		Pair pair;
		long long dCostPrev = -1, dCostNext = -1;
		unsigned long long last = graph->getSize() - 1;
		unsigned long long time = 0;
		do
		{
			if (time > tabuRevokeTime)
			{
				dCostPrev = graph->getCost(cycle, pair, moveType);
				move(pair, cycle, moveType);
				if (dCostPrev == -1)
					*cost = graph->getCost(cycle);
				else
				{
					dCostNext = graph->getCost(cycle, pair, moveType, false);
					*cost += dCostNext - dCostPrev;
				}
				if (*cost < bestCost)
					return setTabu(tabuList, frequency, tabuTime, moveType, pair);
				revertMove(pair, cycle, moveType);
				*cost += dCostPrev - dCostNext;
			}
			pair = getRandom(engine, last, moveType);
			++time;
		} while (tabuList.at(pair.first).at(pair.second));
		dCostPrev = graph->getCost(cycle, pair, moveType);
		move(pair, cycle, moveType);
		if (dCostPrev == -1)
			*cost = graph->getCost(cycle);
		else
			*cost += graph->getCost(cycle, pair, moveType, false) - dCostPrev;
		setTabu(tabuList, frequency, tabuTime, moveType, pair);
	}

	unsigned long long * TabuSearch::getMinimalCycle(std::shared_ptr<Graph> graph, InitialType initType, MoveType moveType, bool random,
		unsigned long long time, unsigned long long tabuTime, unsigned long long maxStagnationTime, unsigned long long tabuRevokeTime)
	{
		// http://www.eng.uwaterloo.ca/~sjayaswa/projects/MSCI703_project.pdf
		// https://www.researchgate.net/publication/268293729_Tabu_Search_Implementation_on_Traveling_Salesman_Problem_and_Its_Variations_A_Literature_Survey
		const unsigned long long size = graph->getSize();
		const unsigned long long size_1 = size - 1;
		const unsigned long long cycleBytes = size * sizeof(unsigned long long);
		TabuMatrix tabuList(size, TabuRow(size, 0));
		TabuMatrix frequency(size, TabuRow(size, 0));

		std::mt19937_64 engine(std::random_device{}());
		unsigned long long * bestCycle = nullptr;
		long long bestCost = 0;
		switch (initType)
		{
		case InitialType::NN:
			bestCost = graph->getInitialCycleNN(&bestCycle);
			break;
		case InitialType::Random:
			bestCost = graph->getInitialCycleRandom(&bestCycle);
			break;
		case InitialType::RandomNN:
			bestCost = graph->getInitialCycleRandomNN(&bestCycle);
			break;
		}
		long long cost = bestCost;
		unsigned long long * cycle = new unsigned long long[size];
		memcpy_s(cycle, cycleBytes, bestCycle, cycleBytes);

		for (unsigned long long stagnationTime = 0; time > 0; --time, ++stagnationTime)
		{
			if (random)
				makeMoveRandom(graph, engine, moveType, tabuList, frequency, cycle, &cost, bestCost, tabuTime, tabuRevokeTime);
			else
				makeMove(graph, engine, moveType, tabuList, frequency, cycle, &cost, bestCost, tabuTime, tabuRevokeTime);
			if (cost < bestCost)
			{
				stagnationTime = 0;
				bestCost = cost;
				memcpy_s(bestCycle, cycleBytes, cycle, cycleBytes);
			}
			if (stagnationTime > maxStagnationTime)
			{
				stagnationTime = 0;
				Pair pair = getRandom(engine, size_1, moveType);
				const long long dCost = graph->getCost(cycle, pair, moveType);
				move(pair, cycle, moveType);
				if (dCost == -1)
					cost = graph->getCost(cycle);
				else
					cost += graph->getCost(cycle, pair, moveType, false) - dCost;
				setTabu(tabuList, frequency, tabuTime, moveType, pair);
			}
			decrementTabu(tabuList, frequency);
		}
		delete[] cycle;
		return bestCycle;
	}
}
