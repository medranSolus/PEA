#include "pch.h"
#include "Graph.h"
#include "DisjunctiveSet.h"
#include "PriorityQueue.h"
#include <unordered_set>
#include <string>

void Graph::init()
{
	matrix = new int*[vertices];
	name = "randomData" + std::to_string(vertices);
	for (unsigned int i = 0; i < vertices; ++i)
	{
		matrix[i] = new int[vertices];
		for (unsigned int j = 0; j < vertices; ++j)
			matrix[i][j] = randomGenerateStart + rand() % randomGenerateCount;
	}
}

void Graph::reset()
{
	if (matrix)
	{
		for (unsigned int i = 0; i < vertices; ++i)
			if (matrix[i])
				delete[] matrix[i];
		delete[] matrix;
		matrix = nullptr;
	}
}

Graph::Graph(std::ifstream & fin, const std::string & dataName) : name(dataName)
{
	fin >> vertices;
	matrix = new int*[vertices];
	for (unsigned int i = 0; i < vertices; ++i)
	{
		matrix[i] = new int[vertices];
		for (unsigned int j = 0; j < vertices; ++j)
			fin >> matrix[i][j];
	}
	for (unsigned int i = 0; i < vertices; ++i)
		for (unsigned int j = 0; j < vertices; ++j)
			if (matrix[i][j] != matrix[j][i])
			{
				symetric = false;
				return;
			}
}

int Graph::getInitialCycle(unsigned int ** cycle) const
{
	int cost = 0;
	unsigned int currentColumn = 0;
	*cycle = new unsigned int[vertices];
	(*cycle)[0] = 0;
	unsigned int i = 1;
	std::unordered_set<unsigned int> checked;
	while (checked.size() < vertices - 1)
	{
		PriorityQueue<std::tuple<unsigned int, int>> toCheck(vertices);
		for (unsigned int j = 1; j < vertices; ++j)
			if (j != currentColumn && checked.find(j) == checked.end())
				toCheck.push(std::move(std::make_tuple(j, matrix[currentColumn][j])));
		auto next = toCheck.top();
		cost += std::get<1>(next);
		currentColumn = std::get<0>(next);
		(*cycle)[i++] = currentColumn;
		checked.emplace(currentColumn);
	}
	return cost + matrix[currentColumn][0];
}

int Graph::getUpperBound() const
{
	int cost = 0;
	unsigned int currentColumn = 0;
	std::unordered_set<unsigned int> checked;
	while (checked.size() < vertices - 1)
	{
		PriorityQueue<std::tuple<unsigned int, int>> toCheck(vertices);
		for (unsigned int j = 1; j < vertices; ++j)
			if (j != currentColumn && checked.find(j) == checked.end())
				toCheck.push(std::move(std::make_tuple(j, matrix[j][currentColumn])));
		auto next = toCheck.top();
		cost += std::get<1>(next);
		currentColumn = std::get<0>(next);
		checked.emplace(currentColumn);
	}
	return cost + matrix[0][currentColumn];
}

int Graph::getMST(const std::vector<unsigned int> & path) const
{
	int cost = 0;
	if (symetric)
	{
		std::unordered_set<unsigned int> excluded(path.begin(), path.end());
		DisjunctiveSet set(vertices);
		PriorityQueue<std::tuple<unsigned int, int, unsigned int>> list;
		for (unsigned int i = 0; i < vertices; ++i)
		{
			if (excluded.find(i) == excluded.end())
			{
				set.makeSet(i);
				for (unsigned int j = 0; j < vertices; ++j)
					if (i != j && excluded.find(j) == excluded.end())
						list.push(std::make_tuple(i, matrix[i][j], j));
			}
		}
		for (unsigned int i = 1; list.size() && i < vertices;)
		{
			std::tuple<unsigned int, int, unsigned int> temp = list.top();
			if (set.findSet(std::get<0>(temp)) != set.findSet(std::get<2>(temp)))
			{
				cost += std::get<1>(temp);
				set.unionSets(std::get<0>(temp), std::get<2>(temp));
				++i;
			}
		}
	}
	return cost;
}

int Graph::getShortests(unsigned int current) const
{
	int min = INT_MAX;
	for (unsigned int i = 0; i < vertices; ++i)
		if (matrix[current][i] < min)
			min = matrix[current][i];
	return ++min;
}

void Graph::resetRandom(unsigned int vertexCount)
{
	reset();
	vertices = vertexCount;
	init();
}

void Graph::show() const
{
	std::cout << "\nName: " << name << "\n\t|";
	for (unsigned int i = 0; i < vertices; ++i)
		std::cout << i << "\t";
	std::cout << "\n--------X";
	for (unsigned int i = 0; i < vertices; ++i)
		std::cout << "--------";
	for (unsigned int i = 0; i < vertices; ++i)
	{
		std::cout << std::endl << i << "\t|";
		for (unsigned int j = 0; j < vertices; ++j)
			std::cout << matrix[i][j] << '\t';
	}
	std::cout << std::endl;
}

int Graph::getCost(unsigned int * cycle) const
{
	if (cycle)
	{
		int cost = matrix[cycle[vertices - 1]][cycle[0]];
		for (unsigned int i = 1; i < vertices; ++i)
			cost += matrix[cycle[i - 1]][cycle[i]]; // Check for diagonal value suppressed as no possible cycle can contain 2 same values next to themselves
		return cost;
	}
	return -1;
}
