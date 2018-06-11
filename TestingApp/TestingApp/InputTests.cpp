#include <iostream>
#include <vector>
#include "Tests.h"
#include "InputValidator.h"

std::vector<std::pair<const char*, bool>> inputValidationRegEprs = {
	{ " ", false },
	{ "abc:5 dfc:4 * .", true },
	{ "abc: 5 dfc:4 * ." , false },
	{ " abc:5 dfc:4 * ." , true }, // Initial space
	{ "abc:5 dfc:4 *." , false }, // Missing operation
	{ "abc:5 dfc:4 * . ." , false }, // Not enough object
	{ "abc:5 dfc:4 * . *" , true },
	{ "abc:5 dfc:4 * . d:4 ." , true },
	{ "abc:5 dfc:4 * . d:4 . *" , true },
	{ "abc:5 dfc:4 * . d:4 . |" , false },
	{ "abc:5 dfc:4 * . d:4 . d:1 |" , true },
	{ "abc:5 dfc:4 * . d:4 . d:1 | c:3" , false }, // Extra object
	{ "abc:5 dfc:4 * . d:4 . d:1 | c:" , false },
	{ "abc:5 dfc:4 * . d:4 . d:1 | c" , false },
	{ "abc:5 dfc:4 * . d:4 . d:1 | " , true },
	{ "abc:5 dfc:4 * . d:4 . d:1 | :" , false },
	{ "abc:5 dfc:4 * . d:4 . d:1 | : " , false },
	{ "abc:5 dfc:4 * . d:4 . d:1 | :5 " , false },
	{ "abc:5 dfc:4 * . d:4 . d:1 | 5 " , false },
	{ "   abc:5    dfc:4 *     .    d:4   .   d:1    |     " , true }, // Many spaces.
	{ ":5 " , false },
	{ "ac:513 " , true },
	{ "ac:513 * * * * *" , true },
	{ "ac:513" , true },
};

// Input validation tests
bool TestInputValidation(const char* regExpr, bool expectedResult, int testNumber)
{
	std::cout << "---" << testNumber << "---[\"" << regExpr << "\"]\n (info)\n";
	bool passed = checkInputCorrectness(regExpr) == expectedResult;
	std::cout << (passed ? "------[passed" : "*****[FAILED") << "].\n";
	return passed;
}

void RunInputValidationTests()
{
	auto failedTests = 0;
	std::cout << "Running " << inputValidationRegEprs.size() << " input validation tests.\n";
	for (int i = 0, bound = (int)inputValidationRegEprs.size(); i < bound; ++i)
	{
		const auto& input = inputValidationRegEprs[i];
		failedTests += static_cast<int>(!TestInputValidation(input.first, input.second, i));
		std::cout << std::endl;
	}

	if (failedTests > 0)
	{
		std::cout << "Passed " << inputValidationRegEprs.size() - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << inputValidationRegEprs.size() - failedTests << " tests.\n";
	}
}