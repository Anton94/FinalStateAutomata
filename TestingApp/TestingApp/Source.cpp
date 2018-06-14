#include <iostream>
#include "FinalStateTransducer.h"
#include "RegularFinalStateTransducerBuilder.h"
#include "InputValidator.h"
#include "Tests.h"

const char * regExpr = "a:5 +";

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

	tr->TraverseWithWord("");
	tr->TraverseWithWord("a");
	tr->TraverseWithWord("aa");
	tr->TraverseWithWord("aaa");
	tr->TraverseWithWord("aaaa");

	return 0;
}