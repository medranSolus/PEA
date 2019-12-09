#include "pch.h"
#include "FileService.h"
#include "Dynamic.h"
#include "BruteForce.h"
#include "BranchAndBound.h"
#include "TabuSearch.h"
#include "SimulatedAnnealing.h"
#include <ctime>
#include <thread>
using namespace std;

void testBF();
void testDP();
void testBnB();

constexpr size_t START_SIZE = 17;
constexpr const char * TEST_FILE = "Tests\\SMALL\\data";

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));
	constexpr unsigned long long time = 2000000;
	constexpr unsigned long long tabuTime = 10;
	constexpr unsigned long long maxStagnationTime = 50;
	constexpr unsigned long long tabuRevokeTime = 40;
	shared_ptr<Graph> graph = FileService::readGraph(TEST_FILE + to_string(START_SIZE) + ".txt");
	//graph->show();
	for (size_t i = 0; i < 5; ++i)
	{
		unsigned long long * cycle = Algorithm::TabuSearch::getMinimalCycle(graph, Algorithm::TabuSearch::InitialType::RandomNN,
			Algorithm::MoveType::Swap, false, time, tabuTime, maxStagnationTime, tabuRevokeTime);
		//unsigned long long * cycle = Algorithm::SimulatedAnnealing::getMinimalCycle(graph, Algorithm::MoveType::Insert, 999999.9f, 0.602f, 9.0f, 1000000);
		graph->showCost(cycle);
		delete[] cycle;
	}
	return 0;

	// SA porównaæ ze zwyk³ym T*=tFactor;

	// SA:
	//Fuc: 0.669 Min: 9 Cost: 2391.1
	//Fuc: 0.475998 Min: 9 Cost: 2398.5
	//Fuc: 0.488998 Min : 9 Cost : 2398.2
	//Fuc: 0.496998 Min : 9 Cost : 2395.7
	//Fuc: 0.602997 Min: 9 Cost: 2384
}

void testBF()
{
	constexpr size_t COUNT = 4;
	constexpr const char * OUT_FILE = "BF.txt";

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	vector<vector<double>> time(COUNT, std::move(vector<double>(4, 0.0)));
	unsigned long long * cycle = nullptr;
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
	unsigned long long * cycle = nullptr;
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
	unsigned long long * cycle = nullptr;
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