#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class Dynamic
	{
		static constexpr size_t maxCities = (sizeof(size_t) << 3) + 1;

	public:
		Dynamic() = delete;

		static unsigned long long * getMinimalCycle(std::shared_ptr<Graph> graph);
	};
}
