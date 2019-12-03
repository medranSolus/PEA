#include "pch.h"
#include "TabuSearch.h"

void Algorithm::TabuSearch::swap(std::mt19937 & engine, unsigned int size, unsigned int * cycle)
{
	unsigned int i = std::uniform_int_distribution<unsigned int>(1, size)(engine);
	unsigned int j;
	do
	{
		j = std::uniform_int_distribution<unsigned int>(1, size)(engine);
	} while (j == i);
	size = cycle[i];
	cycle[i] = cycle[j];
	cycle[j] = size;
}

unsigned int * Algorithm::TabuSearch::getMinimalCycle(std::shared_ptr<Graph> graph, unsigned int iterationCount)
{
	// http://www.eng.uwaterloo.ca/~sjayaswa/projects/MSCI703_project.pdf
	const unsigned int size = graph->getSize();
	const unsigned int size_1 = size - 1;
	const unsigned int cycleBytes = size * sizeof(unsigned int);

	std::mt19937 engine(std::random_device{}());
	unsigned int * bestCycle = nullptr;
	int bestCost = graph->getInitialCycle(&bestCycle);
	int cost = bestCost;
	unsigned int * cycle = new unsigned int[size];
	memcpy_s(cycle, cycleBytes, bestCycle, cycleBytes);

	for (; iterationCount > 0; --iterationCount)
	{
		swap(engine, size_1, cycle);
		cost = graph->getCost(cycle);
		if (cost < bestCost)
		{
			bestCost = cost;
			memcpy_s(bestCycle, cycleBytes, cycle, cycleBytes);
		}
	}
	delete[] cycle;
	return bestCycle;
}
