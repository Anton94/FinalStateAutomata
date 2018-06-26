#include <stdlib.h> // atoi
#include <iostream>
#include <deque>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include "FinalStateTransducer.h"
#include "AssertLog.h"

FinalStateTransducer::FinalStateTransducer(char* regExpr, int separator, int length) // The regular expression should be of type: 'word:number'
	: RecognizingEmptyWord(false)
	, Infinite(false)
	, RealTime(false)
{
	char* word = regExpr;
	*(regExpr + separator) = '\0';
	*(regExpr + length) = '\0';
	const char* pOutputNumber = regExpr + separator + 1; // + ':'.
	size_t outputNumber = atoi(pOutputNumber);
#if defined(INFO)
		std::cout << "Creating a transducer for word \"" << word << "\" with output number " << outputNumber << std::endl; // Only info purposes.
#endif
	// Creating a transducer which accepts only the given word.
	Delta.resize(2);

	Delta[0][word].insert(Transition{ 1, outputNumber }); // One transition: 0 -> 1 via word @word and output @outputNumber.
	FinalStates.insert(1); // 1 is a final state.
	InitialStates.insert(0); // q0 which is 0 is the initial state
}

void FinalStateTransducer::CloseStar()
{
	ClosePlus();
	FinalStates.clear();
	FinalStates.insert(Delta.size() - 1); // Add the new state as a final,
}

void FinalStateTransducer::ClosePlus()
{
	// Make a new state with no transitions.
	Delta.push_back(StateTransitions());
	size_t newStateIndex = Delta.size() - 1;

	for (const auto& finalStateIndex : FinalStates)
	{
		Delta[finalStateIndex][""].insert(Transition{ newStateIndex, 0 }); // Add "finalState --e:0--> newState" transition
	}

	MakeSingleInitialState(newStateIndex);
}

void FinalStateTransducer::Concat(FinalStateTransducer& right)
{
	right.Remap(Delta.size());

	Delta.insert(Delta.end(),
		std::make_move_iterator(right.Delta.begin()),
		std::make_move_iterator(right.Delta.end()));

	for (auto& leftFinalStateIndex : FinalStates)
	{
		for (auto& rightInitialStateIndex : right.InitialStates)
		{
			Delta[leftFinalStateIndex][""].insert(Transition{ rightInitialStateIndex, 0 });
		}
	}

	FinalStates.clear();
	FinalStates.swap(right.FinalStates);
}

void FinalStateTransducer::Union(FinalStateTransducer& right)
{
	auto offset = Delta.size();
	right.RemapDelta(Delta.size());

	Delta.insert(Delta.end(),
		std::make_move_iterator(right.Delta.begin()),
		std::make_move_iterator(right.Delta.end()));

	MoveRightInitialStatesIntoLeft(right, offset);
	MoveRightFinalStatesIntoLeft(right, offset);
}


void FinalStateTransducer::Remap(size_t offset)
{
	RemapDelta(offset);
	RemapInitialStates(offset);
	RemapFinalStates(offset);
}

void FinalStateTransducer::RemapDelta(size_t offset)
{
	// I do not need this explicit remaping. TODO keep the offset 'per state' and recalculate "on the fly"?
	for (auto& state : Delta)
	{
		for (auto& transitions : state)
		{
			std::unordered_set<Transition> remapedTransitions;
			for (const auto& transition : transitions.second)
			{
				remapedTransitions.insert(Transition { transition.state + offset, transition.output });
			}
			transitions.second = std::move(remapedTransitions);
		}
	}
}

void FinalStateTransducer::RemapInitialStates(size_t offset)
{
	std::unordered_set<size_t> newInitialStates;
	for (auto& stateIndex : InitialStates)
	{
		newInitialStates.insert(stateIndex + offset);
	}
	InitialStates = std::move(newInitialStates);
}

void FinalStateTransducer::RemapFinalStates(size_t offset)
{
	std::unordered_set<size_t> newFinalStates;
	for (auto& stateIndex : FinalStates)
	{
		newFinalStates.insert(stateIndex + offset);
	}
	FinalStates = std::move(newFinalStates);
}

void FinalStateTransducer::MoveRightInitialStatesIntoLeft(FinalStateTransducer& right, size_t offset)
{
	for (auto& intialStateIndex : right.InitialStates)
	{
		InitialStates.insert(intialStateIndex + offset);
	}
}

