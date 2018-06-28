#include <stdlib.h> // atoi
#include <iostream>
#include <deque>
#include <unordered_set>
#include <queue>
#include <boost/functional/hash.hpp>
#include "FinalStateTransducer.h"
#include "AssertLog.h"

FinalStateTransducer::FinalStateTransducer(char* regExpr, int separator, int length) // The regular expression should be of type: 'word:number'
	: RecognizingEmptyWord(false)
	, Infinite(false)
	, RealTime(false)
	, Functional(false)
{
	char* word = regExpr;
	*(regExpr + separator) = '\0';
	*(regExpr + length) = '\0';
	const char* pOutputNumber = regExpr + separator + 1; // + ':'.
	unsigned outputNumber = atoi(pOutputNumber);
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
	FinalStates.insert((unsigned) Delta.size() - 1); // Add the new state as a final,
}

void FinalStateTransducer::ClosePlus()
{
	// Make a new state with no transitions.
	Delta.push_back(StateTransitions());
	unsigned newStateIndex = (unsigned) Delta.size() - 1;

	for (const auto& finalStateIndex : FinalStates)
	{
		Delta[finalStateIndex][""].insert(Transition{ newStateIndex, 0 }); // Add "finalState --e:0--> newState" transition
	}

	MakeSingleInitialState(newStateIndex);
}

void FinalStateTransducer::Concat(FinalStateTransducer& right)
{
	right.Remap((unsigned) Delta.size());

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
	unsigned offset = (unsigned) Delta.size();
	right.RemapDelta(offset);

	Delta.insert(Delta.end(),
		std::make_move_iterator(right.Delta.begin()),
		std::make_move_iterator(right.Delta.end()));

	MoveRightInitialStatesIntoLeft(right, offset);
	MoveRightFinalStatesIntoLeft(right, offset);
}


void FinalStateTransducer::Remap(unsigned offset)
{
	RemapDelta(offset);
	RemapInitialStates(offset);
	RemapFinalStates(offset);
}

void FinalStateTransducer::RemapDelta(unsigned offset)
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

void FinalStateTransducer::RemapInitialStates(unsigned offset)
{
	std::unordered_set<unsigned> newInitialStates;
	for (auto& stateIndex : InitialStates)
	{
		newInitialStates.insert(stateIndex + offset);
	}
	InitialStates = std::move(newInitialStates);
}

void FinalStateTransducer::RemapFinalStates(unsigned offset)
{
	std::unordered_set<unsigned> newFinalStates;
	for (auto& stateIndex : FinalStates)
	{
		newFinalStates.insert(stateIndex + offset);
	}
	FinalStates = std::move(newFinalStates);
}

void FinalStateTransducer::MoveRightInitialStatesIntoLeft(FinalStateTransducer& right, unsigned offset)
{
	for (auto& intialStateIndex : right.InitialStates)
	{
		InitialStates.insert(intialStateIndex + offset);
	}
}

void FinalStateTransducer::MoveRightFinalStatesIntoLeft(FinalStateTransducer& right, unsigned offset)
{
	for (auto& finalStateIndex : right.FinalStates)
	{
		FinalStates.insert(finalStateIndex + offset);
	}
}

void FinalStateTransducer::MakeSingleInitialState(unsigned newInitialStateIndex)
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
					unsigned newStateIndex = (unsigned) currStatesCount;

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
void SeparateEpsilonTransitions(std::unordered_set<Transition>& v, unsigned output, std::unordered_set<unsigned>& s)
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
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
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
	// Add identity
	//for (unsigned i = 0u; i < Delta.size(); ++i)
	//	Ce[i].insert(i);

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
	std::unordered_set<unsigned> newInitialStates = InitialStates;
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
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
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
	for (unsigned qPrim = 0, bound = (unsigned) Delta.size(); qPrim < bound; ++qPrim)
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

bool FinalStateTransducer::MakeRealTime()
{
	RemoveEpsilon();
	Expand();
	RemoveUpperEpsilon(Infinite);
	RealTime = !Infinite; // If it is not an infinite then the conversion to real-time transducer was successful
	return Infinite;
}

void distance(const Outputs& l, const Outputs& r, Outputs& res)
{
	res.first = l.first + r.first;
	res.second = l.second + r.second;

	auto common = 0u;
	if (res.first > res.second)
	{
		common = res.second;
	}
	else
	{
		common = res.first;
	}

	res.first -= common;
	res.second -= common;
}

