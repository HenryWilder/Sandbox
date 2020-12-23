#pragma once
#include "InputProcessing.h"
#include <thread>
#include <vector>

void StartupThread(std::atomic<unsigned long long>& atomic, int bit);

unsigned long long SpawnThreads(int count);