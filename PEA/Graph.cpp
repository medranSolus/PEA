#include "pch.h"
#include "Graph.h"
#include "DisjunctiveSet.h"
#include "PriorityQueue.h"
#include <unordered_set>
#include <string>
#include <random>

void Graph::init()
{
	matrix = new long long*[vertices];
	name = "randomData" + std::to_string(vertices);
	for (unsigned long long i = 0; i < vertices; ++i)
	{
		matrix[i] = new long long[vertices];
		for (unsigned long long j = 0; j < vertices; ++j)
			matrix[i][j] = randomGenerateStart + rand() % randomGenerateCount;
	}
}

void Graph::reset()
{
	if (matrix)
	{
		for (unsigned long long i = 0; i < vertices; ++i)
			if (matrix[i])
				delete[] matrix[i];
		delete[] matrix;
		matrix = nullptr;
	}
}

Graph::Graph(std::ifstream & fin, const std::string & dataName) : name(dataName)
{
	fin >> vertices;
	matrix = new long long*[vertices];
	for (unsigned long long i = 0; i < vertices; ++i)
	{
		matrix[i] = new long long[vertices];
		for (unsigned long long j = 0; j < vertices; ++j)
			fin >> matrix[i][j];
	}
	for (unsigned long long i = 0; i < vertices; ++i)
		for (unsigned long long j = 0; j < vertices; ++j)
			if (matrix[i][j] != matrix[j][i])
			{
				symetric = false;
				return;
			}
}

long long Graph::getInitialCycleNN(unsigned long long ** cycle) const
{
	long long cost = 0;
	unsigned long long currentColumn = 0;
	*cycle = new unsigned long long[vertices];
	(*cycle)[0] = 0;
	unsigned long long i = 1;
	std::unordered_set<unsigned long long> checked;
	do
	{
		PriorityQueue<std::tuple<unsigned long long, long long>> toCheck(vertices);
		for (unsigned long long j = 1; j < vertices; ++j)
			if (j != currentColumn && checked.find(j) == checked.end())
				toCheck.push(std::move(std::make_tuple(j, matrix[currentColumn][j])));
		auto next = toCheck.top();
		cost += std::get<1>(next);
		currentColumn = std::get<0>(next);
		(*cycle)[i++] = currentColumn;
		checked.emplace(currentColumn);
	} while (i < vertices);
	return cost + matrix[currentColumn][0];
}

long long Graph::getInitialCycleRandomNN(unsigned long long ** cycle) const
{
	std::mt19937_64 engine(std::random_device{}());
	const unsigned long long vertices_1 = vertices - 1;
	const unsigned long long randomCount = std::uniform_int_distribution<unsigned long long>(1, vertices_1)(engine);
	long long cost = 0;
	*cycle = new unsigned long long[vertices];
	(*cycle)[0] = 0;
	unsigned long long i = 1, v;
	std::unordered_set<unsigned long long> checked;
	do
	{
		v = std::uniform_int_distribution<unsigned long long>(1, vertices_1)(engine);
		if (checked.find(v) == checked.end())
		{
			checked.emplace(v);
			cost += matrix[(*cycle)[i - 1]][v];
			(*cycle)[i++] = v;
		}
	} while (i < randomCount);
	while (i < vertices)
	{
		PriorityQueue<std::tuple<unsigned long long, long long>> toCheck(vertices);
		for (unsigned long long j = 1; j < vertices; ++j)
			if (j != v && checked.find(j) == checked.end())
				toCheck.push(std::move(std::make_tuple(j, matrix[v][j])));
		auto next = toCheck.top();
		cost += std::get<1>(next);
		v = std::get<0>(next);
		(*cycle)[i++] = v;
		checked.emplace(v);
	}
	return cost + matrix[v][0];
}

long long Graph::getInitialCycleRandom(unsigned long long ** cycle, bool isNullptr) const
{
	const unsigned long long vertices_1 = vertices - 1;
	long long cost = 0;
	if (isNullptr)
	{
		*cycle = new unsigned long long[vertices];
		(*cycle)[0] = 0;
	}
	unsigned long long i = 1, v;
	std::unordered_set<unsigned long long> checked;
	std::mt19937_64 engine(std::random_device{}());
	do
	{
		v = std::uniform_int_distribution<unsigned long long>(1, vertices_1)(engine);
		if (checked.find(v) == checked.end())
		{
			checked.emplace(v);
			cost += matrix[(*cycle)[i - 1]][v];
			(*cycle)[i++] = v;
		}
	} while (i < vertices);
	return cost + matrix[v][0];
}