void FinalStateTransducer::MoveRightFinalStatesIntoLeft(FinalStateTransducer& right, size_t offset)
{
	for (auto& finalStateIndex : right.FinalStates)
	{
		FinalStates.insert(finalStateIndex + offset);
	}
}

void FinalStateTransducer::MakeSingleInitialState(size_t newInitialStateIndex)
{
	for (const auto& initialStateIndex : InitialStates)
	{
		Delta[newInitialStateIndex][""].insert(Transition{ initialStateIndex, 0 }); // Add "newState --e:0--> initialState" transition
	}

	// Make the new state to be the initial one.
	InitialStates.clear();
	InitialStates.insert(newInitialStateIndex);
}

void FinalStateTransducer::Expand()
{
	std::string tmpWord = "x"; // TODO: make the transitions const char* or something better than std::string...

	for (size_t stateIndex = 0, bound = Delta.size(); stateIndex < bound; ++stateIndex)
	{
		StateTransitions transitionsExpanded;
		StateTransitions transitions = std::move(Delta[stateIndex]); // To avoid reference invalidation when(if) Delta resizes
		for (auto& transition : transitions)
		{
			// q --word--> [<r0, out0>, <r1, out1>, ... , <rk, ok>]
			const auto& transitionWord = transition.first;
			const auto transitionWordLen = transitionWord.length();
			if (transitionWordLen > 1)
			{
				auto& transitionDestinations = transition.second;
				for (auto& transitionDestination : transitionDestinations)
				{
					// q --word--> <r0, out0> => q --word[0]--> <r00, out0> --word[1]--> < r01, 0> --word[2]--> ... --word[len-1]--> <r0,0>

					const auto newStatesCount = transitionWordLen - 1;
					const auto currStatesCount = Delta.size();
					Delta.resize(currStatesCount + newStatesCount);
					auto newStateIndex = currStatesCount;

					// q --word[0]--> <r00, out0>
					const char* pWord = transitionWord.c_str();
					tmpWord[0] = *pWord++;
					transitionsExpanded[tmpWord].insert(Transition{ newStateIndex, transitionDestination.output });

					for (auto bound = currStatesCount + newStatesCount - 1; newStateIndex < bound; ++newStateIndex)
					{
						assert(*pWord);
						tmpWord[0] = *pWord++;
						Delta[newStateIndex][tmpWord].insert(Transition{ newStateIndex + 1, 0 }); // rk --word[k]--> <rk+1, 0>
					}

					assert(*pWord);
					tmpWord[0] = *pWord++;
					Delta[newStateIndex][tmpWord].insert(Transition{ transitionDestination.state, 0 });
				}

			}
			else
			{
				transitionsExpanded[transitionWord] = std::move(transition.second);
			}
		}

		Delta[stateIndex] = std::move(transitionsExpanded);
	}
}

