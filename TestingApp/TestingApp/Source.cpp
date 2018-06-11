#include <iostream>
#include "FinalStateTransducer.h"
#include "InputValidator.h"
#include "Tests.h"

const char * regExpr = "abc:5 * cdf:4 | ";

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
	RunInputValidationTests();

	return 0;
}