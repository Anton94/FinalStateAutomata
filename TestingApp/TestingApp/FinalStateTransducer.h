#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>


/*
	Transducer(E*->N, Q, q0, F, Delta)
    The transducer is E*->N . Sygma(E) which is the alphabet into the natural number's monoid which is (N, 0, +).
    Represents a regular function

	The states are basically the vector for the delta, 0 is the starting state(q0).
*/

struct Transition
{
	int state; // The destination state(index in a vector)
	int output; // Some number which is the output on the second ?strip? of the transducer.
};

class FinalStateTransducer
{
public:
	FinalStateTransducer(char* regExpr, int separator, int length); // The regular expression should be of type: 'word:number'

	void CloseStar();
	void ClosePlus();
	void Concat(const FinalStateTransducer& right);
	void Union(const FinalStateTransducer& right);

private:
	std::vector<std::unordered_map<const char*, std::vector<Transition>>> Delta; // State at position 'i' with word 'w' will lead to state(s) ('state') (which are indexes in the Delta vector) with output ('output') some number.
	std::unordered_set<int> FinalStates;

private: // I do not want to copy this big structures, just to move them arround...
	//FinalStateTransducer(const FinalStateTransducer& other) = delete;
	FinalStateTransducer& operator=(const FinalStateTransducer& other) = delete;
};
