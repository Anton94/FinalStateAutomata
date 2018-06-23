#include <iostream>
#include "SetOperations.h"

typedef std::vector<std::pair<
							  SetOfTransitions, // The input set.
							  SetOfTransitions>> // The desired output one.
	TransitiveSetTestCases;

struct ClosureEpsilonTestCase
{
	SetOfTransitionsWithOutputs input; // The input set.
	SetOfTransitionsWithOutputs output; // The desired output one.
	bool inf; // For the infinite state.
};

typedef std::vector<ClosureEpsilonTestCase>
	ClosureEpsilonTestCases;

static TransitiveSetTestCases transitiveSetTestCases;

static void PopulateWithTransitiveClosureTestCases()
{
	transitiveSetTestCases = {
		{
			{  // 1-->2 ; 2-->3 ; 3-->4 the input
				{ 1, { 2 } },
				{ 2, { 3 } },
				{ 3, { 4 } },
			},
			{  // 1-->2 1-->3 1-->4 ; 2-->3 2-->4 ; 3-->4 the desired output
				{ 1, { 2, 3, 4 } },
				{ 2, { 3, 4 } },
				{ 3, { 4 } },
			},
		},

		{
			{
				{ 1, { 2 } },
				{ 2, { 5 } },
				{ 5, { 1 } },
			},
			{
				{ 1, { 1, 2, 5 } },
				{ 2, { 1, 2, 5 } },
				{ 5, { 1, 2, 5 } },
			},
		},

		{
			{
				{ 1, { 2 } },
				{ 2, { 5 } },
				{ 5, { 2 } },
			},
			{
				{ 1, { 2, 5 } },
				{ 2, { 2, 5 } },
				{ 5, { 2, 5 } },
			},
		},

		{
			{
				{ 1, { 2, 3 } },
				{ 2, { 5 } },
				{ 3, {} },
				{ 4, { 1 } },
				{ 5, { 4 } },
				{ 6, { 7 } },
			},
			{
				{ 1, { 1, 2, 3, 4, 5 } },
				{ 2, { 1, 2, 3, 4, 5 } },
				{ 3, {} },
				{ 4, { 1, 2, 3, 4, 5 } },
				{ 5, { 1, 2, 3, 4, 5 } },
				{ 6, { 7 } },
			},
		},
		// two cycles
		{
			{
				{ 1, { 2 } },
				{ 2, { 3 } },
				{ 3, { 1 } },
				{ 4, { 5 } },
				{ 5, { 6 } },
				{ 6, { 4 } },
			},
			{
				{ 1, { 1, 2, 3 } },
				{ 2, { 1, 2, 3 } },
				{ 3, { 1, 2, 3 } },
				{ 4, { 4, 5, 6 } },
				{ 5, { 4, 5, 6 } },
				{ 6, { 4, 5, 6 } },
			},
		},
		// two cycles connected
		{
			{
				{ 1, { 2 } },
				{ 2, { 3 } },
				{ 3, { 1, 4 } },
				{ 4, { 5 } },
				{ 5, { 6 } },
				{ 6, { 4 } },
			},
			{
				{ 1, { 1, 2, 3, 4, 5, 6 } },
				{ 2, { 1, 2, 3, 4, 5, 6 } },
				{ 3, { 1, 2, 3, 4, 5, 6 } },
				{ 4, { 4, 5, 6 } },
				{ 5, { 4, 5, 6 } },
				{ 6, { 4, 5, 6 } },
			},
		},
		// two cycles connected
		{
			{
				{ 1, { 2 } },
				{ 2, { 3 } },
				{ 3, { 1 } },
				{ 4, { 5 } },
				{ 5, { 6, 1 } },
				{ 6, { 4 } },
			},
			{
				{ 1, { 1, 2, 3 } },
				{ 2, { 1, 2, 3 } },
				{ 3, { 1, 2, 3 } },
				{ 4, { 1, 2, 3, 4, 5, 6 } },
				{ 5, { 1, 2, 3, 4, 5, 6 } },
				{ 6, { 1, 2, 3, 4, 5, 6 } },
			},
		},
		// two cycles connected
		{
			{
				{ 1, { 2 } },
				{ 2, { 3 } },
				{ 3, { 1 } },
				{ 4, { 5 } },
				{ 5, { 6, 5 } },
				{ 6, { 4 } },
			},
			{
				{ 1, { 1, 2, 3 } },
				{ 2, { 1, 2, 3 } },
				{ 3, { 1, 2, 3 } },
				{ 4, { 4, 5, 6 } },
				{ 5, { 4, 5, 6 } },
				{ 6, { 4, 5, 6 } },
			},
		},

		{
			{
				{ 1, { 2 } },
				{ 2, { 3 } },
				{ 3, { 1 } },
				{ 4, { 5 } },
				{ 5, { 6, 4 } },
				{ 6, { 4 } },
			},
			{
				{ 1, { 1, 2, 3 } },
				{ 2, { 1, 2, 3 } },
				{ 3, { 1, 2, 3 } },
				{ 4, { 4, 5, 6 } },
				{ 5, { 4, 5, 6 } },
				{ 6, { 4, 5, 6 } },
			},
		},
		// Missing destinationss
		{
			{
				{ 1, { 2, 3, 4 } },
				{ 2, { 6 } },
				{ 5, { 4 } },
				{ 16, { 19 } },
			},
			{
				{ 1, { 2, 3, 4, 6 } },
				{ 2, { 6 } },
				{ 5, { 4 } },
				{ 16, { 19 } },
			},
		},
	};
}

