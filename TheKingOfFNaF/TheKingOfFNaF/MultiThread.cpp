#include "MultiThread.h"

void StartupThread(std::atomic<unsigned long long>& atomic, int bit)
{
	atomic |= bit;
}

unsigned long long SpawnThreads(int count)
{
	std::atomic<unsigned long long> resultsFlag = 0;

	std::vector<std::thread> threads;

	for (int t = 0; t < count; ++t) {
		threads.push_back(std::thread(StartupThread, std::ref(resultsFlag), (1 << t)));
	}

	for (int t = 0; t < count; ++t) {
		threads[t].join();
	}

	threads.erase(threads.begin(), threads.end());

	return resultsFlag;
}
