#include "pch.h"
#include "Algorithm.h"
#include <iostream>

namespace Algorithm
{
	unsigned long long * generateCycle(unsigned long long vertices, bool isRandom)
	{
		unsigned long long * cycle = new unsigned long long[vertices];
		for (unsigned long long i = 0; i < vertices; ++i)
			cycle[i] = i;
		if (isRandom) // Fisher-Yates shuffle (Porównaæ!!!)
			for (unsigned long long i = vertices, position, swap; i > 1; --i)
			{
				position = rand() % i;
				swap = cycle[position];
				cycle[position] = cycle[i - 1];
				cycle[i - 1] = swap;
			}
		return cycle;
	}

	void showCycle(unsigned long long * cycle, const unsigned long long count)
	{
		if (cycle && count)
		{
			std::cout << std::endl;
			for (unsigned long long i = 0; i < count; ++i)
				std::cout << cycle[i] << "->";
			std::cout << cycle[0] << std::endl;
		}
		else
			std::cout << "\nCycle empty\n";
	}

	std::shared_ptr<PermutationSeed> getNewSeed(unsigned long long count)
	{
		std::shared_ptr<PermutationSeed> seed = std::make_shared<PermutationSeed>();
		seed->size = count;
		seed->control = new unsigned long long[seed->size + 1];
		for (unsigned long long i = 0; i < seed->size; ++i)
			seed->control[i] = i;
		seed->control[seed->size] = seed->size;	// control[size] > 0 controls iteration and the index boundary for index
		return seed;
	}

	void freeSeed(std::shared_ptr<PermutationSeed> seed)
	{
		if (seed)
			if (seed->control)
				delete[] seed->control;
	}

	// NOTICE:  Copyright 1991-2008, Phillip Paul Fuchs
	void getNextPermutation(unsigned long long * cycle, std::shared_ptr<PermutationSeed> seed) // QuickPerm Algorithm
	{
		const unsigned long long lastIndex = seed->size - 1;
		unsigned long long temp, swap;
		--seed->control[seed->index];										// decrease index "weight" for i by one
		temp = lastIndex - seed->index % 2 * seed->control[seed->index];	// IF i is odd then j = ax - p[i] otherwise j = ax
		seed->index = lastIndex - seed->index;								// adjust i to permute tail (i < j)
		swap = cycle[temp];
		cycle[temp] = cycle[seed->index];
		cycle[seed->index] = swap;

		for (seed->index = 1; !seed->control[seed->index]; ++seed->index)	// Reset control zero values
			seed->control[seed->index] = seed->index;
	}

	Pair getRandom(std::mt19937_64 & engine, unsigned long long last, MoveType type)
	{
		unsigned long long j, i = std::uniform_int_distribution<unsigned long long>(1, last)(engine);
		if (type == MoveType::Insert)
		{
			do
			{
				j = std::uniform_int_distribution<unsigned long long>(1, last)(engine);
			} while (j == i || i - j == -1 || j - i == -1);
		}
		else
		{
			do
			{
				j = std::uniform_int_distribution<unsigned long long>(1, last)(engine);
			} while (j == i);
		}
		return { i, j };
	}

	void move(const Pair & pair, unsigned long long * cycle, MoveType type)
	{
		switch (type)
		{
		case MoveType::Swap:
			moveSwap(pair, cycle);
			break;
		case MoveType::Invert:
			moveInvert(pair, cycle);
			break;
		case MoveType::Insert:
			moveInsert(pair, cycle);
			break;
		}
	}

	void moveSwap(const Pair & pair, unsigned long long * cycle)
	{
		unsigned long long temp = cycle[pair.first];
		cycle[pair.first] = cycle[pair.second];
		cycle[pair.second] = temp;
	}

	void moveInvert(const Pair & pair, unsigned long long * cycle)
	{
		unsigned long long i, j, temp;
		if (pair.second < pair.first)
		{
			i = pair.second;
			j = pair.first;
		}
		else
		{
			i = pair.first;
			j = pair.second;
		}
		for (; i < j; ++i, --j)
		{
			temp = cycle[i];
			cycle[i] = cycle[j];
			cycle[j] = temp;
		}
	}

	void moveInsert(const Pair & pair, unsigned long long * cycle)
	{
		unsigned long long i = pair.first;
		unsigned long long temp = cycle[i];
		while (i < pair.second - 1)
		{
			cycle[i] = cycle[i + 1];
			++i;
		}
		while (i > pair.second)
		{
			cycle[i] = cycle[i - 1];
			--i;
		}
		cycle[i] = temp;
	}

	void revertMove(const Pair & pair, unsigned long long * cycle, MoveType type)
	{
		switch (type)
		{
		case MoveType::Swap:
			revertSwap(pair, cycle);
			break;
		case MoveType::Invert:
			revertInvert(pair, cycle);
			break;
		case MoveType::Insert:
			revertInsert(pair, cycle);
			break;
		}
	}

	void revertSwap(const Pair & pair, unsigned long long * cycle)
	{
		unsigned long long temp = cycle[pair.first];
		cycle[pair.first] = cycle[pair.second];
		cycle[pair.second] = temp;
	}

	void revertInvert(const Pair & pair, unsigned long long * cycle)
	{
		unsigned long long i, j, temp;
		if (pair.second < pair.first)
		{
			i = pair.second;
			j = pair.first;
		}
		else
		{
			i = pair.first;
			j = pair.second;
		}
		for (; i < j; ++i, --j)
		{
			temp = cycle[i];
			cycle[i] = cycle[j];
			cycle[j] = temp;
		}
	}

	void revertInsert(const Pair & pair, unsigned long long * cycle)
	{
		unsigned long long temp, j = pair.second;
		if (pair.first < j)
		{
			temp = cycle[--j];
			while (pair.first < j)
			{
				cycle[j] = cycle[j - 1];
				--j;
			}
		}
		else
		{
			temp = cycle[j];
			while (pair.first > j)
			{
				cycle[j] = cycle[j + 1];
				++j;
			}
		}
		cycle[j] = temp;
	}
}
