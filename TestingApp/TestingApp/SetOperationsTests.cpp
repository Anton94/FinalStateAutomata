#include <iostream>
#include "SetOperations.h"

typedef std::vector<std::pair<
							  SetOfTransitions, // The input set.
							  SetOfTransitions>> // The desired output one.
	TransitiveSetTestCases;

static TransitiveSetTestCases transitiveSetTestCases;

static void PopulateWithTestCases()
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
	};
}

void print(const SetOfTransitions& set)
{
	for (const auto& transitionAndDestinations : set)
	{
		const auto& transition = transitionAndDestinations.first;
		const auto& destinations = transitionAndDestinations.second;

		std::cout << "\t{ " << transition << " - ";
		for (const auto& destination : destinations)
		{
			std::cout << destination << " ";
		}
		std::cout << "}\n";
	}
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
	PopulateWithTestCases();

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
			std::cout << "passed.\n";
		}
		else
		{
			std::cout << "failed\n"
				<< "Exprected the transitive closure of:\n";
			print(outputSet);
			std::cout << "To be equal to the set:\n";
			print(inputSetClosed);
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