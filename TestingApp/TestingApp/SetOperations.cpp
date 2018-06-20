#include "SetOperations.h"


void TransitiveClosure(SetOfTransitions& r)
{
	auto cR = r; // {<a, [b]>}, an map of states 'a' and their destinations, i.e. a set of b's ([b])

	bool rHasChanged;
	do
	{
		rHasChanged = false;
		// TODO: check for iterator invalidations!!!(should be fine)
		for (auto& destinations : cR) // @destinations is a pair <a, [b]> for which the array [b] is all 'b', such that (a, b) belongs to 'cR'
		{
			const auto& a = destinations.first;
			const auto destinationsSize = destinations.second.size();
			const auto dests = destinations.second; // to avoid the iterator invalidation because I will insert elements in it
			for (auto b : dests)
			{
				const auto it = r.find(b); // it is a pair <b, [c]> for which the array [c] is all 'c', such that (b, c) belongs to 'r'
				if (it != r.end())
				{
					destinations.second.insert(it->second.begin(), it->second.end()); // add all (a, c) to 'cR' such that c belongs to [c]
				}
			}

			if (destinationsSize != destinations.second.size())
			{
				rHasChanged = true;
			}
		}
	} while (rHasChanged);

	r = std::move(cR);
}