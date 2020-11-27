#pragma once

#include <vector>
#include <chrono>

#include "Global.h"
#include "WorldType.h"

namespace Bot
{
	class Country;

	class World
	{
	private:
		const time_t CONSCRIPTION_UNIT_SECOND_REAL = 365 * 24 * 60 * 60; // 1 year
		const time_t COUNTRY_ACTION_UNIT_SECOND_REAL = 24 * 60 * 60; // 1 day
		const time_t TROOP_ACTION_UNIT_MICROSECOND_REAL = 1000000; // 1 second
		//const time_t TIME_DIVIDER = 24 * 60;
		const time_t TIME_DIVIDER = 24 * 60 * 60; // for test
		const time_t CONSCRIPTION_UNIT_SECOND = CONSCRIPTION_UNIT_SECOND_REAL / TIME_DIVIDER;
		const time_t COUNTRY_ACTION_UNIT_SECOND = COUNTRY_ACTION_UNIT_SECOND_REAL / TIME_DIVIDER;
		const time_t TROOP_ACTION_UNIT_MICROSECOND = TROOP_ACTION_UNIT_MICROSECOND_REAL / TIME_DIVIDER;
		const time_t UPDATE_UNIT_TIME = 1000; // 1000 millisecond -> 1 second

		uint32 _width{ 0 }, _height{ 0 }; // km unit
		WorldType _type;
		std::vector<Country*> _countries;

		// for auto type
		uint32 _maxEnemyDistance;

		std::chrono::steady_clock::time_point _lastConscriptionTime;
		std::chrono::steady_clock::time_point _lastCountryActionTime;
		std::chrono::steady_clock::time_point _lastTroopActionTime;
		std::chrono::steady_clock::time_point _lastUpdateTime;

	public:
		static World& Instance();
	private:
		World(){}
		~World();

	public:
		GETTER(WorldType, Type, _type)

		bool Initialize();
		void Update();

		Country* GetCountry(uint32 id) const;
	};
}