bool balancable(const Outputs& o)
{
	return o.first == 0 || o.second == 0;
}

bool zero(const Outputs& o)
{
	return o.first == 0 && o.second == 0;
}

bool FinalStateTransducer::TestForFunctionality()
{
	Functional = false;
	if (IsInfinite() || !IsRealTime() || InitialEpsilonOutputs.size() > 1)
	{
		return false;
	}

	MakeSquaredOutputTransducer();
	SOT.Trim();

	SetOfTransitionsWithPairedOutputs SOTDeltaO;
	SOT.Proj1_23(SOTDeltaO);

	std::unordered_map<unsigned, Outputs> AdmForLookups(SOT.InitialStates.size()); // For a state it gives the holden outpus
	std::vector<unsigned> AdmStatesForIteration; // For a state it gives the holden outpus
	AdmStatesForIteration.reserve(SOT.InitialStates.size());

	// Insert: I x {0, 0}
	for (const auto& initialStateIndex : SOT.InitialStates)
	{
		AdmForLookups[initialStateIndex] = { 0, 0 };
		AdmStatesForIteration.push_back(initialStateIndex);
	}

	for (size_t i = 0; i < AdmStatesForIteration.size(); ++i)
	{
		auto curr = AdmForLookups.find(AdmStatesForIteration[i]);
		assert(curr != AdmForLookups.end());

		const auto& q = curr->first; // state
		const auto& h = curr->second; // (o1, o2) holden output

		std::vector<StateAndOutputs> Dq;

		auto it = SOTDeltaO.find(q);
		if (it != SOTDeltaO.end())
		{
			for (const auto& transition : it->second)
			{
				// transition.first is state.
				// transition.second is a pair of holded outputs.
				Outputs newHoldenOutput;
				distance(h, transition.second, newHoldenOutput);
				StateAndOutputs newStateAndHoldenOutputs { transition.first, std::move(newHoldenOutput) };
				if (newStateAndHoldenOutputs.first != q || newStateAndHoldenOutputs.second != h)
				{
					Dq.push_back(std::move(newStateAndHoldenOutputs));
				}
			}
		}

		if (Dq.size() > 0)
		{
			for (const auto& qAndH : Dq)
			{
				const auto& qPrim = qAndH.first;
				const auto& hPrim = qAndH.second;
				if (!balancable(hPrim) ||
					(SOT.FinalStates.find(qPrim) != SOT.FinalStates.end() && !zero(hPrim)))
				{
					return false;
				}

				auto it = AdmForLookups.find(qPrim);
				if (it != AdmForLookups.end() && hPrim != it->second)
				{
					return false;
				}
				if (it == AdmForLookups.end())
				{
					AdmForLookups[qPrim] = std::move(hPrim);
					AdmStatesForIteration.push_back(qPrim);
				}
			}
		}
	}

	return Functional = true;
}

// Removes the states which are not connected to the a initial state or a final state.
void FinalStateTransducer::SquaredOutputTransducer::Trim()
{
	// TODO: optimizations at few places
	SetOfTransitions r;
	Proj1_2(r);
	SetOfTransitions reversedR;
	for (const auto& trs : r)
	{
		const auto& a = trs.first;
		for (const auto& tr : trs.second)
		{
			reversedR[tr].insert(a);
		}
	}

	/* Traverse to find the reachable states. */


	std::unordered_set<unsigned> visited;
	std::queue<unsigned> q;
	std::unordered_set<unsigned> reachableStates;
	// Add the initial states.
	for (const auto& initialStateIndex : InitialStates)
	{
		reachableStates.insert(initialStateIndex);
		q.push(initialStateIndex);
		visited.insert(initialStateIndex);
	}
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop();

		for (auto& reachableState : r[curr])
		{
			if (visited.find(reachableState) == visited.end())
			{
				reachableStates.insert(reachableState);
				q.push(reachableState);
				visited.insert(reachableState);
			}
		}
	}
	
	q = std::queue<unsigned>(); // not needed
	visited.clear();

	std::unordered_set<unsigned> coReachableStates;
	// Add the final states.
	for (const auto& finalStateIndex : FinalStates)
	{
		coReachableStates.insert(finalStateIndex);
		q.push(finalStateIndex);
		visited.insert(finalStateIndex);
	}
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop();

		for (auto& coReachableState : reversedR[curr])
		{
			if (visited.find(coReachableState) == visited.end())
			{
				coReachableStates.insert(coReachableState);
				q.push(coReachableState);
				visited.insert(coReachableState);
			}
		}
	}

