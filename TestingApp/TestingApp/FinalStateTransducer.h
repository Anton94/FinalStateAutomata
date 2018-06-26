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

	void Remap(unsigned offset);
	void RemapDelta(unsigned offset);
	void RemapInitialStates(unsigned offset);
	void RemapFinalStates(unsigned offset);

	void MoveRightInitialStatesIntoLeft(FinalStateTransducer& right, unsigned offset);
	void MoveRightFinalStatesIntoLeft(FinalStateTransducer& right, unsigned offset);

	void MakeSingleInitialState(unsigned newInitialStateIndex);

	// Works only for one-symbol transducer(the transitions are only with one symbol or epsilon)
	bool TraverseWithWord(const char* word, std::unordered_set<unsigned>& outputs) const;

	bool GetRecognizingEmptyWord() const;

	bool IsInfinite() const;
	bool IsRealTime() const;

	void MakeRealTime(bool& infinite);

	void UpdateRecognizingEmptyWord();
private:
	// Each transition to be with only single symbol(or epsilon)
	void Expand();

	// Removes the states which are not connected to the a initial state or a final state.
	void Trim();

	void Proj1_2(SetOfTransitions& r) const;
	void Proj1_23(SetOfTransitionsWithOutputs& r) const;

	void MakeSquaredOutputTransducer();

	// (e,0) transitions removing
	void RemoveEpsilon();

	// (e,X) transitions removing. When there is such transitions in the beginning (from the initial states) - they will be written to the @InitialEpsilonOutputs set.
	void RemoveUpperEpsilon(bool& infinite);

	bool RealTimeIsRecognizingEmptyWord() const;
	bool StandardIsRecognizingEmptyWord() const;

	bool StandardTrawerseWithWord(const char* word, std::unordered_set<unsigned>& outputs) const;
	bool RealTimeTraverseWithWord(const char* word, std::unordered_set<unsigned>& outputs) const;
private: // TODO: the key has to be something else, not a whole string!
	typedef std::unordered_map<std::string, std::unordered_set<Transition>>
		StateTransitions;
	typedef std::vector<StateTransitions> DeltaType;
	DeltaType Delta; // State at position 'i' with word 'w' will lead to state(s) ('state') (which are indexes in the Delta vector) with output ('output') some number.
	std::unordered_set<unsigned> FinalStates;
	std::unordered_set<unsigned> InitialStates;

	struct SquaredOutputTransducer
	{
		SetOfTransitionsWithOutputs Delta;
		std::unordered_set<unsigned> FinalStates;
		std::unordered_set<unsigned> InitialStates;
	} SOT;

	bool RecognizingEmptyWord;
	bool Infinite;
	bool RealTime;

	SetOfTransitionsWithOutputs CloseEpsilonOnStates;
	std::unordered_set<unsigned> StatesWithEpsilonCycleWithPositiveOutput;
	std::unordered_set<unsigned> InitialEpsilonOutputs;
private: // I do not want to copy this big structures, just to move them arround...
	//FinalStateTransducer(const FinalStateTransducer& other) = delete; // TODO Whyyyy not able....
	//FinalStateTransducer& operator=(const FinalStateTransducer& other) = delete;
};
