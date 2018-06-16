#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>


/*
	Transducer(E*->N, Q, q0, F, Delta)
    The transducer is E*->N . Sygma(E) which is the alphabet into the natural number's monoid which is (N, 0, +).
    Represents a regular function

	The states are basically the vector for the delta, 0 is the starting state(q0).

	I will denote the transitions between states like that:
	q0 --word:number--> q1 ('e' will be epsilon, i.e. the empty word)
*/

struct Transition
{
	size_t state; // The destination state(index in a vector)
	size_t output; // Some number which is the output on the second ?strip? of the transducer.
};

class FinalStateTransducer
{
public:
	FinalStateTransducer(char* regExpr, int separator, int length); // The regular expression should be of type: 'word:number'

	void CloseStar();
	void ClosePlus();
	void Concat(FinalStateTransducer& right);
	void Union(FinalStateTransducer& right);

	void Remap(int offset);
	void RemapDelta(int offset);
	void RemapInitialStates(int offset);
	void RemapFinalStates(int offset);

	void MoveRightInitialStatesIntoLeft(FinalStateTransducer& right, int offset);
	void MoveRightFinalStatesIntoLeft(FinalStateTransducer& right, int offset);

	void MakeSingleInitialState(int newInitialStateIndex);

	// Only on a real-time transducer.
	bool FinalStateTransducer::TraverseWithWord(const char* word, std::unordered_set<size_t>& outputs) const;
private: // TODO: the key has to be something else, not a whole string!
	typedef std::unordered_map<std::string, std::vector<Transition>>
		StateTransitions;
	std::vector<StateTransitions> Delta; // State at position 'i' with word 'w' will lead to state(s) ('state') (which are indexes in the Delta vector) with output ('output') some number.
	std::unordered_set<size_t> FinalStates;
	std::unordered_set<size_t> InitialStates;

private: // I do not want to copy this big structures, just to move them arround...
	//FinalStateTransducer(const FinalStateTransducer& other) = delete; // TODO Whyyyy not able....
	FinalStateTransducer& operator=(const FinalStateTransducer& other) = delete;
};
