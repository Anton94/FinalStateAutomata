#pragma once

#include <vector>
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
	RegularFinalStateTransducerBuilder(const char* regExpr);
	~RegularFinalStateTransducerBuilder() { delete regExprHolder; }

	FinalStateTransducer* GetBuildedTransducer() { return transducer; }
private:
	void build();
	void executeStarOperation();
	void executePlusOperation();
	void executeConcatOperation();
	void executeUnionOperation();

	FinalStateTransducer* transducer; // TODO move it here!!!! (in a variable)
	std::vector<FinalStateTransducer> stack;
	char* regExprHolder;
private:
	RegularFinalStateTransducerBuilder(const RegularFinalStateTransducerBuilder& other) = delete;
	RegularFinalStateTransducerBuilder& operator=(const RegularFinalStateTransducerBuilder& other) = delete;
};
