#pragma once
#include <memory>
#include <bitset>

namespace Algorithm
{
	struct PermutationSeed
	{
		unsigned int * control = nullptr;
		unsigned int size = 0;
		unsigned int index = 1; // First swap point (size - 1, size)
	};

	unsigned int * generateCycle(unsigned int vertices, bool isRandom = true);

	void showCycle(unsigned int * cycle, const unsigned int count);

	std::shared_ptr<PermutationSeed> getNewSeed(unsigned int count);

	void freeSeed(std::shared_ptr<PermutationSeed> seed);
	
	void getNextPermutation(unsigned int * cycle, std::shared_ptr<PermutationSeed> seed);

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
}