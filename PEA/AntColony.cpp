#include "pch.h"
#include "AntColony.h"
#include "PriorityQueue.h"
#include <algorithm>

namespace Algorithm
{
	unsigned long long * AntColony::getMinimalCycle(std::shared_ptr<Graph> graph, unsigned long long alpha, unsigned long long beta,
		unsigned long long pheromoneFactor, unsigned long long pheromoneLevel)
	{
		// https://www.sciencedirect.com/science/article/pii/S1002007108002736

		unsigned long long size = graph->getSize();
		std::vector<std::vector<unsigned long long>> pheromones(size, std::vector<unsigned long long>(size, 1));
		PriorityQueue<std::tuple<std::vector<unsigned long long>, long long, std::vector<unsigned long long>>> ants(size);
		std::mt19937_64 engine(std::random_device{}());
		for (unsigned long long i = 0; i < size; ++i)
		{
			std::vector<unsigned long long> notVisited;
			for (unsigned long long j = 0; j < size; ++j)
				if (j != i)
					notVisited.emplace_back(j);
			std::shuffle(notVisited.begin(), notVisited.end(), engine);
			std::vector<unsigned long long> visited(1, i);
			visited.reserve(size);
			ants.push(std::make_tuple(std::move(visited), 0, std::move(notVisited)));
		}
		for (unsigned long long i = 1; i < size; ++i)
		{
			for (unsigned long long j = 0; j < size; ++j)
			{
				double sumNotVisited = 0.0;
				unsigned long long currentCity = std::get<0>(ants[j]).back();
				std::vector<unsigned long long> & noVisited = std::get<2>(ants[j]);
				for (auto & city : noVisited)
					sumNotVisited += pow(pheromones.at(currentCity).at(city), alpha) / static_cast<double>(pow(graph->getCost(currentCity, city), beta));
				unsigned long long nextCity;
				long long currentCost;
				size_t index;
				double chance;
				do
				{
					index = std::uniform_int_distribution<size_t>(0, noVisited.size() - 1)(engine);
					nextCity = noVisited.at(index);
					currentCost = graph->getCost(currentCity, nextCity);
					chance = pow(pheromones.at(currentCity).at(nextCity), alpha) / (static_cast<double>(pow(currentCost, beta)) * sumNotVisited);
				} while (std::uniform_real_distribution<double>()(engine) > chance);
				std::get<0>(ants[j]).emplace_back(nextCity);
				std::get<1>(ants[j]) += currentCost;
				noVisited.erase(noVisited.begin() + index);
				if (!currentCost)
					currentCost = 1;
				pheromones.at(currentCity).at(nextCity) += pheromoneLevel * pheromoneFactor / currentCost;
			}
			for (size_t j = 0; j < size; ++j)
				for (size_t k = 0; k < size; ++k)
				{
					pheromones.at(j).at(k) /= pheromoneFactor;
					if (pheromones.at(j).at(k) < 1)
						pheromones.at(j).at(k) = 1;
				}
		}
		for (unsigned long long i = 0; i < size; ++i)
			std::get<1>(ants[i]) += graph->getCost(std::get<0>(ants[i]).back(), std::get<0>(ants[i]).front());
		ants.sort();
		unsigned long long * bestCyle = new unsigned long long[size];
		for (unsigned long long i = 0; i < size; ++i)
			bestCyle[i] = std::get<0>(ants[0]).at(i);
		return bestCyle;
	}
}
