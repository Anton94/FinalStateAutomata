#include <iostream>
#include "FinalStateTransducer.h"
#include "RegularFinalStateTransducerBuilder.h"
#include "InputValidator.h"
#include "Tests.h"

//const char * regExpr = "a:5 * a:100 * | *"; // inf loop, inf outputs..
const char * regExpr = "a:5 b:100 | *";

const char* wordsForTraversion[] = {
	"b",
	"a",
	"aa",
	"aaa",
	"aaaa",
	"ababa",
	"abba",
};

void ProcessCommandLineArguments(int argc, char *argv[])
{
	for (auto i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "-validateInput") == 0)
		{
			std::cout << "---Validating the input string...\n";
			auto isValid = checkInputCorrectness(regExpr);
			std::cout << (isValid ? "---[valid]" : "***[NOT valid]")
				<< ".\n";
		}
	}
}

int main(int argc, char *argv[])
{
	ProcessCommandLineArguments(argc, argv);
	//RunInputValidationTests();

	RegularFinalStateTransducerBuilder builder(regExpr);
	FinalStateTransducer* tr = builder.GetBuildedTransducer();

	for (size_t i = 0, count = sizeof(wordsForTraversion) / sizeof(const char*); i < count; ++i)
	{
		tr->TraverseWithWord(wordsForTraversion[i]);
	}
	return 0;
}