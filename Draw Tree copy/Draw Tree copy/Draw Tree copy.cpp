#include <SDL.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <thread>
#include "Framework.h"
#include "Tree.h"
#include "Random.h"

namespace fs = std::filesystem;

void printVectorInt(const std::vector<int> &ints)
{
	for(int i = 0; i < ints.size(); i++)
	{
		std::cout << ints[i] << ", ";
	}
	std::cout << '\n';
}

std::vector<std::string> getAllTXTinPath(std::string path)
{
	std::vector<std::string> txts(0);
	for(const auto &entry : fs::directory_iterator(path))
	{
		std::string name = entry.path().string();
		if(name.substr(name.size() - 4) == ".txt")
		{
			txts.push_back(name);
		}
	}
	return txts;
}

int countTerminals(Node *head)
{
	if(!head->IsCircle)
	{
		return 1;
	}
	return countTerminals(head->Child1) + countTerminals(head->Child2);
}

int getType(Node *head)
{
	int type = 1;
	while(head->Child2->IsCircle)
	{
		int count = countTerminals(head->Child1);
		if(count > 2)
		{
			return 3;
		}
		if(count > 1)
		{
			type = 2;
		}
		head = head->Child2;
	}
	return type;
}

void generateOutput(std::string *output, int n)
{
	*output = "";

	double maxProb = 0.5 * (3 - std::sqrt(5));

	//double randMin = Random::get(0.0, maxProb);
	double randMax = Random::get(0.001, maxProb);
	/*if(randMin > randMax)
	{
		double hold = randMin;
		randMin = randMax;
		randMax = hold;
	}*/
	int n1 = Random::get(1, n - 1);
	int n2 = n - n1;
	//std::vector<double> Probabilities = Random::GenerateDecending(randMin, randMax, n);
	std::vector<double> Probabilities(n1 + n2, randMax);
	//std::vector<double> Probabilities2(n2, randMin);
	//Probabilities.insert(Probabilities.end(), Probabilities2.begin(), Probabilities2.end());
	std::vector<double> Weights = GetWeights(Probabilities);

	std::vector<int> IDs;
	for(int j = Weights.size(); j > 0; j--)
	{
		IDs.push_back(j);
	}

	Tree treeFast(Weights, IDs, true);

	//int type = 1;

	//int z = Weights.size() - 2;
	//int r = 0;
	//for(; z > 0; z--)
	//{
	//	if(treeFast.StartingList[z - 1]->Level >= treeFast.StartingList[z]->Level)
	//	{
	//		type = 2;
	//		r = Weights.size() - z;
	//		break;
	//	}
	//}
	//z -= 2;
	//for(; z > 0; z -= 2)
	//{
	//	if(treeFast.StartingList[z]->Level != treeFast.StartingList[z + 1]->Level)
	//	{
	//		type = 3;
	//		break;
	//	}
	//}

	int type = getType(treeFast.Head);
	double sum3 = treeFast.StartingList[0]->Weight + treeFast.StartingList[1]->Weight + treeFast.StartingList[2]->Weight;

	*output += /*std::to_string(n1) + "," + std::to_string(n2)*/std::to_string(n1 + n2) + "," + std::to_string(randMax)/* + "," + std::to_string(randMin)*/ + "," + std::to_string(type) + "," + std::to_string(sum3);
}

