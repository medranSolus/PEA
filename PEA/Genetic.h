#pragma once
#include "Algorithm.h"
#include "Graph.h"
#include "PriorityQueue.h"
#include <random>

namespace Algorithm
{
	class Genetic
	{
		typedef std::tuple<unsigned long long*, long long> Chromosome;
		typedef PriorityQueue<Chromosome> ChromosomeQueue;

		static unsigned long long * tempCycle;
		static unsigned long long size_1;

		static void mutate(std::shared_ptr<Graph> graph, float mutationChance, Chromosome & t, std::mt19937_64 & eng);
		static void swap(Chromosome & t1, Chromosome & t2);

		static void crossPartiallyMapped(std::shared_ptr<Graph> graph, unsigned long long * parent1, unsigned long long * parent2,
			Chromosome & child1, Chromosome & child2, std::mt19937_64 & eng);
		static void crossOrder(std::shared_ptr<Graph> graph, unsigned long long * parent1, unsigned long long * parent2,
			Chromosome & child1, Chromosome & child2, std::mt19937_64 & eng);

	public:
		Genetic() = delete;

		enum CrossType { Mapped, Order };

		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph, unsigned long long chromosomeCount, unsigned long long maxTime, float mutationChance, CrossType crossType);
	};
}
