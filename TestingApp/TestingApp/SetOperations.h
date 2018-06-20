#pragma once

#include <unordered_map>
#include <unordered_set>

typedef std::unordered_map<size_t, std::unordered_set<size_t>> SetOfTransitions; // { (a, b) | "a is connected to b" } stored by the first component

void TransitiveClosure(SetOfTransitions& R);