#pragma once

#include <unordered_map>
#include <unordered_set>
#include <boost/functional/hash.hpp>

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

template<> struct std::hash<Transition>
{
	typedef Transition argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& s) const noexcept
	{
		return boost::hash<std::pair<std::size_t, std::size_t>>{}({ s.state, s.output }); // reusing the boost hash 
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
