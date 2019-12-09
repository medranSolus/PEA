#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "Algorithm.h"

class Graph
{
	std::string name;
	unsigned long long vertices = 0;
	long long ** matrix = nullptr;
	unsigned long long randomGenerateCount = 10;
	unsigned long long randomGenerateStart = 0;
	bool symetric = true;

	void init();
	void reset();

public:
	Graph(std::ifstream & fin, const std::string & dataName = "none");
	Graph(unsigned long long vertexCount) : vertices(vertexCount) { init(); }
	~Graph() { reset(); }

	constexpr unsigned long long getRandomGenerateStart() const noexcept { return randomGenerateStart; }
	constexpr unsigned long long getRandomGenerateCount() const noexcept { return randomGenerateCount; }
	constexpr unsigned long long getSize() const noexcept { return vertices; }
	constexpr long long getCost(unsigned long long start, unsigned long long end) const noexcept { return matrix[start][end]; }

	constexpr void setRandomGenerateStart(unsigned long long start) noexcept { randomGenerateStart = start; }
	constexpr void setRandomGenerateCount(unsigned long long count) noexcept { randomGenerateCount = count; }
	inline void showCost(unsigned long long * cycle) const noexcept { std::cout << "Cost: " << getCost(cycle) << std::endl; }

	long long getInitialCycleNN(unsigned long long ** cycle) const;
	long long getInitialCycleRandomNN(unsigned long long ** cycle) const;
	long long getInitialCycleRandom(unsigned long long ** cycle, bool isNullptr = true) const;

	long long getUpperBound() const;
	long long getMST(const std::vector<unsigned long long> & path) const;
	long long getShortests(unsigned long long current) const;
	long long getCost(unsigned long long * cycle, const Algorithm::Pair & pair, Algorithm::MoveType moveType, bool firstCall = true) const noexcept;
	void resetRandom(unsigned long long vertexCount);

	void show() const;
	long long getCost(unsigned long long * cycle) const;
};
