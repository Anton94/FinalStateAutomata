#include <stdlib.h> // atoi
#include <iostream>
#include <deque>
#include "FinalStateTransducer.h"
#include "AssertLog.h"

FinalStateTransducer::FinalStateTransducer(char* regExpr, int separator, int length) // The regular expression should be of type: 'word:number'
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

	Delta[0][word].push_back(Transition{ 1, outputNumber }); // One transition: 0 -> 1 via word @word and output @outputNumber.
	FinalStates.insert(1); // 1 is a final state.
	InitialStates.insert(0); // q0 which is 0 is the initial state
}

void FinalStateTransducer::CloseStar()
{
	ClosePlus();
	FinalStates.insert(Delta.size() - 1); // Add the new state as a final,
}

void FinalStateTransducer::ClosePlus()
{
	// Make a new state with no transitions.
	Delta.push_back(StateTransitions());
	size_t newStateIndex = Delta.size() - 1;

	for (const auto& finalStateIndex : FinalStates)
	{
		Delta[finalStateIndex][""].push_back(Transition{ newStateIndex, 0 }); // Add "finalState --e:0--> newState" transition
	}

	MakeSingleInitialState(newStateIndex);
}

void FinalStateTransducer::Concat(FinalStateTransducer& right)
{
	right.RemapDelta(Delta.size());

	Delta.insert(Delta.end(),
		std::make_move_iterator(right.Delta.begin()),
		std::make_move_iterator(right.Delta.end()));

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


void FinalStateTransducer::Remap(int offset)
{
	RemapDelta(offset);
	RemapInitialStates(offset);
	RemapFinalStates(offset);
}

void FinalStateTransducer::RemapDelta(int offset)
{
	// I do not need this explicit remaping. TODO keep the offset 'per state' and recalculate "on the fly"?
	for (auto& state : Delta)
	{
		for (auto& transitions : state)
		{
			for (auto& transition : transitions.second)
			{
				transition.state += offset;
			}
		}
	}
}

void FinalStateTransducer::RemapInitialStates(int offset)
{
	std::unordered_set<size_t> newInitialStates;
	for (auto& stateIndex : InitialStates)
	{
		newInitialStates.insert(stateIndex + offset);
	}
	InitialStates.swap(newInitialStates);
}

void FinalStateTransducer::RemapFinalStates(int offset)
{
	std::unordered_set<size_t> newFinalStates;
	for (auto& stateIndex : FinalStates)
	{
		newFinalStates.insert(stateIndex + offset);
	}
	FinalStates.swap(newFinalStates);
}

void FinalStateTransducer::MoveRightInitialStatesIntoLeft(FinalStateTransducer& right, int offset)
{
	for (auto& intialStateIndex : right.InitialStates)
	{
		InitialStates.insert(intialStateIndex + offset);
	}
}

void FinalStateTransducer::MoveRightFinalStatesIntoLeft(FinalStateTransducer& right, int offset)
{
	for (auto& finalStateIndex : right.FinalStates)
	{
		FinalStates.insert(finalStateIndex + offset);
	}
}

void FinalStateTransducer::MakeSingleInitialState(int newInitialStateIndex)
{
	for (const auto& initialStateIndex : InitialStates)
	{
		Delta[newInitialStateIndex][""].push_back(Transition{ initialStateIndex, 0 }); // Add "newState --e:0--> initialState" transition
	}

	// Make the new state to be the initial one.
	InitialStates.clear();
	InitialStates.insert(newInitialStateIndex);
}

bool FinalStateTransducer::TraverseWithWord(const char* word, std::unordered_set<size_t>& outputs) const
{
	if (!word) return false;
	const char* pWord = word;
#if defined(INFO)
	std::cout << "Traversing with \"" << pWord << "\" word...\n";
#endif
	// TODO: check for epsilon cycles.. (inf outputs then for the word...)
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
#if defined(INFO)
			std::cout << "\tThe word is not from this regular expression.\n";
#endif
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

			it = Delta[currTransition.state].find("");
			if (it != Delta[currTransition.state].end()) // There are epsilon transitions from this state to others.
			{
				// TODO: check for epsilon cycle...
				for (const auto& transition : it->second) // Add them to the current level, because we have reached them withoud reading a symbol.
				{
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
	while (!q.empty())
	{
		const auto& currTransition = q.front();
		if (FinalStates.find(currTransition.state) != FinalStates.end())
		{
			accumulatedOutputs.insert(currTransition.accumulatedOutput);
		}

		const auto it = Delta[currTransition.state].find("");
		if (it != Delta[currTransition.state].end()) // There are epsilon transitions from this state to others which might be finils.
		{
			// TODO: check for epsilon cycle...
			for (const auto& transition : it->second) // Add them to the current level, because we have reached them withoud reading a symbol.
			{
				q.push_back(TraverseTransition{ (int)transition.state, currTransition.accumulatedOutput + transition.output });
			}
		}

		q.pop_front();
	}

	outputs.clear();
	outputs.swap(accumulatedOutputs);

	if (accumulatedOutputs.empty() && *word) // It is not an empty word
	{
#if defined(INFO)
		std::cout << "\tThe word is not from this regular expression.\n";
#endif
		return false;
	}

#if defined(INFO)
	std::cout << "END traversing.\n";
	for (const auto& accumulatedOutput : accumulatedOutputs)
	{
		std::cout << "\tAn output: " << accumulatedOutput << ".\n";
	}
#endif
	return true;
}
