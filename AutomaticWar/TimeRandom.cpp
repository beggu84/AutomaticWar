#include "TimeRandom.h"

#include <chrono>
#include <random>
#include <cmath>

using namespace std;

int TimeRandom::Next(int minValue, int maxValue)
{
	auto current = chrono::system_clock::now();
	auto duration = current.time_since_epoch();
	auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
	mt19937_64 genMT(millis);

	int range = maxValue - minValue;
	double ratio = (double)(genMT() % 100L) / 100.0;
	int randomValue = (int)round((double)range * ratio);

	return minValue + randomValue;
}

unsigned int TimeRandom::Next(unsigned int maxValue)
{
	return Next(0, maxValue);
}
