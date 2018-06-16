#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "RegularFinalStateTransducerBuilder.h"
#include "Tests.h"

typedef std::unordered_map<std::string, // The regular expression 
						   std::vector<
							   std::pair<std::string,  // A list of words to test and
							   std::vector<int>>>> // a list of desired outputs (if empty then the word is not from the language) 
	FinalStateTransducerTestCases;

static FinalStateTransducerTestCases FSTTestcases;

void PopulateWithTestCases()
{
	FSTTestcases["a:5"] = {
		{ "", {  } },
		{ "b", {  } },
		{ "aa", {  } },
		{ "a", { 5 } },
	};

	FSTTestcases["a:5 *"] = {
		{ "", {  } },
		{ "b", {  } },
		{ "a",{ 5 } },
		{ "aa", { 10 } },
	};

	FSTTestcases["a:5 a:100 | *"] = {
		{ "", {  } },
		{ "b", {  } },
		{ "a",{ 5, 100 } },
		{ "aa", { 10, 105, 200 } },
		{ "aaa", { 15, 110, 205, 300 } },
		{ "aaaa", { 20, 115, 210, 305, 400 } },
		{ "aaaaa", { 25, 120, 215, 310, 405, 500 } },
	};

	FSTTestcases["a:5 b:100 | c:1 |"] = {
		{ "", {  } },
		{ "x", { } },
		{ "xasffsaq", { } },
		{ "a", { 5 } },
		{ "c", { 1 } },
		{ "b", { 100 } },
	};
}

void RunFinalStateTransducerTests()
{
	PopulateWithTestCases();

	auto failedTests = 0;
	std::cout << "RUNNING " << FSTTestcases.size() << " FINAL STATE TRANSDUCER TESTS:\n";
	for (const auto& testCase : FSTTestcases)
	{
		const auto& regExpr = testCase.first;
		std::cout << "The regular expression is \"" << regExpr << "\". Running the test words & their outputs:\n";

		RegularFinalStateTransducerBuilder ts(regExpr.c_str());
		auto transducer = ts.GetBuildedTransducer();
		if (!transducer) { std::cout << "Something went wrong...\n"; continue; }

		int testNumber = 0;
		const auto& wordForTraversing = testCase.second;
		for (const auto& wordAndOutputs : wordForTraversing)
		{
			std::unordered_set<size_t> outputs;
			const auto& testWord = wordAndOutputs.first;
			const auto& testOutputs = wordAndOutputs.second;

			transducer->TraverseWithWord(testWord.c_str(), outputs);

			std::cout << "\t" << testNumber << ": \"" << testWord << "\", ";
			for (const auto& expectedOutput : testOutputs)
			{
				std::cout << expectedOutput << " ";
			}

			bool passed = true;
			if (outputs.size() == testOutputs.size())
			{
				for (const auto& expectedOutput : testOutputs)
				{
					if (outputs.find(expectedOutput) == outputs.end())
					{
						passed = false;
						break;
					}
				}
			}

			if (passed)
			{
				std::cout << " - passing.\n";
			}
			else
			{
				std::cout << "\n\t\tFAILED\n";
				std::cout << "\t\tGot the following output: ";
				for (const auto& output : outputs)
				{
					std::cout << output << " ";
				}
				std::cout << ".\n";
				++failedTests;
			}
			++testNumber;
		}
		std::cout << "\n";
	}

	if (failedTests > 0)
	{
		std::cout << "Passed " << FSTTestcases.size() - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << FSTTestcases.size() << " tests.\n";
	}
}