#pragma once

#include <vector>
#include <string>

#include "Global.h"
#include "DVector.h"
#include "Luck.h"

namespace Bot
{
	//enum class TroopType
	//{
	//	OFFENCE = 0,
	//	DEFENCE = 1,
	//	SUPPORT = 2,
	//	NONE = 3
	//};

	class Country;
	class Squad;

	class Troop
	{
	public:
		enum class State
		{
			MARCH = 0,
			FIGHT_READY = 1,
			FIGHT = 2,
			DESTROY = 3,
			END = 4,
			NONE = 10
		};

		enum class Result
		{
			WIN = 0,
			LOSE = 1,
			DRAW = 2,
			NONE = 10
		};

	private:
		const size_t SQUAD_SIZE = 5;
		//const size_t SQUAD_SIZE = 1; // for test

		uint32 _id{ 0 };
		State _state{ State::MARCH };
		Country* _myCountry;
		Country* _targetCountry;
		Troop* _targetTroop{ nullptr };
		DVector _location;
		std::vector<Squad*> _squads;
		Luck _luck;
		Result _result{ Result::NONE };

	public:
		Troop(Country* myCountry, Country* targetCounty, int kmX, int kmY);
		~Troop();

	public:
		GETTER_SETTER(uint32, Id, _id)
		GETTER_SETTER(State, State, _state)
		GETTER(Country*, MyCountry, _myCountry)
		GETTER(Country*, TargetCountry, _targetCountry)
		GETTER_SETTER(Troop*, TargetTroop, _targetTroop)
		GETTER(DVector&, Location, _location)
		GETTER_SETTER(Result, Result, _result)

		bool Initialize(size_t soldierSize);
		void Update();
		void PostUpdate();

		size_t ReturnLiveSoldiers();

	private:
		Squad* GetLiveNearestSquad(size_t index);
		std::string MakeLog();
	};
}

