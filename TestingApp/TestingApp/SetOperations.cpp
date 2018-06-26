#include <iostream>
#include "SetOperations.h"


void TransitiveClosure(SetOfTransitions& r)
{
	/*
		I will devide them into blocks of processing transitions.
		First devided by the first transition state('a'); second on the level of "connection" (number of states to go through to get to the 'one')
	*/
	auto cR = r; // {<a, [b]>}, a map of states 'a' and their destinations, i.e. a set of b's ([b])

	for (auto& destinations : cR) // @destinations is a pair <a, [b]> for which the array [b] is all 'b', such that (a, b) belongs to 'cR'
	{
		std::unordered_set<unsigned> currentlyProcessingDestinations = destinations.second;

		while (currentlyProcessingDestinations.size() > 0)
		{
			std::unordered_set<unsigned> newDestinations;
			for (auto b : currentlyProcessingDestinations)
			{
				const auto it = r.find(b); // @it is a pair <b, [c]> for which the array [c] is all 'c', such that (b, c) belongs to 'r'
				if (it != r.end())
				{
					// Add only the new once, skip the already added!
					for (const auto& destination : it->second)
					{
						if (destinations.second.find(destination) == destinations.second.end())
						{
							newDestinations.insert(destination);
						}
					}
				}
			}

			// Add the currently processed elements to the destinations of 'a'
			destinations.second.insert(
				std::make_move_iterator(currentlyProcessingDestinations.begin()),
				std::make_move_iterator(currentlyProcessingDestinations.end()));
			// Process the newly added elements (so if they are connected to other states they will be added to the destinations of 'a'
			currentlyProcessingDestinations = std::move(newDestinations);
		}
	}

	r = std::move(cR);
}

void ClosureEpsilon(SetOfTransitionsWithOutputs& r, bool& infinite, std::unordered_set<unsigned>& statesWithEpsilonCycleWithPositiveOutput)
{
	statesWithEpsilonCycleWithPositiveOutput.clear();
	infinite = false;
	/*
	I will devide them into blocks of processing transitions.
	First devided by the first transition state('a'); second on the level of "connection" (number of states to go through to get to the 'one')
	*/
	auto cR = r; // { (a, [<b, o>]) }, a map of states 'a' and their destinations, i.e. a set of b's and coresponding outputs 'o' ([<b, o>])

	for (auto& destinations : cR) // @destinations is a pair <a, [<b, o>]> for which the array [<b, o>] is all 'b' and its output 'o', such that (a, <b, o>) belongs to 'cR'
	{
		std::unordered_set<Transition> currentlyProcessingDestinations = destinations.second;

		while (currentlyProcessingDestinations.size() > 0)
		{
			std::unordered_set<Transition> newDestinations;
			const auto& a = destinations.first;
			for (auto bAndO : currentlyProcessingDestinations)
			{
				const auto it = r.find(bAndO.state); // @it is a pair <b, [<c, o'>]> for which the array [<c, o'>] is all 'c' and its output 'o'', such that (b, <c, o'>) belongs to 'r'
				if (it != r.end())
				{
					// Add only the new once, skip the already added!
					for (const auto& destination : it->second)
					{
						const auto& destinationWithUpdatedOutput = Transition{ destination.state, destination.output + bAndO.output };
						if (destinationWithUpdatedOutput.state == a && destinationWithUpdatedOutput.output > 0)
						{
							infinite = true;
							statesWithEpsilonCycleWithPositiveOutput.insert(a);
						}
						else if (std::find_if(destinations.second.begin(),
							destinations.second.end(),
							[&destinationWithUpdatedOutput](const Transition& o) { return destinationWithUpdatedOutput.state == o.state; }
										) == destinations.second.end()
							)
						{
							newDestinations.insert(destinationWithUpdatedOutput);
						}
					}
				}
			}

			// Add the currently processed elements to the destinations of 'a'
			destinations.second.insert(
				std::make_move_iterator(currentlyProcessingDestinations.begin()),
				std::make_move_iterator(currentlyProcessingDestinations.end()));
			// Process the newly added elements (so if they are connected to other states they will be added to the destinations of 'a'
			currentlyProcessingDestinations = std::move(newDestinations);
		}
	}
	r = std::move(cR);
}

void AddIdentity(SetOfTransitions& r)
{
	for (auto& transitions : r)
	{
		transitions.second.insert(transitions.first);
	}
}

void AddIdentity(SetOfTransitionsWithOutputs& r, size_t numberOfStates)
{
	for (size_t i = 0; i < numberOfStates; ++i)
	{
		r[i].insert(Transition{ (unsigned)i, 0 });
	}
}

void Print(const SetOfTransitions& r)
{
	for (const auto& transitionAndDestinations : r)
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

void Print(const SetOfTransitionsWithOutputs& r)
{
	for (const auto& transitionAndDestinations : r)
	{
		const auto& transition = transitionAndDestinations.first;
		const auto& destinations = transitionAndDestinations.second;

		std::cout << "\t{ " << transition << " - ";
		for (const auto& destination : destinations)
		{
			std::cout << "(" << destination.state << ", " << destination.output << ") ";
		}
		std::cout << "}\n";
	}
}