#pragma once
#include <memory>
#include <bitset>
#include <random>

namespace Algorithm
{
	typedef std::pair<unsigned long long, unsigned long long> Pair;

	struct PermutationSeed
	{
		unsigned long long * control = nullptr;
		unsigned long long size = 0;
		unsigned long long index = 1; // First swap point (size - 1, size)
	};

	enum MoveType { Swap, Invert, Insert };

	unsigned long long * generateCycle(unsigned long long vertices, bool isRandom = true);

	void showCycle(unsigned long long * cycle, const unsigned long long count);

	std::shared_ptr<PermutationSeed> getNewSeed(unsigned long long count);

	void freeSeed(std::shared_ptr<PermutationSeed> seed);
	
	void getNextPermutation(unsigned long long * cycle, std::shared_ptr<PermutationSeed> seed);

	template<size_t N>
	void inc(std::bitset<N> & set)
	{
		for (size_t i = 0; i < N; ++i)
		{
			if (!set[i])
			{
				set.set(i);
				return;
			}
			set.reset(i);
		}
	}

	Pair getRandom(std::mt19937_64 & engine, unsigned long long last, MoveType type);

	Pair getRandomAscending(std::mt19937_64 & engine, unsigned long long last);

	void move(const Pair & pair, unsigned long long * cycle, MoveType type);

	void moveSwap(const Pair & pair, unsigned long long * cycle);

	void moveInvert(const Pair & pair, unsigned long long * cycle);

	void moveInsert(const Pair & pair, unsigned long long * cycle);

	void revertMove(const Pair & pair, unsigned long long * cycle, MoveType type);

	void revertSwap(const Pair & pair, unsigned long long * cycle);

	void revertInvert(const Pair & pair, unsigned long long * cycle);

	void revertInsert(const Pair & pair, unsigned long long * cycle);
}
