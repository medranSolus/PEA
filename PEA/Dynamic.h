#pragma once
#include "Algorithm.h"
#include "Graph.h"

namespace Algorithm
{
	class Dynamic
	{
		static constexpr size_t maxCities = (sizeof(size_t) << 3) + 1;

		Dynamic() {}

	public:
		static unsigned int * getMinimalCycle(std::shared_ptr<Graph> graph);
	};
}