// Separates the epsilon transitions in @v (those with @output 0) to the set @s
void SeparateEpsilonTransitions(std::unordered_set<Transition>& v, size_t output, std::unordered_set<size_t>& s)
{
	auto it = v.begin();
	while (it != v.end())
	{
		if (it->output == 0)
		{
			s.insert(it->state);
			it = v.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void FinalStateTransducer::RemoveEpsilon()
{
	SetOfTransitions Ce;

	// Add only the states which has (e,0) transition (i.e. transition with empty word and 0 output to another state)
	// and in the same time remove them from Delta.
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		auto& state = Delta[i];

		auto it = state.find("");
		if (it != state.end())
		{
			SeparateEpsilonTransitions(it->second, 0, Ce[i]);
			if (it->second.size() == 0)
			{
				Delta[i].erase("");
			}
		}
	}

	TransitiveClosure(Ce);
//	AddIdentity(Ce); // I do not need it

	// New Delta
	for (auto& state : Delta)
	{
		for (auto& transitionWordAndDestinations : state)
		{
			// TODO: change to set??
			auto destinationsCopy = transitionWordAndDestinations.second;
			for (auto& transition : destinationsCopy)
			{
				for (auto newDest : Ce[transition.state])
				{
					auto t = Transition{ newDest, transition.output };

					transitionWordAndDestinations.second.insert(std::move(t));
				}
			}
		}
	}

	// Change the new initial states
	std::unordered_set<size_t> newInitialStates = InitialStates;
	for (auto& initialStateIndex : InitialStates)
	{
		newInitialStates.insert(Ce[initialStateIndex].begin(), Ce[initialStateIndex].end());
	}
	InitialStates = std::move(newInitialStates);
}

void FinalStateTransducer::RemoveUpperEpsilon(bool& infinite)
{
	InitialEpsilonOutputs.clear();
	CloseEpsilonOnStates.clear();

	SetOfTransitionsWithOutputs& Ce = CloseEpsilonOnStates;
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		auto& state = Delta[i];

		auto it = state.find("");
		if (it != state.end())
		{
			Ce[i].insert(it->second.begin(), it->second.end());
		}
	}

	ClosureEpsilon(Ce, infinite, StatesWithEpsilonCycleWithPositiveOutput);
	if (infinite)
	{
		return;
	}

	// Remove the epsilon transitions
	for (auto& state : Delta)
	{
		state.erase("");
	}

	AddIdentity(Ce, Delta.size());

	// Keep the initial outputs with the empty word on the inpute line.
	// <q, <r, o>> belings to Ce & q belongs to InitialStates & r belongs to FinalStates Then keep the output 'o'.
	for (const auto& initialState : InitialStates)
	{
		bool addedAsFinal = false;
		auto it = Ce.find(initialState);
		if (it != Ce.end())
		{
			// Search the destinations for a FinalState
			for (const auto& transition : it->second)
			{
				if (FinalStates.find(transition.state) != FinalStates.end())
				{
					InitialEpsilonOutputs.insert(transition.output);
					// If there is a epsilon path from the initial state to a final one then the initial state should(and can be) final.
					if (!addedAsFinal)
					{
						FinalStates.insert(initialState);
						addedAsFinal = true;
					}
				}
			}
		}
	}

	// ReversedCe: { <r , <q, o>> | <q, <r, o> belongs to Ce }
	SetOfTransitionsWithOutputs ReversedCe;
	for (const auto& transitionsFromState : Ce)
	{
		const auto& state = transitionsFromState.first;
		for (const auto& transition : transitionsFromState.second)
		{
			ReversedCe[transition.state].insert(Transition{ state, transition.output });
		}
	}

	/* newDelta = { <q, <word, u+v+w>, r> |
											<q', <word, v>, r'> belongs to Delta & 
											word != epsilon &
											<q, u> belongs to Ce(r') &
											<r, w> belongs to ReversedCe(q') }
	*/
	DeltaType newDelta(Delta.size());
	for (size_t qPrim = 0, bound = Delta.size(); qPrim < bound; ++qPrim)
	{
		// TODO: use directly Delta!!!
		for (const auto& transitionsWithWord : Delta[qPrim])
		{
			const auto& word = transitionsWithWord.first; // Note: there are no epsilon words (already removed)
			for (const auto& transition : transitionsWithWord.second)
			{
				// transition.state is r';
				for (const auto& backwardTransition : ReversedCe[qPrim])
				{
					// backwardTransition.state is "q"
					for (const auto& forwardTransition : Ce[transition.state])
					{
						// forwardTransition.state is "r"
						// Can there be multiple transitions inserted???
						newDelta[backwardTransition.state][word].insert(Transition {
							forwardTransition.state,
							backwardTransition.output + transition.output + forwardTransition.output
						});
					}
				}
			}
		}
	}

	Delta = std::move(newDelta);
}

void FinalStateTransducer::MakeRealTime(bool& infinite)
{
	RemoveEpsilon();
	Expand();
	RemoveUpperEpsilon(infinite);
	Infinite = infinite;
	RealTime = !infinite; // If it is not an infinite then the conversion to real-time transducer was successful
}

// Removes the states which are not connected to the a initial state or a final state.
void FinalStateTransducer::Trim()
{
	// TODO: optimizations at few places
	SetOfTransitions r;
	Proj1_2(r);
	TransitiveClosure(r);

	std::unordered_set<size_t> reachableStates;
	// Add the initial states.
	for (const auto& initialStateIndex : InitialStates)
	{
		reachableStates.insert(initialStateIndex);
	}

	// Remove all states which are not connected to an initial one.
	for (const auto& initialStateIndex : InitialStates)
	{
		auto it = r.find(initialStateIndex);
		if (it != r.end())
		{
			for (const auto& reachableState : it->second)
			{
				reachableStates.insert(reachableState);
			}
		}
	}

	std::unordered_set<size_t> coReachableStates;
	// Add the final states.
	for (const auto& finalStateIndex : FinalStates)
	{
		coReachableStates.insert(finalStateIndex);
	}

	// Remove all states which are not connected to a final one.
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		auto it = r.find(i);
		if (it != r.end())
		{
			for (const auto& finalStateIndex : FinalStates)
			{
				auto ifFinal = it->second.find(finalStateIndex);
				if (ifFinal != it->second.end())
				{
					coReachableStates.insert(i);
					break;
				}
			}
		}
	}

	// Remove the non reachable or co-reachabe state transitions.
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		if (reachableStates.find(i) == reachableStates.end() || coReachableStates.find(i) == coReachableStates.end())
		{
			Delta[i].clear();
		}
	}

	// Update the Initial and Final states.
	for (const auto& initialStateIndex : InitialStates)
	{
		if (coReachableStates.find(initialStateIndex) == coReachableStates.end())
		{
			InitialStates.erase(initialStateIndex);
		}
	}
	for (const auto& finalStateIndex : FinalStates)
	{
		if (reachableStates.find(finalStateIndex) == reachableStates.end())
		{
			FinalStates.erase(finalStateIndex);
		}
	}

	// Remove the deleted states from the vector and remap the transitions
	const size_t REMOVED_STATE = size_t(-1);

	std::unordered_map<size_t, size_t> remapingOfStateIndexes;
	size_t newStateIndex = 0;
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		if (Delta[i].empty())
		{
			remapingOfStateIndexes[i] = REMOVED_STATE;
		}
		else
		{
			remapingOfStateIndexes[i] = newStateIndex;
			++newStateIndex;
		}
	}

	// "Squash" the elements in the vector.
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		const auto& newStateIndex = remapingOfStateIndexes[i];
		if (newStateIndex != REMOVED_STATE)
		{
			Delta[newStateIndex] = std::move(Delta[i]);
		}
	}

	// Now the @newStateIndex is the new vector size.
	Delta.resize(newStateIndex);

	std::unordered_set<Transition> updatedTransitions;
	// Update the transitions.
	for (auto& state : Delta)
	{
		for (auto& transitionsWithWord : state)
		{
			updatedTransitions.clear();
			for (const auto& transition : transitionsWithWord.second)
			{
				const auto& newDestinationStateIndex = remapingOfStateIndexes[transition.state];
				if (newDestinationStateIndex != REMOVED_STATE)
				{
					updatedTransitions.insert(Transition{ newDestinationStateIndex, transition.output });
				}
			}
			transitionsWithWord.second = std::move(updatedTransitions);
		}
	}
}

