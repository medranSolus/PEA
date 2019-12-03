#include "pch.h"
#include "Algorithm.h"
#include <iostream>

unsigned int * Algorithm::generateCycle(unsigned int vertices, bool isRandom)
{
	unsigned int * cycle = new unsigned int[vertices];
	for (unsigned int i = 0; i < vertices; ++i)
		cycle[i] = i;
	if (isRandom) // Fisher-Yates shuffle (Porównaæ!!!)
		for (unsigned int i = vertices, position, swap; i > 1; --i)
		{
			position = rand() % i;
			swap = cycle[position];
			cycle[position] = cycle[i - 1];
			cycle[i - 1] = swap;
		}
	return cycle;
}

void Algorithm::showCycle(unsigned int * cycle, const unsigned int count)
{
	if (cycle && count)
	{
		std::cout << std::endl;
		for (unsigned int i = 0; i < count; ++i)
			std::cout << cycle[i] << "->";
		std::cout << cycle[0] << std::endl;
	}
	else
		std::cout << "\nCycle empty\n";
}

std::shared_ptr<Algorithm::PermutationSeed> Algorithm::getNewSeed(unsigned int count)
{
	std::shared_ptr<PermutationSeed> seed = std::make_shared<PermutationSeed>();
	seed->size = count;
	seed->control = new unsigned int[seed->size + 1];
	for (unsigned int i = 0; i < seed->size; ++i)
		seed->control[i] = i;
	seed->control[seed->size] = seed->size;	// control[size] > 0 controls iteration and the index boundary for index
	return seed;
}

void Algorithm::freeSeed(std::shared_ptr<PermutationSeed> seed)
{
	if (seed)
	{
		if (seed->control)
			delete[] seed->control;
	}
}

// NOTICE:  Copyright 1991-2008, Phillip Paul Fuchs
void Algorithm::getNextPermutation(unsigned int * cycle, std::shared_ptr<PermutationSeed> seed) // QuickPerm Algorithm
{
	const unsigned int lastIndex = seed->size - 1;
	unsigned int temp, swap;
	--seed->control[seed->index];										// decrease index "weight" for i by one
	temp = lastIndex - seed->index % 2 * seed->control[seed->index];	// IF i is odd then j = ax - p[i] otherwise j = ax
	seed->index = lastIndex - seed->index;								// adjust i to permute tail (i < j)
	swap = cycle[temp];
	cycle[temp] = cycle[seed->index];
	cycle[seed->index] = swap;

	for (seed->index = 1; !seed->control[seed->index]; ++seed->index)	// Reset control zero values
		seed->control[seed->index] = seed->index;
}
