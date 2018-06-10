#include <iostream>
#include "AssertLog.h"

void logMessage(const char* msg1, const char* msg2, const char* msg3, const char* msg4, const char* msg5, const char* msg6)
{
	std::cout << msg1;
	if (msg2)
		std::cout << msg2;
	if (msg3)
		std::cout << msg3;
	if (msg4)
		std::cout << msg4;
	if (msg5)
		std::cout << msg5;
	if (msg6)
		std::cout << msg6;
	std::cout << std::endl;
}

void LogAndAssert(bool error, const char* msg1, const char* msg2, const char* msg3, const char* msg4, const char* msg5, const char* msg6)
{
	if (!error)
	{
		logMessage(msg1, msg2, msg3, msg4, msg5, msg6);
		assert(false);
	}
}