// END
	//std::unordered_set<unsigned> reachableStates;
	//// Add the initial states.
	//for (const auto& initialStateIndex : InitialStates)
	//{
	//	reachableStates.insert(initialStateIndex);
	//}

	//TransitiveClosure(r);
	//// Remove all states which are not connected to an initial one.
	//for (const auto& initialStateIndex : InitialStates)
	//{
	//	auto it = r.find(initialStateIndex);
	//	if (it != r.end())
	//	{
	//		for (const auto& reachableState : it->second)
	//		{
	//			reachableStates.insert(reachableState);
	//		}
	//	}
	//}
	//
	//std::unordered_set<unsigned> coReachableStates;
	//// Add the final states.
	//for (const auto& finalStateIndex : FinalStates)
	//{
	//	coReachableStates.insert(finalStateIndex);
	//}
	
	//// Remove all states which are not connected to a final one.
	//for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
	//{
	//	auto it = r.find(i);
	//	if (it != r.end())
	//	{
	//		for (const auto& finalStateIndex : FinalStates)
	//		{
	//			auto ifFinal = it->second.find(finalStateIndex);
	//			if (ifFinal != it->second.end())
	//			{
	//				coReachableStates.insert(i);
	//				break;
	//			}
	//		}
	//	}
	//}
	//
	const unsigned REMOVED_STATE = unsigned(-1);
	std::unordered_map<unsigned, unsigned> remapingOfStateIndexes;
	// Remove the non reachable or co-reachabe state transitions.
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
	{
		if (reachableStates.find(i) == reachableStates.end() || coReachableStates.find(i) == coReachableStates.end())
		{
			remapingOfStateIndexes[i] = REMOVED_STATE;
		}
	}
	
	// Update the Initial and Final states.
	for (auto it = InitialStates.begin(); it != InitialStates.end(); )
	{
		if (coReachableStates.find(*it) == coReachableStates.end())
		{
			remapingOfStateIndexes[*it] = REMOVED_STATE;
			it = InitialStates.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto it = FinalStates.begin(); it != FinalStates.end(); )
	{
		if (reachableStates.find(*it) == reachableStates.end())
		{
			remapingOfStateIndexes[*it] = REMOVED_STATE;
			it = FinalStates.erase(it);
		}
		else
		{
			++it;
		}
	}
	
	// Remove the deleted states from the vector
	unsigned newStateIndex = 0;
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
	{
		auto it = remapingOfStateIndexes.find(i);
		if (it != remapingOfStateIndexes.end())
		{
			assert(it->second == REMOVED_STATE);
			Delta[i].clear();
		}
		else
		{
			remapingOfStateIndexes[i] = newStateIndex;
			++newStateIndex;
		}
	}
	
	// The final states might not have transitions from them, but they can be reachable.
	for (const auto& finalStateIndex : FinalStates)
	{
		if (remapingOfStateIndexes.find(finalStateIndex) == remapingOfStateIndexes.end())
		{
			remapingOfStateIndexes[finalStateIndex] = finalStateIndex;
		}
	}
	// Remap the initial and final states
	std::unordered_set<unsigned> remapedInitialStates;
	for (const auto& initialStateIndex : InitialStates)
	{
		assert(remapingOfStateIndexes.find(initialStateIndex) != remapingOfStateIndexes.end());
		const auto mapedInitialStateIndex = remapingOfStateIndexes[initialStateIndex];
		if (mapedInitialStateIndex != REMOVED_STATE)
		{
			remapedInitialStates.insert(mapedInitialStateIndex);
		}
	}
	InitialStates = std::move(remapedInitialStates);
	
	std::unordered_set<unsigned> remapedFinalStates;
	for (const auto& finalStateIndex : FinalStates)
	{
		assert(remapingOfStateIndexes.find(finalStateIndex) != remapingOfStateIndexes.end());
		const auto mapedFinalStateIndex = remapingOfStateIndexes[finalStateIndex];
		if (mapedFinalStateIndex != REMOVED_STATE)
		{
			remapedFinalStates.insert(mapedFinalStateIndex);
		}
	}
	FinalStates = std::move(remapedFinalStates);
	
	// "Squash" the elements in the vector.
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
	{
		assert(remapingOfStateIndexes.find(i) != remapingOfStateIndexes.end());
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
				auto it = remapingOfStateIndexes.find(transition.state);
				if (it != remapingOfStateIndexes.end())
				{
					const auto& newDestinationStateIndex = it->second;
					if (newDestinationStateIndex != REMOVED_STATE)
					{
						updatedTransitions.insert(Transition{ newDestinationStateIndex, transition.output });
					}
				}
			}
			transitionsWithWord.second = std::move(updatedTransitions);
		}
	}
}

