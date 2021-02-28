#include "pch.h"
#include "Genetic.h"
#include <algorithm>
#include <deque>
#include <unordered_set>

namespace Algorithm
{
	unsigned long long Genetic::size_1 = 0;
	unsigned long long Genetic::temp = 0;
	unsigned long long * Genetic::tempCycle = nullptr;

	void Genetic::mutate(std::shared_ptr<Graph> graph, float mutationChance, Chromosome & t, std::mt19937_64 & eng)
	{
		while (std::uniform_real_distribution<float>()(eng) < mutationChance)
		{
			const MoveType type = static_cast<MoveType>(std::uniform_int_distribution<short>(0, 2)(eng));
			const Pair pair = getRandom(eng, size_1, type);
			const long long dCost = graph->getCost(std::get<0>(t), pair, type);
			move(pair, std::get<0>(t), type);
			if (dCost == -1)
				std::get<1>(t) = graph->getCost(std::get<0>(t));
			else
				std::get<1>(t) += graph->getCost(std::get<0>(t), pair, type, false) - dCost;
		}
	}

	void Genetic::swap(Chromosome & t1, Chromosome & t2)
	{
		tempCycle = std::get<0>(t1);
		std::get<0>(t1) = std::get<0>(t2);
		std::get<0>(t2) = tempCycle;
		tempCycle = nullptr;
		temp = std::get<1>(t2);
		std::get<1>(t2) = std::get<1>(t1);
		std::get<1>(t1) = temp;
	}

	void Genetic::crossPartiallyMapped(std::shared_ptr<Graph> graph, unsigned long long * parent1, unsigned long long * parent2,
		Chromosome & child1, Chromosome & child2, std::mt19937_64 & eng)
	{
		const Pair range = getRandomAscending(eng, size_1);
		unsigned long long x = 1;
		std::vector<unsigned long long> temp(size_1);
		std::generate(temp.begin(), temp.end(), [&x]() {return x++; });
		std::unordered_set<unsigned long long> missing1(temp.begin(), temp.end());
		std::unordered_set<unsigned long long> missing2(missing1);
		for (unsigned long long i = range.first; i <= range.second; ++i)
		{
			std::get<0>(child1)[i] = parent2[i];
			missing1.erase(parent2[i]);
			std::get<0>(child2)[i] = parent1[i];
			missing2.erase(parent1[i]);
		}
		for (unsigned long long i = 1; i < range.first; ++i)
		{
			if (missing1.find(parent1[i]) != missing1.end())
			{
				std::get<0>(child1)[i] = parent1[i];
				missing1.erase(parent1[i]);
			}
			else
				std::get<0>(child1)[i] = 0;
			if (missing2.find(parent2[i]) != missing2.end())
			{
				std::get<0>(child2)[i] = parent2[i];
				missing2.erase(parent2[i]);
			}
			else
				std::get<0>(child2)[i] = 0;
		}
		for (unsigned long long i = range.second + 1; i <= size_1; ++i)
		{
			if (missing1.find(parent1[i]) != missing1.end())
			{
				std::get<0>(child1)[i] = parent1[i];
				missing1.erase(parent1[i]);
			}
			else
				std::get<0>(child1)[i] = 0;
			if (missing2.find(parent2[i]) != missing2.end())
			{
				std::get<0>(child2)[i] = parent2[i];
				missing2.erase(parent2[i]);
			}
			else
				std::get<0>(child2)[i] = 0;
		}
		for (unsigned long long i = 1; i < range.first; ++i)
		{
			if (std::get<0>(child1)[i] == 0)
			{
				std::get<0>(child1)[i] = *missing1.begin();
				missing1.erase(std::get<0>(child1)[i]);
			}
			if (std::get<0>(child2)[i] == 0)
			{
				std::get<0>(child2)[i] = *missing2.begin();
				missing2.erase(std::get<0>(child2)[i]);
			}
		}
		for (unsigned long long i = range.second + 1; i <= size_1; ++i)
		{
			if (std::get<0>(child1)[i] == 0)
			{
				std::get<0>(child1)[i] = *missing1.begin();
				missing1.erase(std::get<0>(child1)[i]);
			}
			if (std::get<0>(child2)[i] == 0)
			{
				std::get<0>(child2)[i] = *missing2.begin();
				missing2.erase(std::get<0>(child2)[i]);
			}
		}
		std::get<1>(child1) = graph->getCost(std::get<0>(child1));
		std::get<1>(child2) = graph->getCost(std::get<0>(child2));
	}

