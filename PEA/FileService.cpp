#include "pch.h"
#include "FileService.h"
#include <string>
#include <fstream>

std::shared_ptr<Graph> FileService::readGraph(const std::string & file)
{
	std::ifstream fin(file);
	if (!fin.good())
		return nullptr;
	std::string dataName = "";
	std::getline(fin, dataName);
	return std::make_shared<Graph>(fin, dataName);
}

void FileService::saveData(const std::vector<std::vector<double>> & values, const std::vector<std::string> & titles, const std::string & name)
{
	std::ofstream fout(name);
	if (!fout.good())
	{
		std::cerr << "\n[ERROR] " << name;
		return;
	}
	for (const auto & title : titles)
		fout << title << '\t';
	for (const auto & instance : values)
	{
		fout << std::endl;
		for (auto value : instance)
			fout << value << '\t';
	}
	fout.close();
}
