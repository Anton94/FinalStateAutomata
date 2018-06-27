#include <fstream>
#include <streambuf>
#include <stdlib.h> // atoi
#include <unordered_set>

#include "CustomTestExecuter.h"
#include "RegularFinalStateTransducerBuilder.h"


void ExecuteCustomTestFromFile(std::string& fileName)
{
	std::string regex;

	std::ifstream f(fileName);

	getline(f, regex);

	std::cout << "Regex is: \"" << regex << "\"\n";

	std::cout << "Building the transducer...\n";
	RegularFinalStateTransducerBuilder ts(regex.c_str());
	auto transducer = ts.GetBuildedTransducer();

	std::cout << "Trying to make it a real-time...\n";
	bool infinite = transducer->MakeRealTime();
	std::cout << "\tFST is " << (transducer->IsRealTime() ? "real-time" : "not real-time") << ".\n";
	std::cout << "\tFST is " << (infinite ? "infinite" : "not infinite") << ".\n";

	transducer->UpdateRecognizingEmptyWord();

	std::cout << "Making a functionality test...\n";
	bool functional = transducer->TestForFunctionality();

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

		transducer->TraverseWithWord(word.c_str(), outputs);

		std::cout << " : ";
		for (const auto& expectedOutput : outputs)
		{
			std::cout << expectedOutput << " ";
		}
		std::cout << "\n";
	}
}