static ClosureEpsilonTestCases closureEpsilonTestCases;

static void PopulateWithClosureEpsilonTestCases()
{
	closureEpsilonTestCases = {
		{
			{  // 1-->(2, 5) ; 2-->(3, 10) ; 3-->(4, 20) the input
				{ 1, {
					{ 2, 5 } }
				},
				{ 2, {
					{ 3, 10 } }
				},
				{ 3, {
					{ 4, 20 } }
				},
			},
			{  // 1-->(2, 5) 1-->(3, 15) 1-->(4, 35) ; 2-->(3, 10) 2-->(4, 30) ; 3-->(4, 20) the desired output
				{ 1, {
					{ 2, 5 }, { 3, 15 }, { 4, 35 } }
				},
				{ 2, {
					{ 3, 10 }, { 4, 30 } }
				},
				{ 3, {
					{ 4, 20 } }
				},
			},
			false
		},
	};
}

static TransitiveSetTestCases addIdentityTestCases;

static void PopulateWithAddIdentityTestCases()
{
	addIdentityTestCases = {
		{
			{
				{ 1, { 2 } },
				{ 2, { 5 } },
				{ 5, { 1 } },
			},
			{
				{ 1, { 1, 2 } },
				{ 2, { 2, 5 } },
				{ 5, { 5, 1 } },
			},
		},
		{
			{
				{ 1, { 2, 3, 1 } },
				{ 2, { 5, 7, 9 } },
				{ 5, { 1 } },
			},
			{
				{ 1, { 2, 3, 1 } },
				{ 2, { 2, 5, 7, 9 } },
				{ 5, { 5, 1 } },
			},
		},
		{
			{
				{ 1, { 2, 3, 1 } },
				{ 2, { 5, 7, 9 } },
				{ 5, { 1 } },
				{ 6, { 6 } },
				{ 8, { 15 } },
				{ 9, { } },
			},
			{
				{ 1, { 2, 3, 1 } },
				{ 2, { 2, 5, 7, 9 } },
				{ 5, { 5, 1 } },
				{ 6, { 6 } },
				{ 8, { 8, 15 } },
				{ 9, { 9 } },
			},
		},
	};
}

bool equal(const SetOfTransitions& l, const SetOfTransitions& r)
{
	return l == r;
	//if (l.size() != r.size())
	//{
	//	return false;
	//}
	//for (const auto& transitionAndDestinations : l)
	//{
	//	const auto& transition = transitionAndDestinations.first;
	//	const auto& destinations = transitionAndDestinations.second;
	//
	//	auto it = r.find(transition);
	//	if (it == r.end())
	//	{
	//		return false;
	//	}
	//
	//	if (it->second != destinations)
	//	{
	//		return false;
	//	}
	//}
	//
	//return true;
}