void FinalStateTransducer::SquaredOutputTransducer::Proj1_2(SetOfTransitions& r) const
{
	for (unsigned i = 0, bound = (unsigned)Delta.size(); i < bound; ++i)
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

void FinalStateTransducer::SquaredOutputTransducer::Proj1_23(SetOfTransitionsWithPairedOutputs& r) const
{
	for (unsigned i = 0, bound = (unsigned)Delta.size(); i < bound; ++i)
	{
		for (const auto& transitions : Delta[i])
		{
			for (const auto& transition : transitions.second)
			{
				r[i].insert(StateAndOutputs{ transition.state, Outputs { transitions.first, transition.output} });
			}
		}
	}
}

void FinalStateTransducer::Proj1_2(SetOfTransitions& r) const
{
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
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

void FinalStateTransducer::Proj1_23(SetOfTransitionsWithOutputs& r) const
{
	for (unsigned i = 0, bound = (unsigned) Delta.size(); i < bound; ++i)
	{
		for (const auto& transitions : Delta[i])
		{
			for (const auto& transition : transitions.second)
			{
				r[i].insert(Transition{ transition.state, transition.output });
			}
		}
	}
}

void FinalStateTransducer::MakeSquaredOutputTransducer()
{
	assert(IsRealTime());
	assert(!IsInfinite());

	typedef std::pair<unsigned, unsigned> P; // <p1, p2>

	size_t initialIxIElementsCount = InitialStates.size() * InitialStates.size();
	std::vector<P> pForIteration;
	pForIteration.reserve(initialIxIElementsCount);
	std::unordered_map<P, unsigned,	boost::hash<P>> // <p1, p2>, id (it's place in the iteration vector)
		pForLookups(initialIxIElementsCount);

	unsigned id = 0;
	for (const auto& initialStateIndex1 : InitialStates)
	{
		for (const auto& initialStateIndex2 : InitialStates)
		{
			pForIteration.push_back(P{ initialStateIndex1, initialStateIndex2 });
			pForLookups[P{ initialStateIndex1, initialStateIndex2 }] = id;
			++id;
		}
	}

	typedef std::pair<std::pair<unsigned, unsigned>, std::pair<unsigned, unsigned>>	// < <o1, o2>, <q1, q2>>
		PairOutputsAndStates;

	SOT.Delta.clear();
	SOT.Delta.reserve(initialIxIElementsCount); // Tiny posible optimization.
	for (unsigned i = 0; i < pForIteration.size(); ++i)
	{
		const auto& p = pForIteration[i]; // a pair <p1, p2>
		if (p.first < Delta.size() && p.second < Delta.size())
		{
			const auto& D1Map = Delta[p.first];
			const auto& D2Map = Delta[p.second];

			std::unordered_set<PairOutputsAndStates, boost::hash<PairOutputsAndStates>> N;
			for (const auto& wordAndTransitions : D1Map)
			{
				// Only couple them by same transition word.
				const auto& D1 = wordAndTransitions.second;
				auto it = D2Map.find(wordAndTransitions.first);
				if (it != D2Map.end())
				{
					const auto& D2 = it->second;
					for (const auto& tr1 : D1)
					{
						for (const auto& tr2 : D2)
						{
							N.insert(PairOutputsAndStates{ { tr1.output, tr2.output },{ tr1.state, tr2.state } }); // Inserting  < <o1, o2>, <q1, q2>>
						}
					}
				}
			}

			// Add the new pair of states.
			for (const auto& pairOutputsAndStates : N)
			{
				const auto& p = pairOutputsAndStates.second;
				const auto it = pForLookups.find(p);
				if (it == pForLookups.end())
				{
					pForLookups[p] = (unsigned)pForIteration.size(); // The number of 'p' in the vector
					pForIteration.push_back(p);
				}
			}

			// Update the Squared FST's delta by adding a new state and it's transitions.
			SquaredOutputTransducer::SOTStateTransitions newStateWithTransitions;
			for (const auto& pairOutputsAndStates : N)
			{
				const auto& o = pairOutputsAndStates.first;
				const auto& p = pairOutputsAndStates.second;
				const auto it = pForLookups.find(p);
				assert(it != pForLookups.end());

				// add transition <k, o1, #p, o2> ; it->second is the #p and k is the new state.
				newStateWithTransitions[o.first].insert(Transition{ it->second, o.second });
			}
			SOT.Delta.push_back(std::move(newStateWithTransitions));
		}
	}

	SOT.FinalStates.clear();
	SOT.InitialStates.clear();
	for (unsigned i = 0, bound = (unsigned) pForIteration.size(); i < bound; ++i)
	{
		const auto& p = pForIteration[i];

		// Update the initial states.
		if (InitialStates.find(p.first) != InitialStates.end() &&
			InitialStates.find(p.second) != InitialStates.end())
		{
			SOT.InitialStates.insert(i);
		}

		// Update the final states.
		if (FinalStates.find(p.first) != FinalStates.end() &&
			FinalStates.find(p.second) != FinalStates.end())
		{
			SOT.FinalStates.insert(i);
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
bool FinalStateTransducer::TraverseWithWord(const char* word, std::unordered_set<unsigned>& outputs) const
{
	if (IsRealTime())
	{
		return RealTimeTraverseWithWord(word, outputs);
	}

	return StandardTrawerseWithWord(word, outputs);
}

bool FinalStateTransducer::StandardTrawerseWithWord(const char* word, std::unordered_set<unsigned>& outputs) const
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
		unsigned accumulatedOutput;
	};
	TraverseTransition BFSLevelSeparator{ -1, 0 };
	std::deque<TraverseTransition> q;

	q.push_back(BFSLevelSeparator);
	for (const auto& initialStateIndex : InitialStates)
	{
		q.push_back(TraverseTransition{ static_cast<int>(initialStateIndex), 0 }); // Fictial initial transition with the empty word and no output to each initial state.
	}

#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
	typedef std::tuple<unsigned, unsigned, unsigned> VisitedPair; // .first is the source, .second is the destination state
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
			if (StatesWithEpsilonCycleWithPositiveOutput.find(currTransition.state) != StatesWithEpsilonCycleWithPositiveOutput.end())
			{
#if defined(INFO)
				std::cout << "The word was with infinite ambiguous\n";
#endif
				return false;
			}

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
				for (const auto& transition : it->second) // Add them to the current level, because we have reached them withoud reading a symbol.
				{
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
					const auto accomulatedOutput = currTransition.accumulatedOutput + transition.output;
					const std::tuple<unsigned, unsigned, unsigned> visitedPairWithEpsilon { currTransition.state, transition.state, accomulatedOutput };
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
	std::unordered_set<unsigned> accumulatedOutputs;
#if defined (GUARD_FROM_EPSILON_CYCLE_ON_TRAVERSING)
	visitedStateToStateWithEpsilon.clear();
#endif
	while (!q.empty())
	{
		const auto& currTransition = q.front();
		if (StatesWithEpsilonCycleWithPositiveOutput.find(currTransition.state) != StatesWithEpsilonCycleWithPositiveOutput.end())
		{
#if defined(INFO)
			std::cout << "The word was with infinite ambiguous\n";
#endif
			return false;
		}
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
				const std::tuple<unsigned, unsigned, unsigned> visitedPairWithEpsilon{ currTransition.state, transition.state, accomulatedOutput };
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

bool FinalStateTransducer::RealTimeTraverseWithWord(const char* word, std::unordered_set<unsigned>& outputs) const
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
		unsigned accumulatedOutput;
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

	std::unordered_set<unsigned> accumulatedOutputs;
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

bool FinalStateTransducer::IsFunctional() const
{
	return Functional;
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