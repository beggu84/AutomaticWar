#pragma once

#include <string>
#include <vector>
#include <list>
#include <queue>
#include <json.hpp>

#include "Global.h"
#include "DVector.h"

namespace Bot
{
	class Troop;

	class Country
	{
	private:
		const size_t POPULATION_UNIT = 100000;
		const size_t MINIMUM_POPULATION = 100;

		uint32 _updateOrder;

		uint32 _id{ 0 };
		std::string _name;
		int _x{ 0 }, _y{ 0 }; // km unit
		DVector _locationForMeter;
		std::list<uint32> _enemyIds;
		size_t _population{ 0 }; // 10000 unit

		uint32 _currTroopId{ 1 };
		std::queue<size_t> _babySizes;
		std::vector<uint32> _targetEnemyIds;
		std::vector<Troop*> _troops;

	public:
		Country(uint32 updateOrder);
		~Country();

	public:
		GETTER(uint32, UpdateOrder, _updateOrder)
		GETTER(uint32, Id, _id)
		GETTER(std::string&, Name, _name)
		GETTER(int, X, _x)
		GETTER(int, Y, _y)
		GETTER(std::list<uint32>&, EnemyIds, _enemyIds)
		GETTER(size_t, Population, _population)
		GETTER(DVector&, LocationForMeter, _locationForMeter)
		GETTER(std::vector<Troop*>&, Troops, _troops)

		bool Initialize(const nlohmann::json& jsonObj);
		void UpdateBabies();
		void Update();
		void UpdateTroops();
		void PostUpdateTroops();
	};
}
