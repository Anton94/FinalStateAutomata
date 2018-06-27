#pragma once

#include <iostream>
#include <string>

void GenerateCustomWordConcatenationsAndIncreasingOutputs(std::string& word, unsigned len)
{
	std::cout << "";
	for (auto i = 1u; i <= len; ++i)
	{
		std::cout << word << ":" << i << " ";
	}

	for (auto i = 1u; i <= len - 1; ++i)
	{
		std::cout << ". ";
	}
	std::cout << "\n";
	std::cout << "Expected only word for recognizing:\n";
	for (auto i = 1u; i <= len; ++i)
	{
		std::cout << word;
	}
	std::cout << "\n";
	std::cout << "Expected output value: " << len * (len + 1) / 2 << "\n";
}