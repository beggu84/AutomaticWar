#pragma once

class TimeRandom
{
public:
	static int Next(int minValue, int maxValue);
	static unsigned int Next(unsigned int maxValue);
};
