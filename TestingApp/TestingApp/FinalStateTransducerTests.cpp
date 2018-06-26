#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "RegularFinalStateTransducerBuilder.h"
#include "Tests.h"

struct TestCaseInfo
{
	std::string regExpr;
	bool infinite;
	bool functional;
	typedef std::pair<std::string,  // Word for traversing and expected outputs.
					  std::vector<int>> // a list of desired outputs (if empty then the word is not from the language)
		WordAndExpectedOutputs;
	std::vector<WordAndExpectedOutputs> wordsAndExpectedOutputs;
};

typedef std::vector<TestCaseInfo>
	FinalStateTransducerTestCases;

static FinalStateTransducerTestCases FSTTestcases;

static void PopulateWithTestCases()
{
	FSTTestcases.push_back({
		"a:5",
		false,
		true,
		{
			{ "", {} },
			{ "b", {} },
			{ "aa", {} },
			{ "a", { 5 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 *",
		false,
		true,
		{
			{ "", { 0 } },
			{ "b", {} },
			{ "a", { 5 } },
			{ "aa", { 10 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 a:100 | *",
		false,
		true,
		{
			{ "", { 0 } },
			{ "b", {} },
			{ "a", { 5, 100 } },
			{ "aa", { 10, 105, 200 } },
			{ "aaa", { 15, 110, 205, 300 } },
			{ "aaaa", { 20, 115, 210, 305, 400 } },
			{ "aaaaa", { 25, 120, 215, 310, 405, 500 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 a:100 | * c:40 * .",
		false,
		true,
		{
			{ "", { 0 } },
			{ "b", {} },
			{ "a", { 5, 100 } },
			{ "aa", { 10, 105, 200 } },
			{ "aaa", { 15, 110, 205, 300 } },
			{ "aaaa", { 20, 115, 210, 305, 400 } },
			{ "aaaaa", { 25, 120, 215, 310, 405, 500 } },
			{ "aaaaac", { 65, 160, 255, 350, 445, 540 } },
			{ "aaaac", { 60, 155, 250, 345, 440 } },
			{ "aaac", { 55, 150, 245, 340 } },
			{ "aac", { 50, 145, 240 } },
			{ "ac", { 45, 140 } },
			{ "c", { 40 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 |",
		false,
		true,
		{
			{ "", {} },
			{ "x", {} },
			{ "xasffsaq", {} },
			{ "a", { 5 } },
			{ "c", { 1 } },
			{ "b", { 100 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 .",
		false,
		true,
		{
			{ "", {} },
			{ "x", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "xasffsaq", {} },
			{ "ac", { 6 } },
			{ "bc", { 101 } },
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . *",
		false,
		true,
		{
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
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . * d:3 |",
		false,
		true,
		{
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
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . * d:3 | *",
		false,
		true,
		{
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
		}
	});
	FSTTestcases.push_back({
		"a:5 b:100 | c:1 . * d:3 | * x:7 .",
		false,
		true,
		{
			{ "", {} },
			{ "c", {} },
			{ "b", {} },
			{ "c", {} },
			{ "ab", {} },
			{ "bcc", {} },
			{ "xasffsaq", {} },
			{ "dacdadcd", {} },
			{ "d", {} },
			{ "ac", {} },
			{ "bc", {} },
			{ "bcbc", {} },
			{ "bcac", {} },
			{ "acbc", {} },
			{ "acac", {} },
			{ "acacbc", {} },
			{ "acacbcd", {} },
			{ "dacacbc", {} },
			{ "dacacbcd", {} },
			{ "acacd", {} },
			{ "dacacd", {} },
			{ "dacdacd", {} },
			{ "x", { 7 } },
			{ "dx", { 10 } },
			{ "acx", { 13 } },
			{ "bcx", { 108 } },
			{ "bcbcx", { 209 } },
			{ "bcacx", { 114 } },
			{ "acbcx", { 114 } },
			{ "acacx", { 19 } },
			{ "acacbcx", { 120 } },
			{ "acacbcdx", { 123 } },
			{ "dacacbcx", { 123 } },
			{ "dacacbcdx", { 126 } },
			{ "acacdx", { 22 } },
			{ "dacacdx", { 25 } },
			{ "dacdacdx", { 28 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 c:3 d:4 e:5 . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcde", { 15 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 c:3 d:4 e:5 . . . . +",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcde", { 15 } },
			{ "abcdeabcde", { 30 } },
			{ "abcdeabcdeabcde", { 45 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 * c:3 d:4 e:5 . . . .",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "acde", { 13 } },
			{ "abcde", { 15 } },
			{ "abbcde", { 17 } },
			{ "abbbbbcde", { 23 } },
		}
	});
	FSTTestcases.push_back({
		"a:1 b:2 * c:3 d:4 e:5 . . . . g:6 |",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "ag", {} },
			{ "abg", {} },
			{ "abcg", {} },
			{ "abcdg", {} },
			{ "abcdeeg", {} },
			{ "bcdeg", {} },
			{ "bdfg", {} },
			{ "aeg", {} },
			{ "acdeg", {} },
			{ "abcdeg", {} },
			{ "abbcdeg", {} },
			{ "abbbbbcdeg", {} },
			{ "gg",{ } },
			{ "g", { 6 } },
			{ "acde", { 13 } },
			{ "abcde", { 15 } },
			{ "abbcde", { 17 } },
			{ "abbbbbcde", { 23 } },
		}
	});
	FSTTestcases.push_back({
		"abcde:15",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcde", { 15 } },
		}
	});
	FSTTestcases.push_back({
		"abcde:15 +",
		false,
		true,
		{
			{ "", {} },
			{ "a", {} },
			{ "ab", {} },
			{ "abc", {} },
			{ "abcd", {} },
			{ "abcdee", {} },
			{ "bcde", {} },
			{ "bdf", {} },
			{ "ae", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcdeabcd", {} },
			{ "abcdea", {} },
			{ "abcde", { 15 } },
			{ "abcdeabcde", { 30 } },
			{ "abcdeabcdeabcde", { 45 } },
		}
	});
}


// TODO: test for e:5 word !!
void RunFinalStateTransducerTests()
{
	PopulateWithTestCases();

	auto failedTests = 0;
	auto testCases = 0;
	std::cout << "RUNNING TESTS WITH " << FSTTestcases.size() << " REG EXPRS:\n";
	for (const auto& testCase : FSTTestcases)
	{
		std::cout << "The regular expression is \"" << testCase.regExpr << "\". Running the test words & their outputs:\n";

		RegularFinalStateTransducerBuilder ts(testCase.regExpr.c_str());
		auto transducer = ts.GetBuildedTransducer();
		// If some test fails try to skip Real-time conversion.
		
		bool infinite;
		transducer->MakeRealTime(infinite);

		if (infinite != testCase.infinite)
		{
			std::cout << "FAILED: Expected the transducer to be "
				<< (testCase.infinite ? "infinite" : "non-infinite")
				<< " but it was not.\n";
			++failedTests;
		}

		int testNumber = 0;
		testCases += testCase.wordsAndExpectedOutputs.size();
		for (const auto& wordAndOutputs : testCase.wordsAndExpectedOutputs)
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