#include "pch.h"
#include "FileService.h"
#include "Dynamic.h"
#include "BruteForce.h"
#include "BranchAndBound.h"
#include "TabuSearch.h"
#include <ctime>
using namespace std;

void testBF();
void testDP();
void testBnB();

constexpr size_t START_SIZE = 10;
constexpr const char * TEST_FILE = "Tests\\SMALL\\data";

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));
	shared_ptr<Graph> graph = FileService::readGraph(TEST_FILE + to_string(START_SIZE) + ".txt");
	graph->show();
	unsigned int * cycle = Algorithm::TabuSearch::getMinimalCycle(graph, 100000);
	Algorithm::showCycle(cycle, graph->getSize());
	graph->showCost(cycle);
	return 0;
}

void testBF()
{
	constexpr size_t COUNT = 4;
	constexpr const char * OUT_FILE = "BF.txt";

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	vector<vector<double>> time(COUNT, std::move(vector<double>(4, 0.0)));
	unsigned int * cycle = nullptr;
	for (size_t size = START_SIZE; size < START_SIZE + COUNT; ++size)
	{
		shared_ptr<Graph> graph = FileService::readGraph(TEST_FILE + to_string(size) + ".txt");
		for (size_t i = 0; i < 50; ++i)
		{
			start = getTime();
			cycle = Algorithm::BruteForce::getMinimalCycleSlow(graph);
			end = getTime();
			time[size - START_SIZE][0] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BF " << size << ".0[" << i << ']';
			delete[] cycle;
			start = getTime();
			cycle = Algorithm::BruteForce::getMinimalCycleRecur(graph);
			end = getTime();
			time[size - START_SIZE][1] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BF " << size << ".1[" << i << ']';
			delete[] cycle;
			start = getTime();
			cycle = Algorithm::BruteForce::getMinimalCycle(graph);
			end = getTime();
			time[size - START_SIZE][2] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BF " << size << ".2[" << i << ']';
			delete[] cycle;
			start = getTime();
			cycle = Algorithm::BruteForce::getMinimalCycleOpt(graph);
			end = getTime();
			time[size - START_SIZE][3] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BF " << size << ".3[" << i << ']';
			delete[] cycle;
		}
		time[size - START_SIZE][0] *= 20.0;
		time[size - START_SIZE][1] *= 20.0;
		time[size - START_SIZE][2] *= 20.0;
		time[size - START_SIZE][3] *= 20.0;
	}
	FileService::saveData(time, { "Tree", "Rec", "Perm", "Opt" }, OUT_FILE);
}

void testDP()
{
	constexpr size_t COUNT = 9;
	constexpr const char * OUT_FILE = "DP.txt";

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	vector<vector<double>> time(COUNT, std::move(vector<double>(1, 0.0)));
	unsigned int * cycle = nullptr;
	for (size_t size = START_SIZE; size < START_SIZE + COUNT; ++size)
	{
		shared_ptr<Graph> graph = FileService::readGraph(TEST_FILE + to_string(size) + ".txt");
		for (size_t i = 0; i < 50; ++i)
		{
			start = getTime();
			cycle = Algorithm::Dynamic::getMinimalCycle(graph);
			end = getTime();
			time[size - START_SIZE][0] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "DP " << size << "[" << i << ']';
			delete[] cycle;
		}
		time[size - START_SIZE][0] *= 20.0;
	}
	FileService::saveData(time, { "DP" }, OUT_FILE);
}

void testBnB()
{
	constexpr size_t COUNT = 8;
	constexpr const char * OUT_FILE = "BnB.txt";

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	vector<vector<double>> time(COUNT, std::move(vector<double>(3, 0.0)));
	unsigned int * cycle = nullptr;
	for (size_t size = START_SIZE; size < START_SIZE + COUNT; ++size)
	{
		shared_ptr<Graph> graph = FileService::readGraph(TEST_FILE + to_string(size) + ".txt");
		for (size_t i = 0; i < 50; ++i)
		{
			start = getTime();
			cycle = Algorithm::BranchAndBound::getMinimalCycleBS(graph);
			end = getTime();
			time[size - START_SIZE][0] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BnB " << size << ".0[" << i << ']';
			delete[] cycle;
			start = getTime();
			cycle = Algorithm::BranchAndBound::getMinimalCycle(graph, false);
			end = getTime();
			time[size - START_SIZE][1] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BnB " << size << ".1[" << i << ']';
			delete[] cycle;
			start = getTime();
			cycle = Algorithm::BranchAndBound::getMinimalCycle(graph);
			end = getTime();
			time[size - START_SIZE][2] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
			cout << endl << "BnB " << size << ".2[" << i << ']';
			delete[] cycle;
		}
		time[size - START_SIZE][0] *= 20.0;
		time[size - START_SIZE][1] *= 20.0;
		time[size - START_SIZE][2] *= 20.0;
	}
	FileService::saveData(time, { "BS", "BFS", "DFS" }, OUT_FILE);
}