#pragma once

#include <cassert>

// The idea is to add more things to it, e.g. saveiry.

void logMessage(const char* msg1, const char* msg2 = nullptr, const char* msg3 = nullptr, const char* msg4 = nullptr, const char* msg5 = nullptr, const char* msg6 = nullptr);

void LogAndAssert(bool error, const char* msg1, const char* msg2 = nullptr, const char* msg3 = nullptr, const char* msg4 = nullptr, const char* msg5 = nullptr, const char* msg6 = nullptr);