	void Genetic::crossOrder(std::shared_ptr<Graph> graph, unsigned long long * parent1, unsigned long long * parent2, Chromosome & child1, Chromosome & child2, std::mt19937_64 & eng)
	{
		const Pair range = getRandomAscending(eng, size_1);
		std::unordered_set<unsigned long long> used1;
		std::unordered_set<unsigned long long> used2;
		for (unsigned long long i = range.first; i <= range.second; ++i)
		{
			std::get<0>(child1)[i] = parent2[i];
			used1.emplace(parent2[i]);
			std::get<0>(child2)[i] = parent1[i];
			used2.emplace(parent1[i]);
		}
		for (unsigned long long i = range.second + 1; i <= size_1; ++i)
		{
			if (used1.find(parent2[i]) == used1.end())
				std::get<0>(child1)[i] = parent2[i];
			if (used2.find(parent1[i]) == used2.end())
				std::get<0>(child2)[i] = parent1[i];
		}
		for (unsigned long long i = 1, j1 = 1, j2 = 1; i <= range.second; ++i)
		{
			if (used1.find(parent2[i]) == used1.end())
				std::get<0>(child1)[j1++] = parent2[i];
			if (used2.find(parent1[i]) == used2.end())
				std::get<0>(child2)[j2++] = parent1[i];
		}
		std::get<1>(child1) = graph->getCost(std::get<0>(child1));
		std::get<1>(child2) = graph->getCost(std::get<0>(child2));
	}

	unsigned long long * Genetic::getMinimalCycle(std::shared_ptr<Graph> graph, unsigned long long chromosomeCount, unsigned long long maxTime, float mutationChance, CrossType crossType)
	{
		// https://www.hindawi.com/journals/cin/2017/7430125/

		unsigned long long parentCount = chromosomeCount / 2 + (chromosomeCount / 2) % 2;
		temp = chromosomeCount + parentCount;
		size_1 = graph->getSize() - 1;

		ChromosomeQueue chromosomes(temp);
		for (unsigned long long i = 0; i < temp; ++i)
		{
			chromosomes.push(std::make_tuple(tempCycle, graph->getInitialCycleRandom(&tempCycle)));
			tempCycle = nullptr;
		}

		std::mt19937_64 engine(std::random_device{}());
		std::vector<bool> isParent(chromosomeCount, false);
		unsigned long long parentId1, parentId2;
		do
		{
			for (unsigned long long i = 0; i < parentCount; i += 2)
			{
				do
				{
					parentId1 = std::uniform_int_distribution<unsigned long long>(0, static_cast<unsigned long long>(chromosomeCount - 1))(engine);
				} while (isParent.at(parentId1));
				isParent.at(parentId1) = true;
				do
				{
					parentId2 = std::uniform_int_distribution<unsigned long long>(0, static_cast<unsigned long long>(chromosomeCount - 1))(engine);
				} while (isParent.at(parentId2));
				isParent.at(parentId2) = true;
				Chromosome & parent1 = chromosomes[parentId1];
				Chromosome & parent2 = chromosomes[parentId2];
				switch (crossType)
				{
				case Algorithm::Genetic::Mapped:
					crossPartiallyMapped(graph, std::get<0>(parent1), std::get<0>(parent2), chromosomes[chromosomeCount + i], chromosomes[chromosomeCount + i + 1], engine);
					break;
				case Algorithm::Genetic::Order:
					crossOrder(graph, std::get<0>(parent1), std::get<0>(parent2), chromosomes[chromosomeCount + i], chromosomes[chromosomeCount + i + 1], engine);
					break;
				}
				mutate(graph, mutationChance, chromosomes[chromosomeCount + i], engine);
				mutate(graph, mutationChance, chromosomes[chromosomeCount + i + 1], engine);
			}
			chromosomes.sort();
			std::fill(isParent.begin(), isParent.end(), false);
		} while (maxTime--);
		chromosomes.sort();
		return std::get<0>(chromosomes[0]);
	}
}
