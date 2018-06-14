#include <string>
#include <iostream>
#include "RegularFinalStateTransducerBuilder.h"
#include "AssertLog.h"

RegularFinalStateTransducerBuilder::RegularFinalStateTransducerBuilder(const char* regExpr)
	: transducer(nullptr)
{
	auto regExprLen = std::strlen(regExpr);
	regExprHolder = new (std::nothrow) char[regExprLen + 1];
	if (!regExprHolder)
	{
		std::cout << "not enough memory...";
	}

	std::memcpy(regExprHolder, regExpr, regExprLen + 1);
	build();
}

void RegularFinalStateTransducerBuilder::build()
{
	//stack.reserve(regExpr.length() / 4); // Some 'random' optimization...
	std::cout << "Buidling a final state transudcer with regular expression \"" << regExprHolder << "\".\n";
	int currLength, separatorAt;
	char* pRegExpr = regExprHolder,
		*pCurrStart;
	const char* const pRexExprStart = pRegExpr;
	while (*pRegExpr)
	{
		while (*pRegExpr == ' ')
		{
			++pRegExpr;
		}
		if (!*pRegExpr)
		{
			break;
		}

		pCurrStart = pRegExpr;
		currLength = 0;
		separatorAt = 0;
		while (*pRegExpr && *pRegExpr != ' ')
		{
			if (*pRegExpr == ':')
			{
				separatorAt = currLength;
			}
			++currLength;
			++pRegExpr;
		}
		if (currLength == 1)
		{
			switch (*pCurrStart)
			{
			case '*':
				std::cout << "Star operation\n";
				executeStarOperation();
				break;
			case '+':
				executePlusOperation();
				break;
			case '|':
				executeUnionOperation();
				break;
			case '.':
				executeConcatOperation();
				break;
			default:
				LogAndAssert(false, "Failed when parsing a single symbol from the input. Make sure that the operational symbols you used are correct.",
					" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str());
				return;
			}
		}
		else
		{
			++pRegExpr;
			stack.push_back(FinalStateTransducer(pCurrStart, separatorAt, currLength));
		}
	}

	LogAndAssert(stack.size() == 1,
		(stack.size() > 1 ? "There are more than one objects left to apply operations to."
			: "There are no constructed objects."))	;
	transducer = &stack[0];
	std::cout << "Done buidling it.\n";
}

void RegularFinalStateTransducerBuilder::executeStarOperation()
{
	assert(stack.size() >= 1);
	stack.back().CloseStar();
}
void RegularFinalStateTransducerBuilder::executePlusOperation()
{
	assert(stack.size() >= 1);
	stack.back().ClosePlus();
}
void RegularFinalStateTransducerBuilder::executeConcatOperation()
{
	assert(stack.size() >= 2);
	auto last = stack.size() - 1;
	stack[last - 1].Concat(stack[last]);
	stack.pop_back();
}
void RegularFinalStateTransducerBuilder::executeUnionOperation()
{
	assert(stack.size() >= 2);
	auto last = stack.size() - 1;
	stack[last - 1].Union(stack[last]);
	stack.pop_back();
}
