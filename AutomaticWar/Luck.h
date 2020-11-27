#pragma once

class Luck
{
private:
	unsigned int _state[16];
	unsigned int _index;

public:
	Luck();
	~Luck();

public:
	unsigned int Next(unsigned int minValue, unsigned int maxValue);
	unsigned int Next(unsigned int maxValue);
	unsigned int Next();
};
