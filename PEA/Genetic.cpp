#include "pch.h"
#include "Genetic.h"
#include <algorithm>
#include <deque>
#include <unordered_set>

namespace Algorithm
{
	unsigned long long Genetic::size_1 = 0;
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
		std::get<1>(t2) = std::get<1>(t1);
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

		bool isEven = (chromosomeCount / 2) % 2;
		size_1 = graph->getSize() - 1;
		std::shared_ptr<ChromosomeQueue> tempSwap = nullptr;
		std::tuple<std::shared_ptr<ChromosomeQueue>, std::shared_ptr<ChromosomeQueue>> chromosomes =
			std::make_tuple(std::make_shared<ChromosomeQueue>(chromosomeCount), std::make_shared<ChromosomeQueue>(chromosomeCount));
		std::mt19937_64 engine(std::random_device{}());
		std::vector<unsigned long long> parentMatches(chromosomeCount / 2 - isEven);
		unsigned long long temp = 0;
		std::generate(parentMatches.begin(), parentMatches.end(), [&temp](){ return temp++; });

		for (unsigned long long i = 0; i < chromosomeCount; ++i)
		{
			graph->getInitialCycleRandom(&tempCycle);
			std::get<0>(chromosomes)->push(std::make_tuple(tempCycle, graph->getCost(tempCycle)));
			tempCycle = new unsigned long long[graph->getSize()];
			tempCycle[0] = 0;
			std::get<1>(chromosomes)->push(std::make_tuple(tempCycle, 0));
		}

		do
		{
			std::shuffle(parentMatches.begin(), parentMatches.end(), engine);
			for (unsigned long long i = 0, j = parentMatches.size(); i < parentMatches.size(); i += 2, j += 2)
			{
				auto & parent1 = (*std::get<0>(chromosomes))[parentMatches.at(i)];
				auto & parent2 = (*std::get<0>(chromosomes))[parentMatches.at(i + 1)];
				auto & parentNew1 = (*std::get<1>(chromosomes))[i];
				auto & parentNew2 = (*std::get<1>(chromosomes))[i + 1];
				swap(parent1, parentNew1);
				swap(parent2, parentNew2);
				switch (crossType)
				{
				case Algorithm::Genetic::Mapped:
					crossPartiallyMapped(graph, std::get<0>(parentNew1), std::get<0>(parentNew2), (*std::get<1>(chromosomes))[j], (*std::get<1>(chromosomes))[j + 1], engine);
					break;
				case Algorithm::Genetic::Order:
					crossOrder(graph, std::get<0>(parentNew1), std::get<0>(parentNew2), (*std::get<1>(chromosomes))[j], (*std::get<1>(chromosomes))[j + 1], engine);
					break;
				}
				mutate(graph, mutationChance, (*std::get<1>(chromosomes))[j], engine);
				mutate(graph, mutationChance, (*std::get<1>(chromosomes))[j + 1], engine);
			}
			if (isEven)
			{
				graph->getInitialCycleRandom(&tempCycle);
				std::get<0>((*std::get<1>(chromosomes))[chromosomeCount - 1]) = tempCycle;
				std::get<1>((*std::get<1>(chromosomes))[chromosomeCount - 1]) = graph->getCost(tempCycle);
			}
			std::get<1>(chromosomes)->sort();
			tempSwap = std::get<1>(chromosomes);
			std::get<1>(chromosomes) = std::get<0>(chromosomes);
			std::get<0>(chromosomes) = tempSwap;
		} while (maxTime--);/*
		for (unsigned long long i = 0; i < chromosomeCount; ++i)
			std::cout << "\n" << std::get<1>((*std::get<0>(chromosomes))[i]);*/
		delete[] tempCycle;
		tempCycle = nullptr;
		std::get<0>(chromosomes)->sort();
		return std::get<0>((*std::get<0>(chromosomes))[0]);
	}
}
