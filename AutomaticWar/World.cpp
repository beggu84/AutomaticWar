#include "World.h"

#include <fstream>
#include <sstream>
#include <chrono>

#include "Country.h"

#include "Global.h"
#include "json.hpp"
#include "JsonHelper.h"
#include "Radar.h"

using namespace Bot;
using namespace std;
using namespace nlohmann;

World& World::Instance()
{
	static World world;
	return world;
}

World::~World()
{
	for (Country* country : _countries)
		SAFE_DELETE(country);
}

bool World::Initialize()
{
	//ifstream ifs("../bin/data/World.json");
	ifstream ifs("../bin/data/WorldForTest.json"); // for test
	assert(ifs.is_open());

	stringstream ss;
	while (!ifs.eof())
	{
		string str;
		ifs >> str;
		ss << str;
	}
	ifs.close();

	json worldObj = json::parse(ss.str());
	if (!JsonHelper::GetUintField(worldObj, "width", _width)) return false;
	if (!JsonHelper::GetUintField(worldObj, "height", _height)) return false;

	string typeStr;
	if (!JsonHelper::GetStringField(worldObj, "type", typeStr)) return false;
	if (typeStr == "GRID")
		_type = WorldType::GRID;
	else if (typeStr == "MANUAL")
		_type = WorldType::MANUAL;
	else if (typeStr == "AUTO")
		_type = WorldType::AUTO;
	else
		return false;

	json countriesObj;
	if (!JsonHelper::GetArrayField(worldObj, "countries", countriesObj)) return false;

	for (size_t i = 0; i < countriesObj.size(); ++i)
	{
		Country* newCountry = new Country(i);
		if (!newCountry->Initialize(countriesObj[i]))
		{
			SAFE_DELETE(newCountry);
			return false;
		}

		_countries.push_back(newCountry);
	}
	cout << endl;

	if (_type == WorldType::AUTO)
	{
		if (!JsonHelper::GetUintField(worldObj, "max_enemy_distance", _maxEnemyDistance)) return false;
	}

	_lastConscriptionTime = _lastCountryActionTime =
		_lastTroopActionTime = _lastUpdateTime = chrono::steady_clock::now();

	//_lastConscriptionTime -= chrono::seconds(CONSCRIPTION_UNIT_SECOND);
	_lastCountryActionTime -= chrono::seconds(COUNTRY_ACTION_UNIT_SECOND);
	//_lastTroopActionTime -= chrono::microseconds(TROOP_ACTION_UNIT_MICROSECOND);

	Radar::Instance().Initialize(900, 1050);

	return true;
}

void World::Update()
{
	chrono::steady_clock::time_point currTime = chrono::steady_clock::now();

	time_t conscriptionDeltaSeconds = chrono::duration_cast<chrono::seconds>(currTime - _lastConscriptionTime).count();
	if (conscriptionDeltaSeconds >= CONSCRIPTION_UNIT_SECOND)
	{
		_lastConscriptionTime = currTime;

		for (Country* country : _countries)
			country->UpdateBabies();
		cout << "----- update babies ----- " << endl;
	}

	time_t countryActionDeltaSeconds = chrono::duration_cast<chrono::seconds>(currTime - _lastCountryActionTime).count();
	if (countryActionDeltaSeconds >= COUNTRY_ACTION_UNIT_SECOND)
	{
		_lastCountryActionTime = currTime;

		for (Country* country : _countries)
			country->Update();
		//cout << "----- update countries ----- " << endl;
	}

	time_t troopActionDeltaMicroseconds = chrono::duration_cast<chrono::microseconds>(currTime - _lastTroopActionTime).count();
	if (troopActionDeltaMicroseconds >= TROOP_ACTION_UNIT_MICROSECOND)
	{
		_lastTroopActionTime = currTime;

		for (Country* country : _countries)
			country->UpdateTroops();
		for (Country* country : _countries)
			country->PostUpdateTroops();
		//cout << "----- update troops ----- " << endl;

		Radar::Instance().Update();
	}

	static uint32 updateCount = 0;
	time_t updateDeltaTime = chrono::duration_cast<chrono::milliseconds>(currTime - _lastUpdateTime).count();
	if (updateDeltaTime >= UPDATE_UNIT_TIME)
	{
		_lastUpdateTime = currTime;

		if(updateCount < TIME_DIVIDER)
			cout << "Update is slow! - UPS: " << updateCount << ", need: " << TIME_DIVIDER << endl;

		updateCount = 0;
	}

	updateCount++;
}

Country* World::GetCountry(uint32 id) const
{
	for (Country* country : _countries)
	{
		if (country->GetId() == id)
			return country;
	}

	return nullptr;
}
