#pragma once

#include "FinalStateTransducer.h"

/*
	Constructs a final state transducer E*->N from a regular expression.

	'*' is a Kleene star
	'+' is a plus
	'.' is a concatenation
	'|' is a union

	constructs from a string which represents the regular expression in a reversed polish notation.
*/

class RegularFinalStateTransducerBuilder
{
public:
	// The string should be correct, here doesn't make almost no checks for correctness.
	void build(const char* regularExpression);
private:
	void executeStarOperation();
	void executePlusOperation();
	void executeConcatOperation();
	void executeUnionOperation();

	FinalStateTransducer transducer;
	std::vector<FinalStateTransducer> stack;
};