long long Graph::getUpperBound() const
{
	long long cost = 0;
	unsigned long long currentColumn = 0;
	std::unordered_set<unsigned long long> checked;
	while (checked.size() < vertices - 1)
	{
		PriorityQueue<std::tuple<unsigned long long, long long>> toCheck(vertices);
		for (unsigned long long j = 1; j < vertices; ++j)
			if (j != currentColumn && checked.find(j) == checked.end())
				toCheck.push(std::move(std::make_tuple(j, matrix[j][currentColumn])));
		auto next = toCheck.top();
		cost += std::get<1>(next);
		currentColumn = std::get<0>(next);
		checked.emplace(currentColumn);
	}
	return cost + matrix[0][currentColumn];
}

long long Graph::getMST(const std::vector<unsigned long long> & path) const
{
	long long cost = 0;
	if (symetric)
	{
		std::unordered_set<unsigned long long> excluded(path.begin(), path.end());
		DisjunctiveSet set(vertices);
		PriorityQueue<std::tuple<unsigned long long, long long, unsigned long long>> list;
		for (unsigned long long i = 0; i < vertices; ++i)
		{
			if (excluded.find(i) == excluded.end())
			{
				set.makeSet(i);
				for (unsigned long long j = 0; j < vertices; ++j)
					if (i != j && excluded.find(j) == excluded.end())
						list.push(std::make_tuple(i, matrix[i][j], j));
			}
		}
		for (unsigned long long i = 1; list.size() && i < vertices;)
		{
			std::tuple<unsigned long long, long long, unsigned long long> temp = list.top();
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

long long Graph::getShortests(unsigned long long current) const
{
	long long min = LLONG_MAX;
	for (unsigned long long i = 0; i < vertices; ++i)
		if (matrix[current][i] < min)
			min = matrix[current][i];
	return ++min;
}

long long Graph::getCost(unsigned long long * cycle, const Algorithm::Pair & pair, Algorithm::MoveType moveType, bool firstCall) const noexcept
{
	long long cost = 0;
	switch (moveType)
	{
	case Algorithm::MoveType::Swap:
	{
		if (pair.second - 1 == pair.first)
			return matrix[cycle[pair.second - 2]][cycle[pair.second - 1]] +
				matrix[cycle[pair.second - 1]][cycle[pair.second]] +
				matrix[cycle[pair.second]][cycle[(pair.second + 1) % vertices]];
		if (pair.first - 1 == pair.second)
			return matrix[cycle[pair.first - 2]][cycle[pair.first - 1]] +
				matrix[cycle[pair.first - 1]][cycle[pair.first]] +
				matrix[cycle[pair.first]][cycle[(pair.first + 1) % vertices]];
		return matrix[cycle[pair.first - 1]][cycle[pair.first]] +
			matrix[cycle[pair.first]][cycle[(pair.first + 1) % vertices]] +
			matrix[cycle[pair.second - 1]][cycle[pair.second]] +
			matrix[cycle[pair.second]][cycle[(pair.second + 1) % vertices]];
	}
	case Algorithm::MoveType::Insert:
	{
		cost = matrix[cycle[pair.second - 1]][cycle[pair.second]];
		if (pair.first < pair.second)
		{
			if (firstCall)
				cost += matrix[cycle[pair.first]][cycle[pair.first + 1]];
			else
				cost += matrix[cycle[pair.second - 2]][cycle[pair.second - 1]];
			return cost + matrix[cycle[pair.first - 1]][cycle[pair.first]];
		}
		if (firstCall)
			cost += matrix[cycle[pair.first - 1]][cycle[pair.first]];
		else
			cost += matrix[cycle[pair.second]][cycle[pair.second + 1]];
		return cost + matrix[cycle[pair.first]][cycle[(pair.first + 1) % vertices]];
	}
	default:
		return -1;
	}
}

void Graph::resetRandom(unsigned long long vertexCount)
{
	reset();
	vertices = vertexCount;
	init();
}

void Graph::show() const
{
	std::cout << "\nName: " << name << "\n\t|";
	for (unsigned long long i = 0; i < vertices; ++i)
		std::cout << i << "\t";
	std::cout << "\n--------X";
	for (unsigned long long i = 0; i < vertices; ++i)
		std::cout << "--------";
	for (unsigned long long i = 0; i < vertices; ++i)
	{
		std::cout << std::endl << i << "\t|";
		for (unsigned long long j = 0; j < vertices; ++j)
			std::cout << matrix[i][j] << '\t';
	}
	std::cout << std::endl;
}

long long Graph::getCost(unsigned long long * cycle) const
{
	if (cycle)
	{
		long long cost = matrix[cycle[vertices - 1]][cycle[0]];
		for (unsigned long long i = 1; i < vertices; ++i)
			cost += matrix[cycle[i - 1]][cycle[i]]; // Check for diagonal value suppressed as no possible cycle can contain 2 same values next to themselves
		return cost;
	}
	return -1;
}