int main(int argc, char *argv[])
{
	const int totalHeight = 1000;
	const int totalWidth = 1500;

	/*std::vector<double> roots;

	for(int i = 3; i <= 8; i++)
	{
		roots.push_back(1 - RootFinder(i, 0.000001));
		std::cout << i << ": " << roots[i - 3] << std::endl;
	}*/

	/*std::vector<double> rootProbabilities;
	for(int i = 0; i < roots.size() - 1; i++)
	{
		rootProbabilities.push_back((roots[i] + roots[i + 1]) / 2);
	}*/

	//Framework fw(totalHeight, totalWidth);

	/*for(int i = 0; i < rootProbabilities.size(); i++)
	{
		std::vector<double> probabilities(i + 7, rootProbabilities[i]);
		std::vector<int> IDs;
		for(int i = 1; i <= probabilities.size(); i++)
			IDs.push_back(i);
		while(probabilities.size() > i + 1)
		{
			fw.clear();
			std::vector<double> weights = GetWeights(probabilities);
			fw.draw_tree(weights, IDs);
			fw.screenshot("C:\\Users\\Admin\\Documents\\Programming\\LMT projektas\\Files\\p=" + std::to_string(rootProbabilities[i]).substr(0, std::to_string(rootProbabilities[i]).find('.') + 3) + " r=" + std::to_string(i + 3) + " n=" + std::to_string(weights.size()) + ".png");
			probabilities.erase(probabilities.begin());
			IDs.erase(IDs.begin());
		}
	}*/

	/*using namespace std::chrono;

	long long timeTakenSlow{0};
	long long timeTakenFast{0};
	int weightsSize = 3;
	std::ofstream output("times.csv");
	output << "n,SlowTime,FastTime\n";
	while(timeTakenSlow < 100000)
	{
		std::vector<double> Weights = Random::GenerateValley(1, 100, weightsSize);

		std::vector<int> IDs;
		for(int j = Weights.size(); j > 0; j--)
		{
			IDs.push_back(j);
		}

		auto start = high_resolution_clock::now();

		Tree treeSlow(Weights, IDs);

		auto stop = high_resolution_clock::now();

		auto duration = duration_cast<microseconds>(stop - start);
		timeTakenSlow = duration.count();

		start = high_resolution_clock::now();

		Tree treeFast(Weights, IDs, true);

		stop = high_resolution_clock::now();

		duration = duration_cast<microseconds>(stop - start);
		timeTakenFast = duration.count();

		output << weightsSize << ',' << timeTakenSlow << ',' << timeTakenFast << '\n';

		if(weightsSize % 100 == 0)
		{
			std::cout << weightsSize << "; Slow: " << timeTakenSlow << "; Fast: " << timeTakenFast << '\n';
		}

		weightsSize++;
	}

	output.close();*/

	//using namespace std::chrono;

	//double maxProb = 0.5 * (3 - std::sqrt(5));

	//std::ofstream output("Vadovo testai.csv");
	//output << "x1,x2,x3,SlowTime,FastTime\n";
	//int maxTreeSize = 10000;
	//int nOfReplications = 1;

	//for(int i = 3; i <= maxTreeSize; i++)
	//{
	//	std::cout << i << '\n';
	//	for(int j = 1; j <= nOfReplications; j++)
	//	{
	//		std::vector<double> Weights = Random::GenerateValley(0, maxProb, i);

	//		std::vector<int> IDs;
	//		for(int j = Weights.size(); j > 0; j--)
	//		{
	//			IDs.push_back(j);
	//		}

	//		auto start = high_resolution_clock::now();

	//		//Tree treeSlow(Weights, IDs);

	//		auto stop = high_resolution_clock::now();

	//		auto duration = duration_cast<microseconds>(stop - start);
	//		double timeTakenSlow = duration.count();

	//		start = high_resolution_clock::now();

	//		Tree treeFast(Weights, IDs, true);

	//		stop = high_resolution_clock::now();

	//		duration = duration_cast<microseconds>(stop - start);
	//		double timeTakenFast = duration.count();

	//		output << i << ',' << i * std::log(i) << ',' << i * i << ',' << timeTakenSlow << ',' << timeTakenFast << '\n';
	//	}
	//}
	//output.close();

	using namespace std::chrono;
	int threads = std::thread::hardware_concurrency();
	std::cout << threads << '\n' << '\n';

	double maxProb = 0.5 * (3 - std::sqrt(5));

	std::ofstream output("Medziu strukturos.csv");
	int maxTreeSize = 1000;
	int nOfReplications = 1000;

	output << "n,p,Type,Sum3\n";

	for(int i = 3; i <= maxTreeSize; i++)
	{
		std::cout << i << '\n';
		for(int j = 1; j <= nOfReplications/threads + 1; j++)
		{
			std::vector<std::string> outputs(threads);
			std::vector<std::thread> currentThreads;

			for(int z = 0; z < threads; z++)
			{
				currentThreads.push_back(std::thread(generateOutput, &(outputs[z]), i));
			}
			for(int z = 0; z < threads; z++)
			{
				currentThreads[z].join();
			}

			for(int z = 0; z < threads; z++)
			{
				output << outputs[z] << '\n';
			}
		}
	}

	output.close();

	/*for(int i = 0; i < 20; i++)
	{
		std::vector<double> Weights = Random::GenerateValley(0, 0.2, 10);

		for(int i = 0; i < Weights.size(); i++)
		{
			std::cout << Weights[i] << ' ';
		}
		std::cout << '\n';

		std::vector<int> IDs;
		for(int j = Weights.size(); j > 0; j--)
		{
			IDs.push_back(j);
		}

		fw.clear();
		fw.draw_tree(Weights, IDs);
		fw.screenshot("C:\\Users\\Admin\\Documents\\Programming\\LMT projektas\\Files\\" + std::to_string(i) + " Slow.png");
		std::cout << "Slow clear!\n";
		fw.clear();
		fw.draw_tree(Weights, IDs, true);
		fw.screenshot("C:\\Users\\Admin\\Documents\\Programming\\LMT projektas\\Files\\" + std::to_string(i) + " Fast.png");
		std::cout << "Fast clear!\n";
	}*/

	//Framework fw(totalHeight, totalWidth);

	//std::vector<double> probabilities;
	//for(int i = 0; i < 8; i++)
	//{
	//	probabilities.push_back(0.16);
	//}
	///*for(int i = 0; i < 7; i++)
	//{
	//	probabilities.push_back(0.112961
	//	);
	//}*/
	//probabilities = GetWeights(probabilities);
	//std::vector<int> IDs;
	//for(int i = probabilities.size(); i > 0; i--)
	//{
	//	IDs.push_back(i);
	//}
	//fw.clear();
	//fw.draw_tree(probabilities, IDs, true);
	//fw.screenshot("test.png");

	//SDL_Event event{};    // Event variable

	//// Below while loop checks if the window has terminated using close in the
	////  corner.
	//while (!(event.type == SDL_QUIT))
	//{
	//	SDL_Delay(10);  // setting some Delay
	//	SDL_PollEvent(&event);  // Catching the poll event.
	//}

	return 0;
}