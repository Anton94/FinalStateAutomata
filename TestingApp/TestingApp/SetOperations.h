#pragma once

#include <unordered_map>
#include <unordered_set>

struct Transition
{
	size_t state; // The destination state(index in a vector)
	size_t output; // Some number which is the output on the second ?strip? of the transducer.
};


typedef std::unordered_map<size_t, std::unordered_set<size_t>> SetOfTransitions; // { (a, b) | "a is connected to b" } stored by the first component

void TransitiveClosure(SetOfTransitions& r);

void AddIdentity(SetOfTransitions& r);

void Print(const SetOfTransitions& r);
