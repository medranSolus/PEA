#include "pch.h"
#include "FileService.h"
#include "Dynamic.h"
#include "BruteForce.h"
#include "BranchAndBound.h"
#include "TabuSearch.h"
#include "SimulatedAnnealing.h"
#include "Genetic.h"
#include "AntColony.h"
#include <ctime>
#include <thread>
using namespace std;

void testBF();
void testDP();
void testBnB();
void testInstanceSA(const string & name, const vector<unsigned int> & sizes, const string & out, const vector<long long> & optimal);
void testInstanceTS(const string & name, const vector<unsigned int> & sizes, const string & out, const vector<long long> & optimal, bool random = false);
void testTSSA();

constexpr size_t START_SIZE = 443; // 2720
constexpr const char * TEST_FILE = "Tests\\ATSP\\data";

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));
	shared_ptr<Graph> graph = FileService::readGraph(TEST_FILE + to_string(START_SIZE) + ".txt");
	unsigned long long * cycle = Algorithm::Genetic::getMinimalCycle(graph, 1500, 15000, 0.1f, Algorithm::Genetic::CrossType::Order);
	Algorithm::showCycle(cycle, graph->getSize());
	graph->showCost(cycle);
	return 0;
}

void testTSSA()
{
	thread mall2(testInstanceTS, "Tests\\SMALL\\data", move(vector<unsigned int>({ 10,18 })), "DATA\\fast_small", move(vector<long long>({ 212,187 })), false);
	thread tsp2(testInstanceTS, "Tests\\TSP\\data", move(vector<unsigned int>({ 17,58,120 })), "DATA\\fast_tsp", move(vector<long long>({ 2085,25395,6942 })), false);
	thread atsp2(testInstanceTS, "Tests\\ATSP\\data", move(vector<unsigned int>({ 17,43,100 })), "DATA\\fast_atsp", move(vector<long long>({ 39,5620,36230 })), false);
	mall2.join();
	thread mall3(testInstanceSA, "Tests\\SMALL\\data", move(vector<unsigned int>({ 10,18 })), "DATA\\nor_small", move(vector<long long>({ 212,187 })));
	tsp2.join();
	thread tsp3(testInstanceSA, "Tests\\TSP\\data", move(vector<unsigned int>({ 17,58,120 })), "DATA\\nor_tsp", move(vector<long long>({ 2085,25395,6942 })));
	atsp2.join();
	thread atsp3(testInstanceSA, "Tests\\ATSP\\data", move(vector<unsigned int>({ 17,43,100 })), "DATA\\nor_atsp", move(vector<long long>({ 39,5620,36230 })));
	mall3.join();
	tsp3.join();
	atsp3.join();
}

void testInstanceTS(const string & name, const vector<unsigned int> & sizes, const string & out, const vector<long long> & optimal, bool random)
{
	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	unsigned long long * cycle = nullptr;
	vector<float> tabuRevokeTime({ 0.2f, 0.7f });
	vector<Algorithm::TabuSearch::InitialType> init({ Algorithm::TabuSearch::InitialType::NN, Algorithm::TabuSearch::InitialType::RandomNN, Algorithm::TabuSearch::InitialType::Random });
	double cost;
	for (size_t s = 0; s < sizes.size(); ++s)
	{
		shared_ptr<Graph> graph = FileService::readGraph(name + to_string(sizes.at(s)) + ".txt");
		for (auto ini : init)
		{
			const string code = (ini == Algorithm::TabuSearch::InitialType::NN ? "_nn" : (ini == Algorithm::TabuSearch::InitialType::RandomNN ? "_rn" : "_ra"));
			for (auto revoke : tabuRevokeTime)
			{
				vector<vector<double>> data(20, std::move(vector<double>(9, 0.0)));
				for (size_t t = 0; t < 20; ++t)
				{
					data[t][0] = (t + 1) * 5.0;
					data[t][7] = DBL_MAX;
					const unsigned long long rT = static_cast<unsigned long long>(data[t][0] * revoke);
					for (size_t i = 0; i < 5; ++i)
					{
						start = getTime();
						cycle = Algorithm::TabuSearch::getMinimalCycle(graph, ini, Algorithm::MoveType::Swap, random, 100000, static_cast<unsigned long long>(data[t][0]), 500, rT);
						end = getTime();
						data[t][1] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
						cost = static_cast<double>(graph->getCost(cycle));
						if (cost < data[t][7])
						{
							data[t][7] = cost;
							data[t][8] = 1.0;
						}
						data[t][4] += cost;
						delete[] cycle;
						start = getTime();
						cycle = Algorithm::TabuSearch::getMinimalCycle(graph, ini, Algorithm::MoveType::Insert, random, 100000, static_cast<unsigned long long>(data[t][0]), 500, rT);
						end = getTime();
						data[t][2] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
						cost = static_cast<double>(graph->getCost(cycle));
						if (cost < data[t][7])
						{
							data[t][7] = cost;
							data[t][8] = 2.0;
						}
						data[t][5] += cost;
						delete[] cycle;
						start = getTime();
						cycle = Algorithm::TabuSearch::getMinimalCycle(graph, ini, Algorithm::MoveType::Invert, random, 100000, static_cast<unsigned long long>(data[t][0]), 500, rT);
						end = getTime();
						data[t][3] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
						cost = static_cast<double>(graph->getCost(cycle));
						if (cost < data[t][7])
						{
							data[t][7] = cost;
							data[t][8] = 3.0;
						}
						data[t][6] += cost;
						delete[] cycle;
					}
					for (size_t j = 1; j <= 3; ++j)
					{
						data[t][j] *= 200.0; // ms
						data[t][j + 3] = 100.0 * (data[t][j + 3] / 5.0 - optimal.at(s)) / optimal.at(s);
					}
					data[t][7] = 100.0 * (data[t][7] - optimal.at(s)) / optimal.at(s);
				}
				cout << endl << out << "__g" << sizes.at(s) << '_' << code << "__rv" << to_string(static_cast<unsigned long long>(revoke * 100));
				FileService::saveData(data, { "TabuT", "SWAP", "INS", "INV", "SWAPc", "INSc", "INVc", "BESTc", "BEST_type" }, out + "_g" + to_string(sizes.at(s)) + code + "_rv" + to_string(static_cast<unsigned long long>(revoke * 100)) + ".txt");
			}
		}		
	}
}

