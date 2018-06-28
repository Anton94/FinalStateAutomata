#include <iostream>
#include "FinalStateTransducer.h"
#include "RegularFinalStateTransducerBuilder.h"
#include "InputValidator.h"
#include "Tests.h"
#include "TestCaseGenerator.h"
#include "CustomTestExecuter.h"

const char * regExpr = "a:5 b:100 | c:1 |";

void ProcessCommandLineArguments(int argc, char *argv[])
{
	for (auto i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "-validateInput") == 0)
		{
			// TODO: take care for the validation at some point...
			//std::cout << "---Validating the input string \"" << regExpr << "\"...\n";
			//auto isValid = checkInputCorrectness(regExpr);
			//std::cout << (isValid ? "---[valid]" : "***[NOT valid]")
			//	<< ".\n\n";
		}
	}
}

int main(int argc, char *argv[])
{
	//GenerateCustomWordConcatenationsAndIncreasingOutputs(std::string{ "word" }, 2000);
	//GenerateCustomWordUnionsAndIncreasingOutputs(std::string{ "abc" }, 250);
	//ExecuteCustomTestFromFile(std::string{ "test1.txt" });
	//ExecuteCustomTestFromFile(std::string{ "test2000ConcatWordsWithIncrOuts.txt" });
	//ExecuteCustomTestFromFile(std::string{ "test500UnionWordsWithIncrOuts.txt" });
	//ExecuteCustomTestFromFile(std::string{ "test100UnionWordsWithIncrOuts.txt" });
	//ExecuteCustomTestFromFile(std::string{ "test250UnionWordsWithIncrOuts.txt" });
	//ExecuteCustomTestFromFile(std::string{ "N.txt" });
	//ExecuteCustomTestFromFile(std::string{ "N1.txt" });
	ExecuteCustomTestFromFile(std::string{ "N2.txt" });
	//ExecuteCustomTestFromFile(std::string{ "N3.txt" });

	//ProcessCommandLineArguments(argc, argv);
	//RunInputValidationTests();

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