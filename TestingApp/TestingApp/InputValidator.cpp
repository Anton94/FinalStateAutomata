#include "InputValidator.h"
#include <vector>
#include <string>
#include "AssertLog.h"

bool checkInputCorrectness(const char* regExpr)
{
	std::string regExprCopy(regExpr);
	std::vector<char> dummyStack;

	int currLength, separatorAt = 0;
	char* pRegExpr = const_cast<char*>(regExprCopy.c_str()),
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
		separatorAt = -1;
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
			case '*': // Unary operation (pop & push result)
				if (dummyStack.size() < 1)
				{
					logMessage("Faild to do a Kleene star operation because there are not enough objects to apply.",
						" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
				}
				break;
			case '+': // Unary operation (pop & push result)
				if (dummyStack.size() < 1)
				{
					logMessage("Faild to do a Plus operation because there are not enough objects to apply.",
						" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
					return false;
				}
				break;
			case '|': // Binary operation (pop pop & push result)
				if (dummyStack.size() < 2)
				{
					logMessage("Faild to do a Union operation because there are not enough objects to apply.",
						" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
					return false;
				}

				dummyStack.pop_back();
				break;
			case '.': // Binary operation (pop pop & push result)
				if (dummyStack.size() < 2)
				{
					logMessage("Faild to do a Concatenation operation because there are not enough objects to apply.",
						" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
					return false;
				}
				dummyStack.pop_back();
				break;
			default:
				logMessage("Symbol '", std::string(pCurrStart, 1).c_str(), "' was not recognized as an operation."
					" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
				return false;
			}
		}
		else
		{
			if (separatorAt == -1)
			{
				logMessage("Missing separator while parsing an expression \"", std::string(pCurrStart, currLength).c_str(), "\"",
					" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
				return false;
			}
			if (separatorAt == currLength - 1)
			{
				logMessage("Missing output value after the separator(:) while parsing an expression \"", std::string(pCurrStart, currLength).c_str(), "\"",
					" Failed at position ", std::to_string(pCurrStart - pRexExprStart).c_str(), ".");
				return false;
			}

			dummyStack.push_back(1);
		}
	}
	if (dummyStack.size() > 1)
	{

		logMessage("There are more than one objects left to apply operations to.");
		return false;
	}
	if (dummyStack.size() < 1)
	{

		logMessage("There are no constructed objects.");
		return false;
	}
	return true;
}