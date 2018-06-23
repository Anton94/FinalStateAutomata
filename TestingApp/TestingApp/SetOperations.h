#pragma once

#include <unordered_map>
#include <unordered_set>

struct Transition
{
	size_t state; // The destination state(index in a vector)
	size_t output; // Some number which is the output on the second ?strip? of the transducer.

	bool operator==(const Transition& right) const
	{
		return state == right.state && output == right.output;
	}

	bool operator!=(const Transition& right) const
	{
		return !operator==(right);
	}
};

typedef std::unordered_map<size_t, std::unordered_set<size_t>> SetOfTransitions; // { (a, b) | "a is connected to b" } stored by the first component
typedef std::unordered_map<size_t, std::unordered_set<Transition>> SetOfTransitionsWithOutputs; // { (a, b, out) | "a is connected to b with output out" } stored by the first component

void TransitiveClosure(SetOfTransitions& r);

// Very similar to the transitive closure with accumulating outputs
// Additionaly infinity checking, i.e.
// if there is a cycle of type (a, <b, o>) ---> (b, <c, o'>) ---> ... ---> (z, <a, o'">)
// and the sum of all o, o', ... , o'" is not 0 then it is a non-trivial one, there could be infinite many outputs.
void ClosureEpsilon(SetOfTransitionsWithOutputs& r, bool& infinite);

void AddIdentity(SetOfTransitions& r);

void Print(const SetOfTransitions& r);
void Print(const SetOfTransitionsWithOutputs& r);
