#pragma once
#include <iostream>
#include <fstream>
#include <vector>

class Graph
{
	std::string name;
	unsigned int vertices = 0;
	int ** matrix = nullptr;
	unsigned int randomGenerateCount = 10;
	unsigned int randomGenerateStart = 0;
	bool symetric = true;

	void init();
	void reset();

public:
	Graph(std::ifstream & fin, const std::string & dataName = "none");
	Graph(unsigned int vertexCount) : vertices(vertexCount) { init(); }
	~Graph() { reset(); }

	inline unsigned int getRandomGenerateStart() const { return randomGenerateStart; }
	inline unsigned int getRandomGenerateCount() const { return randomGenerateCount; }
	constexpr inline unsigned int getSize() const { return vertices; }
	constexpr inline int getCost(unsigned int start, unsigned int end) const { return matrix[start][end]; }

	inline void setRandomGenerateStart(unsigned int start) { randomGenerateStart = start; }
	inline void setRandomGenerateCount(unsigned int count) { randomGenerateCount = count; }
	inline void showCost(unsigned int * cycle) const { std::cout << "Cost: " << getCost(cycle) << std::endl; }

	int getInitialCycle(unsigned int ** cycle) const;
	int getUpperBound() const;
	int getMST(const std::vector<unsigned int> & path) const;
	int getShortests(unsigned int current) const;
	void resetRandom(unsigned int vertexCount);

	void show() const;
	int getCost(unsigned int * cycle) const;
};
