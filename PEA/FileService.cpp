#include "pch.h"
#include "FileService.h"
#include <string>
#include <fstream>
#include <iomanip>

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
	fout << titles.at(0);
	for (size_t i = 1; i < titles.size(); ++i)
		fout << " & " << titles.at(i);
	for (const auto & instance : values)
	{
		fout << std::endl << instance.at(0);
		for (size_t i = 1; i < instance.size(); ++i)
		{
			fout << " & " << std::fixed << std::setprecision(2) << instance.at(i);
			if (i > 3)
				fout << '%';
		}
	}
	fout.close();
}
