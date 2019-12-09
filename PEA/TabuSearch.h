#pragma once
#include "Algorithm.h"
#include "Graph.h"
#include <random>

namespace Algorithm
{
	class TabuSearch
	{
		typedef std::vector<unsigned long long> TabuRow;
		typedef std::vector<TabuRow> TabuMatrix;

		TabuSearch() {}

		static void decrementTabu(TabuMatrix & tabuList, const TabuMatrix & frequency);

		static void setTabu(TabuMatrix & tabuList, TabuMatrix & frequency, unsigned long long tabuTime, MoveType moveType, const Pair & pair);

		static void makeMove(std::shared_ptr<Graph> graph, std::mt19937_64 & engine, MoveType moveType,
			TabuMatrix & tabuList, TabuMatrix & frequency, unsigned long long * cycle, long long * cost,
			long long bestCost, unsigned long long tabuTime, unsigned long long tabuRevokeTime);

		static void makeMoveRandom(std::shared_ptr<Graph> graph, std::mt19937_64 & engine, MoveType moveType,
			TabuMatrix & tabuList, TabuMatrix & frequency, unsigned long long * cycle, long long * cost,
			long long bestCost, unsigned long long tabuTime, unsigned long long tabuRevokeTime);

	public:
		enum InitialType { NN, Random, RandomNN };

		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph, InitialType initType, MoveType moveType, bool random,
			unsigned long long time, unsigned long long tabuTime, unsigned long long maxStagnationTime, unsigned long long tabuRevokeTime);
	};
}
