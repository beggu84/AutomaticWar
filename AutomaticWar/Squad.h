#pragma once

#include "Global.h"
#include "Troop.h"

class Luck;

namespace Bot
{
	class Troop;

	class Squad
	{
	private:
		uint32 _index;
		size_t _soldierSize;
		uint32 _damage{ 0 };
		Troop* _troop;
		Luck* _luck;

	public:
		Squad(size_t soldierSize, Luck* luck);
		~Squad();

	public:
		GETTER_SETTER(uint32, Index, _index)
		GETTER(size_t, SoldierSize, _soldierSize)
		GETTER_SETTER(uint32, Damage, _damage)
		GETTER_SETTER(Troop*, Troop, _troop)

		bool Initialize();
		void Strike(Squad* targetSquad);
		bool TakeDamage();
	};
}
