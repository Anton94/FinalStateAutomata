#include <fstream>
#include <streambuf>
#include <stdlib.h> // atoi
#include <unordered_set>

#include <chrono>
#include <ctime>

#include "CustomTestExecuter.h"
#include "RegularFinalStateTransducerBuilder.h"

void PrintTime(std::chrono::duration<double> d)
{
	std::cout << "\t\t(elapsed time: " << d.count() << "s)\n";
}

void ExecuteCustomTestFromFile(std::string& fileName)
{
	std::string regex;

	std::ifstream f(fileName);

	getline(f, regex);

	std::cout << "Regex is: \"" << regex << "\"\n";

	std::cout << "Building the transducer...\n";

	auto startBuilding = std::chrono::system_clock::now();
	RegularFinalStateTransducerBuilder ts(regex.c_str());
	auto endBuilding = std::chrono::system_clock::now();
	std::chrono::duration<double> buildingTime = endBuilding - startBuilding;
	PrintTime(buildingTime);
	std::chrono::duration<double> totalTimeTaken = buildingTime;

	auto transducer = ts.GetBuildedTransducer();

	std::cout << "Trying to make it a real-time...\n";

	auto startRealTime = std::chrono::system_clock::now();
	bool infinite = transducer->MakeRealTime();
	auto endRealTime = std::chrono::system_clock::now();
	std::chrono::duration<double> realTimeFSTtime = endRealTime - startRealTime;
	totalTimeTaken += realTimeFSTtime;
	PrintTime(realTimeFSTtime);

	std::cout << "\tFST is " << (transducer->IsRealTime() ? "real-time" : "not real-time") << ".\n";
	std::cout << "\tFST is " << (infinite ? "infinite" : "not infinite") << ".\n";

	transducer->UpdateRecognizingEmptyWord();

	std::cout << "Making a functionality test...\n";

	auto startFunctionalityTest = std::chrono::system_clock::now();
	bool functional = transducer->TestForFunctionality();
	auto endFunctionalityTest = std::chrono::system_clock::now();
	std::chrono::duration<double> functionalityTestTime = endFunctionalityTest - startFunctionalityTest;
	totalTimeTaken += functionalityTestTime;
	PrintTime(functionalityTestTime);

	std::cout << "\tFST is " << (functional ? "functional" : "not functional") << ".\n";

	std::string numberOfWordsForTraversingFileLine;
	getline(f, numberOfWordsForTraversingFileLine);
	unsigned numberOfWordsForTraversing = atoi(numberOfWordsForTraversingFileLine.c_str());

	std::cout << "Traversing the following " << numberOfWordsForTraversing << " words:\n";

	for (auto i = 1u; i <= numberOfWordsForTraversing; ++i)
	{
		std::string word;

		getline(f, word);
		std::cout << "\t\"" << word << "\"";

		std::unordered_set<unsigned> outputs;

		auto start = std::chrono::system_clock::now();
		transducer->TraverseWithWord(word.c_str(), outputs);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTraversingTime = end - start;
		totalTimeTaken += elapsedTraversingTime;

		std::cout << " : ";
		for (const auto& expectedOutput : outputs)
		{
			std::cout << expectedOutput << " ";
		}
		PrintTime(elapsedTraversingTime);
		std::cout << "\n";
	}

	std::cout << "Total time taken: " << totalTimeTaken.count() << "s.\n";
}

