#include <iostream>
#include "FinalStateTransducer.h"
#include "RegularFinalStateTransducerBuilder.h"
#include "InputValidator.h"
#include "Tests.h"
#include "TestCaseGenerator.h"
#include "CustomTestExecuter.h"

//const char * regExpr = "a:5 * a:100 * | *"; // inf loop, inf outputs..
const char * regExpr = "a:5 b:100 | c:1 |";

const char* wordsForTraversion[] = {
	"b",
	"a",
	"aa",
	"aaa",
	"aaaa",
	"ababa",
	"abbacd",
};

void ProcessCommandLineArguments(int argc, char *argv[])
{
	for (auto i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "-validateInput") == 0)
		{
			std::cout << "---Validating the input string \"" << regExpr << "\"...\n";
			auto isValid = checkInputCorrectness(regExpr);
			std::cout << (isValid ? "---[valid]" : "***[NOT valid]")
				<< ".\n\n";
		}
	}
}

int main(int argc, char *argv[])
{
	//GenerateCustomWordConcatenationsAndIncreasingOutputs(std::string{ "word" }, 2000);
	//ExecuteCustomTestFromFile(std::string{ "test1.txt" });
	ExecuteCustomTestFromFile(std::string{ "test2000ConcatWordsWithIncrOuts.txt" });

	//ProcessCommandLineArguments(argc, argv);
	////RunInputValidationTests();
	//
	//RunFinalStateTransducerTests();
	//RunTransitiveClosureTests();
	//RunAddIdentityTests();
	//RunCloseEpsilonTests();

	//RegularFinalStateTransducerBuilder builder(regExpr);
	//FinalStateTransducer* tr = builder.GetBuildedTransducer();
	//std::unordered_set<size_t> outputs;
	//for (size_t i = 0, count = sizeof(wordsForTraversion) / sizeof(const char*); i < count; ++i)
	//{
	//	tr->TraverseWithWord(wordsForTraversion[i], outputs);
	//}
	return 0;
}