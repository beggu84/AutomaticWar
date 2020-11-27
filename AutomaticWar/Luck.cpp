#include "Luck.h"

#include <iostream>

#include "TimeRandom.h"

using namespace std;

Luck::Luck()
{
	_index = 0;
	unsigned int s = TimeRandom::Next(1000);
	//cout << s << endl;
	for (int i = 0; i < 16; i++)
	{
		_state[i] = s;
		s += s + 73;
	}
}

Luck::~Luck()
{
}

unsigned int Luck::Next(unsigned int minValue, unsigned int maxValue)
{
	return (Next() % (maxValue + 1 - minValue)) + minValue;
}

unsigned int Luck::Next(unsigned int maxValue)
{
	return Next() % (maxValue + 1);
}

unsigned int Luck::Next()
{
	unsigned int a, b, c, d;

	a = _state[_index];
	c = _state[(_index + 13) & 15];
	b = a ^ c ^ (a << 16) ^ (c << 15);
	c = _state[(_index + 9) & 15];
	c ^= (c >> 11);
	a = _state[_index] = b ^ c;
	d = a ^ ((a << 5) & 0xda442d24U);
	_index = (_index + 15) & 15;
	a = _state[_index];
	_state[_index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);

	return _state[_index];
}
