#pragma once
#include "InputProcessing.h"
#include "GameActions.h"
#include <thread>

extern bool g_threadsShouldLoop;
extern bool g_firstTimeScreenUpdate;

void Produce();

void Consume();

void Direct();

void CreateHelpers();