void RunTransitiveClosureTests()
{
	PopulateWithTransitiveClosureTestCases();

	auto failedTests = 0;
	std::cout << "RUNNING TESTS WITH " << transitiveSetTestCases.size() << " TRANSITIVE CLOSURE TESTS:\n";
	for (size_t i = 0, bound = transitiveSetTestCases.size(); i < bound; ++i)
	{
		std::cout << "\t" << i << ": ";
		const auto& inputSet = transitiveSetTestCases[i].first;
		const auto& outputSet = transitiveSetTestCases[i].second;

		auto inputSetClosed = transitiveSetTestCases[i].first;

		TransitiveClosure(inputSetClosed);

		if (equal(inputSetClosed, outputSet))
		{
			std::cout << "passed\n";
		}
		else
		{
			std::cout << "failed!\n"
				<< "Exprected the transitive closure of:\n";
			Print(outputSet);
			std::cout << "To be equal to the set:\n";
			Print(inputSetClosed);
			++failedTests;
		}
	}

	if (failedTests > 0)
	{
		std::cout << "Passed " << transitiveSetTestCases.size() - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << transitiveSetTestCases.size() << " tests.\n";
	}
}

void RunCloseEpsilonTests()
{
	PopulateWithClosureEpsilonTestCases();

	auto failedTests = 0;
	std::cout << "RUNNING TESTS WITH " << transitiveSetTestCases.size() << " CLOSURE EPSILON TESTS:\n";
	for (size_t i = 0, bound = closureEpsilonTestCases.size(); i < bound; ++i)
	{
		std::cout << "\t" << i << ": ";
		const auto& inputSet = closureEpsilonTestCases[i].input;
		const auto& outputSet = closureEpsilonTestCases[i].output;

		auto inputSetClosed = closureEpsilonTestCases[i].input;

		bool infinite;
		ClosureEpsilon(inputSetClosed, infinite);

		const auto expectedInf = closureEpsilonTestCases[i].inf;

		if (infinite ||
			(inputSetClosed == outputSet && infinite == expectedInf))
		{
			std::cout << "passed\n";
		}
		else
		{
			std::cout << "failed!\n"
				<< "Exprected the closed epsilon of:\n";
			Print(outputSet);
			std::cout << "To be equal to the set:\n";
			Print(inputSetClosed);
			std::cout << "The expectd infinite was " << (expectedInf ? "true" : "false")
				<< "and the evaluated one is " << (infinite ? "true" : "false") << ".\n";
			++failedTests;
		}
	}

	if (failedTests > 0)
	{
		std::cout << "Passed " << closureEpsilonTestCases.size() - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << closureEpsilonTestCases.size() << " tests.\n";
	}
}

void RunAddIdentityTests()
{
	PopulateWithAddIdentityTestCases();

	auto failedTests = 0;
	std::cout << "RUNNING TESTS WITH " << addIdentityTestCases.size() << " ADD IDENTITY TESTS:\n";
	for (size_t i = 0, bound = addIdentityTestCases.size(); i < bound; ++i)
	{
		std::cout << "\t" << i << ": ";
		const auto& inputSet = addIdentityTestCases[i].first;
		const auto& outputSet = addIdentityTestCases[i].second;

		auto inputSetAddedIdentity = addIdentityTestCases[i].first;

		AddIdentity(inputSetAddedIdentity);

		if (equal(inputSetAddedIdentity, outputSet))
		{
			std::cout << "passed\n";
		}
		else
		{
			std::cout << "failed!\n"
				<< "Exprected the added identity of:\n";
			Print(outputSet);
			std::cout << "To be equal to the set:\n";
			Print(inputSetAddedIdentity);
			++failedTests;
		}
	}

	if (failedTests > 0)
	{
		std::cout << "Passed " << addIdentityTestCases.size() - failedTests << " tests.\n";
		std::cout << "Failed " << failedTests << " tests.\n";
	}
	else
	{
		std::cout << "Passed all " << addIdentityTestCases.size() << " tests.\n";
	}
}