void FinalStateTransducer::Proj1_2(SetOfTransitions& r) const
{
	for (size_t i = 0, bound = Delta.size(); i < bound; ++i)
	{
		for (const auto& transitions : Delta[i])
		{
			for (const auto& transition : transitions.second)
			{

				r[i].insert(transition.state);
			}
		}
	}
}

void FinalStateTransducer::UpdateRecognizingEmptyWord()
{
	RecognizingEmptyWord = RealTime ?
		RealTimeIsRecognizingEmptyWord() :
		StandardIsRecognizingEmptyWord();
}

// Works only for one-symbol transducer(the transitions are only with one symbol or epsilon)
bool FinalStateTransducer::TraverseWithWord(const char* word, std::unordered_set<size_t>& outputs) const
{
	if (IsRealTime())
	{
		return RealTimeTraverseWithWord(word, outputs);
	}

	return StandardTrawerseWithWord(word, outputs);
}

bool FinalStateTransducer::StandardTrawerseWithWord(const char* word, std::unordered_set<size_t>& outputs) const
{
	if (!word) return false;
	const char* pWord = word;
#if defined(INFO)
	std::cout << "Standard Traversing with \"" << pWord << "\" word...\n";
#endif

	outputs.clear();
	char strTmp[] = "x";

	struct TraverseTransition
	{
		int state;
		size_t accumulatedOutput;
	};
	TraverseTransition BFSLevelSeparator{ -1, 0 };
	std::deque<TraverseTransition> q;

	q.push_back(BFSLevelSeparator);
	for (const auto& initialStateIndex : InitialStates)
	{
		q.push_back(TraverseTransition{ static_cast<int>(initialStateIndex), 0 }); // Fictial initial transition with the empty word and no output to each initial state.
	}

#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
	typedef std::tuple<size_t, size_t, size_t> VisitedPair; // .first is the source, .second is the destination state
	std::unordered_set<VisitedPair,
						boost::hash<VisitedPair>
						> visitedStateToStateWithEpsilon;
#endif
	while (*pWord)
	{
		TraverseTransition currTransition = q.front();
		q.pop_front();

		if (q.empty())
		{
			return false;
		}
		assert(currTransition.state == -1); // Only the level separator can have negative state's index.

		q.push_back(currTransition); // Move the separator to the "end".

#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
		visitedStateToStateWithEpsilon.clear();
#endif
		// Read all states at the 'next' level and add the posibile transitions.
		while (q.front().state != -1) // Untill the level separator.
		{
			currTransition = q.front();
			q.pop_front();
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
			if (StatesWithEpsilonCycleWithPositiveOutput.find(currTransition.state) != StatesWithEpsilonCycleWithPositiveOutput.end())
			{
#if defined(INFO)
				std::cout << "The word was with infinite ambiguous\n";
#endif
				return false;
			}
#endif

			// If the state has a transition with the symbol *word, then add it
			//TODO remove the "hack"
			strTmp[0] = *pWord;
			auto it = Delta[currTransition.state].find(strTmp);
			if (it != Delta[currTransition.state].end()) // Found a transition with *word symbol.
			{
				for (const auto& transition : it->second) // Add all found transitions to the next level, because we have read one more symbol.
				{
					q.push_back(TraverseTransition{ (int)transition.state, currTransition.accumulatedOutput + transition.output });
				}
			}

			it = Delta[currTransition.state].find("");
			if (it != Delta[currTransition.state].end()) // There are epsilon transitions from this state to others.
			{
				// TODO: check for epsilon cycle...
				for (const auto& transition : it->second) // Add them to the current level, because we have reached them withoud reading a symbol.
				{
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
					const auto accomulatedOutput = currTransition.accumulatedOutput + transition.output;
					const std::tuple<size_t, size_t, size_t> visitedPairWithEpsilon { currTransition.state, transition.state, accomulatedOutput };
					if (visitedStateToStateWithEpsilon.find(visitedPairWithEpsilon) != visitedStateToStateWithEpsilon.end())
					{
						continue;
					}
					visitedStateToStateWithEpsilon.insert(visitedPairWithEpsilon);
#endif
					q.push_front(TraverseTransition{ (int)transition.state, currTransition.accumulatedOutput + transition.output });
				}
			}
			assert(!q.empty());
		}
		++pWord;
	}

	if (q.empty())
	{
		std::cout << "\tSomething went wrong...\n";
		return false;
	}
	assert(q.front().state == -1);
	q.pop_front(); // Remove the level separator.

	// TODO the first one is the level separator...
	// Try to find an final state on the last reached
	std::unordered_set<size_t> accumulatedOutputs;
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
	visitedStateToStateWithEpsilon.clear();
#endif
	while (!q.empty())
	{
		const auto& currTransition = q.front();
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
		if (StatesWithEpsilonCycleWithPositiveOutput.find(currTransition.state) != StatesWithEpsilonCycleWithPositiveOutput.end())
		{
#if defined(INFO)
			std::cout << "The word was with infinite ambiguous\n";
#endif
			return false;
		}
#endif
		if (FinalStates.find(currTransition.state) != FinalStates.end())
		{
			accumulatedOutputs.insert(currTransition.accumulatedOutput);
		}

		const auto it = Delta[currTransition.state].find("");
		if (it != Delta[currTransition.state].end()) // There are epsilon transitions from this state to others which might be finils.
		{
			for (const auto& transition : it->second) // Add them to the current level, because we have reached them withoud reading a symbol.
			{
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
				const auto accomulatedOutput = currTransition.accumulatedOutput + transition.output;
				const std::tuple<size_t, size_t, size_t> visitedPairWithEpsilon{ currTransition.state, transition.state, accomulatedOutput };
				if (visitedStateToStateWithEpsilon.find(visitedPairWithEpsilon) != visitedStateToStateWithEpsilon.end())
				{
					continue;
				}
				visitedStateToStateWithEpsilon.insert(visitedPairWithEpsilon);
#endif
				q.push_back(TraverseTransition{ (int)transition.state, currTransition.accumulatedOutput + transition.output });
			}
		}

		q.pop_front();
	}

	if (!*word && RecognizingEmptyWord)
	{
		outputs = std::move(InitialEpsilonOutputs);
		outputs.insert(0);
	}
	else
	{
		outputs = std::move(accumulatedOutputs);
	}

	return !outputs.empty();
}

