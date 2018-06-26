#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "SetOperations.h"


/*
	Transducer(E*->N, Q, q0, F, Delta)
    The transducer is E*->N . Sygma(E) which is the alphabet into the natural number's monoid which is (N, 0, +).
    Represents a regular function

	The states are basically the vector for the delta, 0 is the starting state(q0).

	I will denote the transitions between states like that:
	q0 --word:number--> q1 ('e' will be epsilon, i.e. the empty word)
*/
class FinalStateTransducer
{
public:
	FinalStateTransducer(char* regExpr, int separator, int length); // The regular expression should be of type: 'word:number'

	void CloseStar();
	void ClosePlus();
	void Concat(FinalStateTransducer& right);
	void Union(FinalStateTransducer& right);

	void Remap(size_t offset);
	void RemapDelta(size_t offset);
	void RemapInitialStates(size_t offset);
	void RemapFinalStates(size_t offset);

	void MoveRightInitialStatesIntoLeft(FinalStateTransducer& right, size_t offset);
	void MoveRightFinalStatesIntoLeft(FinalStateTransducer& right, size_t offset);

	void MakeSingleInitialState(size_t newInitialStateIndex);

	// Only on a real-time transducer.
	bool TraverseWithWord(const char* word, std::unordered_set<size_t>& outputs) const;

	bool GetRecognizingEmptyWord() const;

	bool IsInfinite() const;
	bool IsRealTime() const;

	void MakeRealTime(bool& infinite);

	void UpdateRecognizingEmptyWord();
private:
	// Each transition to be with only single symbol(or epsilon)
	void Expand();

	// (e,0) transitions removing
	void RemoveEpsilon();

	// (e,X) transitions removing. When there is such transitions in the beginning (from the initial states) - they will be written to the @InitialEpsilonOutputs set.
	void RemoveUpperEpsilon(bool& infinite);

	bool RealTimeIsRecognizingEmptyWord() const;
	bool StandardIsRecognizingEmptyWord() const;

	bool StandardTrawerseWithWord(const char* word, std::unordered_set<size_t>& outputs) const;
	bool RealTimeTraverseWithWord(const char* word, std::unordered_set<size_t>& outputs) const;
private: // TODO: the key has to be something else, not a whole string!
	typedef std::unordered_map<std::string, std::unordered_set<Transition>>
		StateTransitions;
	typedef std::vector<StateTransitions> DeltaType;
	DeltaType Delta; // State at position 'i' with word 'w' will lead to state(s) ('state') (which are indexes in the Delta vector) with output ('output') some number.
	std::unordered_set<size_t> FinalStates;
	std::unordered_set<size_t> InitialStates;

	bool RecognizingEmptyWord;
	bool Infinite;
	bool RealTime;

	SetOfTransitionsWithOutputs CloseEpsilonOnStates;
	std::unordered_set<size_t> StatesWithEpsilonCycleWithPositiveOutput;
	std::unordered_set<size_t> InitialEpsilonOutputs;
private: // I do not want to copy this big structures, just to move them arround...
	//FinalStateTransducer(const FinalStateTransducer& other) = delete; // TODO Whyyyy not able....
	//FinalStateTransducer& operator=(const FinalStateTransducer& other) = delete;
};