void testInstanceSA(const string & name, const vector<unsigned int> & sizes, const string & out, const vector<long long> & optimal)
{
	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	unsigned long long * cycle = nullptr;
	vector<unsigned long long> iter({ 100000 });
	vector<float> startTemps({ 1000000.0f });
	vector<float> endTemps({ 5.0f });
	double cost;
	for (size_t s = 0; s < sizes.size(); ++s)
	{
		shared_ptr<Graph> graph = FileService::readGraph(name + to_string(sizes.at(s)) + ".txt");
		for (auto sT : startTemps)
		{
			for (auto eT : endTemps)
			{
				for (auto localTime : iter)
				{
					vector<vector<double>> data(19, std::move(vector<double>(9, 0.0)));
					for (size_t i = 0; i < 19; ++i)
					{
						data[i][0] =  (i + 1) / 20.0;
						data[i][7] = DBL_MAX;
						for (size_t z = 0; z < 5; ++z)
						{
							start = getTime();
							cycle = Algorithm::SimulatedAnnealing::getMinimalCycle(graph, Algorithm::MoveType::Swap, sT, static_cast<float>(data[i][0]), eT, localTime);
							end = getTime();
							data[i][1] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
							cost = static_cast<double>(graph->getCost(cycle));
							if (cost < data[i][7])
							{
								data[i][7] = cost;
								data[i][8] = 1.0;
							}
							data[i][4] += cost;
							delete[] cycle;
							start = getTime();
							cycle = Algorithm::SimulatedAnnealing::getMinimalCycle(graph, Algorithm::MoveType::Insert, sT, static_cast<float>(data[i][0]), eT, localTime);
							end = getTime();
							data[i][2] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
							cost = static_cast<double>(graph->getCost(cycle));
							if (cost < data[i][7])
							{
								data[i][7] = cost;
								data[i][8] = 2.0;
							}
							data[i][5] += cost;
							delete[] cycle;
							start = getTime();
							cycle = Algorithm::SimulatedAnnealing::getMinimalCycle(graph, Algorithm::MoveType::Invert, sT, static_cast<float>(data[i][0]), eT, localTime);
							end = getTime();
							data[i][3] += (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
							cost = static_cast<double>(graph->getCost(cycle));
							if (cost < data[i][7])
							{
								data[i][7] = cost;
								data[i][8] = 3.0;
							}
							data[i][6] += cost;
							delete[] cycle;
						}
						for (size_t j = 1; j <= 3; ++j)
						{
							data[i][j] *= 200.0; // ms
							data[i][j + 3] = 100.0 * (data[i][j + 3] / 5.0 - optimal.at(s)) / optimal.at(s);
						}
						data[i][7] = 100.0 * (data[i][7] - optimal.at(s)) / optimal.at(s);
					}
					cout << endl << out << "__g" << sizes.at(s) << "__i" << localTime << "__st" << static_cast<unsigned long long>(sT) << "__et" << static_cast<unsigned long long>(eT);
					FileService::saveData(data, { "Fac", "SWAP", "INV", "INS", "SWAPc", "INVc", "INSc", "BESTc", "BEST_type" }, out + "_g" + to_string(sizes.at(s))  + "__st" + to_string(static_cast<unsigned long long>(sT)) + "__et" + to_string(static_cast<unsigned long long>(eT)) + "__i" + to_string(localTime) + ".txt");
				}
			}
		}
	}
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