bool FinalStateTransducer::RealTimeTraverseWithWord(const char* word, std::unordered_set<size_t>& outputs) const
{
	if (!word) return false;
	const char* pWord = word;
#if defined(INFO)
	std::cout << "RealTime Traversing with \"" << pWord << "\" word...\n";
#endif

	char strTmp[] = "x";

	struct TraverseTransition
	{
		int state;
		size_t accumulatedOutput;
	};
	TraverseTransition BFSLevelSeparator{ -1, 0 };
	std::deque<TraverseTransition> q;

	q.push_back(BFSLevelSeparator);
	for (const auto& initialStateIndex : InitialStates)
	{
		q.push_back(TraverseTransition{ static_cast<int>(initialStateIndex), 0 }); // Fictial initial transition with the empty word and no output to each initial state.
	}

	while (*pWord)
	{
		TraverseTransition currTransition = q.front();
		q.pop_front();

		if (q.empty())
		{
			return false;
		}
		assert(currTransition.state == -1); // Only the level separator can have negative state's index.

		q.push_back(currTransition); // Move the separator to the "end".

		// Read all states at the 'next' level and add the posibile transitions.
		while (q.front().state != -1) // Untill the level separator.
		{
			currTransition = q.front();
			q.pop_front();

			// If the state has a transition with the symbol *word, then add it
			//TODO remove the "hack"
			strTmp[0] = *pWord;
			auto it = Delta[currTransition.state].find(strTmp);
			if (it != Delta[currTransition.state].end()) // Found a transition with *word symbol.
			{
				for (const auto& transition : it->second) // Add all found transitions to the next level, because we have read one more symbol.
				{
					q.push_back(TraverseTransition{ (int)transition.state, currTransition.accumulatedOutput + transition.output });
				}
			}

			assert(!q.empty());
		}
		++pWord;
	}

	assert(!q.empty());
	assert(q.front().state == -1);
	q.pop_front(); // Remove the level separator.

	std::unordered_set<size_t> accumulatedOutputs;
	while (!q.empty())
	{
		const auto& currTransition = q.front();
		if (FinalStates.find(currTransition.state) != FinalStates.end())
		{
			accumulatedOutputs.insert(currTransition.accumulatedOutput);
		}
		q.pop_front();
	}

	if (!*word && RecognizingEmptyWord)
	{
		outputs = std::move(InitialEpsilonOutputs);
		outputs.insert(0);
	}
	else
	{
		outputs = std::move(accumulatedOutputs);
	}

	return !outputs.empty();
}

bool FinalStateTransducer::GetRecognizingEmptyWord() const
{
	return RecognizingEmptyWord;
}

bool FinalStateTransducer::IsInfinite() const
{
	return Infinite;
}

bool FinalStateTransducer::IsRealTime() const
{
	return RealTime;
}

bool FinalStateTransducer::RealTimeIsRecognizingEmptyWord() const
{
	for (const auto& initialState : InitialStates)
	{
		if (FinalStates.find(initialState) != FinalStates.end())
		{
			return true;
		}
	}
	return false;
}

// Note if it is infinite ambiguous for the empty word then it will return false.
bool FinalStateTransducer::StandardIsRecognizingEmptyWord() const
{
	if (IsInfinite())
	{
		return false;
	}

	for (const auto& initialState : InitialStates)
	{
		auto it = CloseEpsilonOnStates.find(initialState);
		if (it != CloseEpsilonOnStates.end())
		{
			for (const auto transition : it->second)
			{
				if (FinalStates.find(transition.state) != FinalStates.end())
				{
					return true;
				}
			}
		}
	}
	return false;
}