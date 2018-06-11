#include <stdlib.h> // atoi
#include <iostream>
#include "FinalStateTransducer.h"

FinalStateTransducer::FinalStateTransducer(char* regExpr, int separator, int length) // The regular expression should be of type: 'word:number'
{
	char* word = regExpr;
	*(regExpr + separator) = '\0';
	*(regExpr + length) = '\0';
	const char* pOutputNumber = regExpr + separator + 1; // + ':'.
	int outputNumber = atoi(pOutputNumber);
#if defined(INFO)
		std::cout << "Creating a transducer for word \"" << word << "\" with output number " << outputNumber << std::endl; // Only info purposes.
#endif


																													   // Creating a transducer which accepts only the given word.
	Delta.resize(2);

	Delta[0][word].push_back(Transition{ 1, outputNumber }); // One transition: 0 -> 1 via word @word and output @outputNumber.
	FinalStates.insert(1); // 1 is a final state (q0 which is 0 is the initial state).
}

void FinalStateTransducer::CloseStar()
{

}

void FinalStateTransducer::ClosePlus()
{

}

void FinalStateTransducer::Concat(const FinalStateTransducer& right)
{

}

void FinalStateTransducer::Union(const FinalStateTransducer& right)
{

}
