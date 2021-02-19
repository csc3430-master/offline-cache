#include <iostream>
#include <iomanip>
#include <memory>

using namespace std;



//const char request[]    = {'a','a','d','e','b','b','a','c','f','d','e','a','f','b','e','c'};
//char cache[]            = {'a','b','c'};

const char request[]    = {'a','b','c','d','a','b','c','a','a','b','c'};
char cache[]            = {'a','b','c'};


const int cacheSize     = sizeof(cache) / sizeof(cache[0]);
const int requestLength = sizeof(request) / sizeof(request[0]);

// for reset
char originalCache[]    = {'a','b','c'};


class Strategy {

public:
	Strategy(std::string name) : strategyName(name) {}
	virtual ~Strategy() = default;

	// calculate which cache place should be used
	virtual int apply(int requestIndex)                                      = 0;

	// updates information the strategy needs
	virtual void update(int cachePlace, int requestIndex, bool cacheMiss)    = 0;

	const std::string strategyName;
};

bool updateCache(int requestIndex, Strategy* strategy)
{
	// calculate where to put request
	int cachePlace = strategy->apply(requestIndex);

	// proof whether its a cache hit or a cache miss
	bool isMiss = request[requestIndex] != cache[cachePlace];

	// update strategy (for example recount distances)
	strategy->update(cachePlace, requestIndex, isMiss);

	// write to cache
	cache[cachePlace] = request[requestIndex];

	return isMiss;
}
class LFD : public Strategy {
public:
	LFD() : Strategy("LFD")
	{
		// precalc next usage before starting to fullfill requests
		for (int i=0; i<cacheSize; ++i) nextUse[i] = calcNextUse(-1, cache[i]);
	}

	int apply(int requestIndex) override
	{
		int latest = 0;

		for(int i=0; i<cacheSize; ++i)
		{
			if(cache[i] == request[requestIndex])
				return i;

			else if(nextUse[i] > nextUse[latest])
				latest = i;
		}

		return latest;
	}

	void update(int cachePos, int requestIndex, bool cacheMiss) override
	{
		nextUse[cachePos] = calcNextUse(requestIndex, cache[cachePos]);
	}

private:

	int calcNextUse(int requestPosition, char pageItem)
	{
		for(int i = requestPosition+1; i < requestLength; ++i)
		{
			if (request[i] == pageItem)
				return i;
		}

		return requestLength + 1;
	}

	// next usage of page
	int nextUse[cacheSize];
};
int main()
{
	Strategy* selectedStrategy[] = { new LFD };

	for (int strat=0; strat < 1; ++strat)
	{
		// reset cache
		for (int i=0; i < cacheSize; ++i) cache[i] = originalCache[i];

		cout <<"\nStrategy: " << selectedStrategy[strat]->strategyName << endl;

		cout << "\nCache initial: (";
		for (int i=0; i < cacheSize-1; ++i) cout << cache[i] << ",";
		cout << cache[cacheSize-1] << ")\n\n";

		cout << setw(9) << "Request";
		for (int i=0; i < cacheSize; ++i) cout << setw(7) << "cache " << i ;
		cout << setw(11) << "cache miss" << endl;

		int cntMisses = 0;

		for(int i=0; i<requestLength; ++i)
		{
			bool isMiss = updateCache(i, selectedStrategy[strat]);
			if (isMiss) ++cntMisses;
			cout << setw(9) << request[i] << "\t";
			for (int l=0; l < cacheSize; ++l) cout << setw(5) << cache[l] ;
			cout << setw(11) << (isMiss ? "x" : "") << endl;
		}

		cout<< "\nTotal cache misses: " << cntMisses << endl;
	}

	for(int i=0; i<5; ++i) delete selectedStrategy[i];
	return 0;
}
