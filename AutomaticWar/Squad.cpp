#include "Squad.h"

#include <iostream>
#include <cassert>

#include "Troop.h"

#include "Luck.h"

using namespace Bot;
using namespace std;

Squad::Squad(size_t soldierSize, Luck* luck)
	: _soldierSize(soldierSize), _luck(luck)
{
}

Squad::~Squad()
{
}

bool Squad::Initialize()
{
	return true;
}

void Squad::Strike(Squad* targetSquad)
{
	const uint32 SOLDIER_STRIKE_POWER = 10;
	const uint32 STRIKE_FACTOR = 5;
	uint32 randomStrikeFactor = _luck->Next(STRIKE_FACTOR - 1, STRIKE_FACTOR + 1);
	//cout << randomStrikeFactor << endl;
	uint32 damage = _soldierSize * SOLDIER_STRIKE_POWER * randomStrikeFactor;

	targetSquad->SetDamage(damage);
}

bool Squad::TakeDamage()
{
	assert(_damage > 0);

	const uint32 SOLDIER_DEFENSIVE_POWER = 10;
	const uint32 DEFENCE_FACTOR = 3;
	uint32 randomDefenceFactor = _luck->Next(DEFENCE_FACTOR - 1, DEFENCE_FACTOR + 1);
	uint32 reducedDamage = _soldierSize * SOLDIER_DEFENSIVE_POWER * randomDefenceFactor;

	if (reducedDamage >= _damage)
		_damage = 1;
	else // _damage > reducedDamage
		_damage -= reducedDamage;

	const uint32 SOLDIER_VITAL_POWER = 10;
	const uint32 VITALITY_FACTOR = 4;
	size_t diedSoldierSize = _damage / (SOLDIER_VITAL_POWER * VITALITY_FACTOR);

	if (diedSoldierSize >= _soldierSize)
	{
		_soldierSize = 0;
		return false; // all died
	}
	else // _soldierSize > diedSoldierSize
	{
		_soldierSize -= diedSoldierSize;
		//cout << _soldierSize << endl;
		return true;
	}
}
