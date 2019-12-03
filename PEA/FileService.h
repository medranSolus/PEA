#pragma once
#include "Graph.h"

class FileService
{
	FileService() {}

public:
	static std::shared_ptr<Graph> readGraph(const std::string & file);

	static void saveData(const std::vector<std::vector<double>> & values, const std::vector<std::string> & titles, const std::string & name);
};