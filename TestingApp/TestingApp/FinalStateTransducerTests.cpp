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
		{ "", {} },
		{ "b", {} },
		{ "aa", {} },
		{ "a", { 5 } },
	};
	FSTTestcases["a:5 *"] = {
		{ "", { 0 } },
		{ "b", {} },
		{ "a",{ 5 } },
		{ "aa", { 10 } },
	};
	FSTTestcases["a:5 a:100 | *"] = {
		{ "", { 0 } },
		{ "b", {} },
		{ "a",{ 5, 100 } },
		{ "aa", { 10, 105, 200 } },
		{ "aaa", { 15, 110, 205, 300 } },
		{ "aaaa", { 20, 115, 210, 305, 400 } },
		{ "aaaaa", { 25, 120, 215, 310, 405, 500 } },
	};
	FSTTestcases["a:5 b:100 | c:1 |"] = {
		{ "", {} },
		{ "x", {} },
		{ "xasffsaq", {} },
		{ "a", { 5 } },
		{ "c", { 1 } },
		{ "b", { 100 } },
	};
	FSTTestcases["a:5 b:100 | c:1 ."] = {
		{ "", {} },
		{ "x", {} },
		{ "c", {} },
		{ "b", {} },
		{ "c", {} },
		{ "ab", {} },
		{ "xasffsaq", {} },
		{ "ac", { 6 } },
		{ "bc", { 101 } },
	};
	FSTTestcases["a:5 b:100 | c:1 . *"] = {
		{ "", { 0 } },
		{ "x", {} },
		{ "c", {} },
		{ "b", {} },
		{ "c", {} },
		{ "ab", {} },
		{ "bcc", {} },
		{ "xasffsaq", {} },
		{ "ac", { 6 } },
		{ "bc", { 101 } },
		{ "bcbc", { 202 } },
		{ "bcac", { 107 } },
		{ "acbc", { 107 } },
		{ "acac", { 12 } },
		{ "acacbc", { 113 } },
	};
	FSTTestcases["a:5 b:100 | c:1 . * d:3 |"] = {
		{ "", { 0 } },
		{ "x", {} },
		{ "c", {} },
		{ "b", {} },
		{ "c", {} },
		{ "ab", {} },
		{ "bcc", {} },
		{ "xasffsaq", {} },
		{ "acacbcd", {} },
		{ "dacacbc", {} },
		{ "d", { 3 } },
		{ "ac", { 6 } },
		{ "bc", { 101 } },
		{ "bcbc", { 202 } },
		{ "bcac", { 107 } },
		{ "acbc", { 107 } },
		{ "acac", { 12 } },
		{ "acacbc", { 113 } },
	};
	FSTTestcases["a:5 b:100 | c:1 . * d:3 | *"] = {
		{ "", { 0 } },
		{ "x", {} },
		{ "c", {} },
		{ "b", {} },
		{ "c", {} },
		{ "ab", {} },
		{ "bcc", {} },
		{ "xasffsaq", {} },
		{ "dacdadcd", {} },
		{ "d", { 3 } },
		{ "ac", { 6 } },
		{ "bc", { 101 } },
		{ "bcbc", { 202 } },
		{ "bcac", { 107 } },
		{ "acbc", { 107 } },
		{ "acac", { 12 } },
		{ "acacbc", { 113 } },
		{ "acacbcd", { 116 } },
		{ "dacacbc", { 116 } },
		{ "dacacbcd", { 119 } },
		{ "acacd", { 15 } },
		{ "dacacd", { 18 } },
		{ "dacdacd", { 21 } },
	};
}

void RunFinalStateTransducerTests()
{
	PopulateWithTestCases();

	auto failedTests = 0;
	auto testCases = 0;
	std::cout << "RUNNING TESTS WITH " << FSTTestcases.size() << " REG EXPRS:\n";
	for (const auto& testCase : FSTTestcases)
	{
		const auto& regExpr = testCase.first;
		std::cout << "The regular expression is \"" << regExpr << "\". Running the test words & their outputs:\n";

		RegularFinalStateTransducerBuilder ts(regExpr.c_str());
		auto transducer = ts.GetBuildedTransducer();

		int testNumber = 0;
		const auto& wordForTraversing = testCase.second;
		testCases += wordForTraversing.size();
		for (const auto& wordAndOutputs : wordForTraversing)
		{
			std::unordered_set<size_t> outputs;
			const auto& testWord = wordAndOutputs.first;
			const auto& testOutputs = wordAndOutputs.second;

			transducer->TraverseWithWord(testWord.c_str(), outputs);

			std::cout << "\t" << testNumber << ": \"" << testWord << "\" ";
			for (const auto& expectedOutput : testOutputs)
			{
				std::cout << expectedOutput << " ";
			}

			bool passed = false;
			if (outputs.size() == testOutputs.size())
			{
				passed = true;
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
		std::cout << "Passed " << testCases - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << testCases << " tests.\n";
	}
}