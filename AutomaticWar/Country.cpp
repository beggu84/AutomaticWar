#include "Country.h"

#include <iostream>
#include <map>

#include "World.h"
#include "WorldType.h"
#include "Troop.h"

#include "Radar.h"
#include "JsonHelper.h"

using namespace Bot;
using namespace std;
using namespace nlohmann;

Country::Country(uint32 updateOrder)
	: _updateOrder(updateOrder)
{

}

Country::~Country()
{
	for (Troop* trup : _troops)
		SAFE_DELETE(trup);
}

bool Country::Initialize(const json& jsonObj)
{
	if (!JsonHelper::GetUintField(jsonObj, "id", _id)) return false;
	if (!JsonHelper::GetStringField(jsonObj, "name", _name)) return false;
	if (!JsonHelper::GetIntField(jsonObj, "x", _x)) return false;
	if (!JsonHelper::GetIntField(jsonObj, "y", _y)) return false;

	_locationForMeter.X = (double)_x * 1000.0;
	_locationForMeter.Y = (double)_y * 1000.0;

	WorldType worldType = World::Instance().GetType();
	if (worldType == WorldType::GRID)
	{
	}
	else if (worldType == WorldType::MANUAL)
	{
		if (!JsonHelper::GetUintArrayField(jsonObj, "enemy_ids", _enemyIds)) return false;
		_population = (_enemyIds.size()+1) * POPULATION_UNIT;
	}
	else if (worldType == WorldType::AUTO)
	{
		if (!JsonHelper::GetUintField(jsonObj, "population", _population)) return false;
		_population *= POPULATION_UNIT;
	}

	for (uint32 i = 0; i < 19; ++i)
		_babySizes.push(_population);

	cout << "id: " << _id << ", name: " << _name << ", x: " << _x << ", y: " << _y << ", population: " << _population << endl;

	Radar::Instance().AddRectangle(_x, _y, 10);

	return true;
}

void Country::UpdateBabies()
{
	if (_babySizes.size() == 0)
		return;

	size_t newSoldierSize = _babySizes.front();
	_babySizes.pop();
	_babySizes.push(_population);

	_population += newSoldierSize;
	cout << "id: " << _id << ", new soldier size: " << newSoldierSize << ", population: " << _population << endl;
}

void Country::Update()
{
	if (_population <= MINIMUM_POPULATION)
		return;

	World& world = World::Instance();

	// 1. 주변 나라의 정황을 살핀다 (타깃 적을 선택한다)
	map<double, Country*> distToCountryMap;
	for (uint32 enmyId : _enemyIds)
	{
		// 이미 타깃으로 선정해서 부대를 보낸 나라는 후보에서 제외
		auto trgtEnmyIdItr = find(_targetEnemyIds.cbegin(), _targetEnemyIds.cend(), enmyId);
		if (trgtEnmyIdItr != _targetEnemyIds.cend())
			continue;

		Country* enemyCountry = world.GetCountry(enmyId);
		auto myTrupItr = find_if(_troops.cbegin(), _troops.cend(), [enemyCountry](Troop* myTrup)
		{
			return (myTrup->GetTargetCountry() == enemyCountry);
		});

		// 난 적국으로 부대를 보내지 않았는데
		if (myTrupItr == _troops.cend())
		{
			const vector<Troop*>& enemyTroops = enemyCountry->GetTroops();
			auto enmyTrupItr = find_if(enemyTroops.cbegin(), enemyTroops.cend(), [this](Troop* enmyTrup)
			{
				return (enmyTrup->GetTargetCountry() == this);
			});

			// 적국은 나에게 부대를 보냈다면 타깃 후보
			if (enmyTrupItr != enemyTroops.cend())
			{
				const DVector& enmyTrupPos = (*enmyTrupItr)->GetLocation();
				double hereToEnmyDist = DVector::Dist(enmyTrupPos, _locationForMeter);
				distToCountryMap.insert(make_pair(hereToEnmyDist, enemyCountry));
			}
		}
	}

	Country* targetEnemy = nullptr;
	if(distToCountryMap.size() > 0) // 날 먼저 치려하는 나라가 있다면
	{
		// 그 중에서 가장 가까운 부대의 나라가 타깃
		// TODO: 가장 가깝진 않은데 가장 위협적인 부대를 먼저 쳐야 할 수도?
		targetEnemy = distToCountryMap.begin()->second;
		cout << "How dare you!!! - id: " << _id << ", enemy id: " << targetEnemy->GetId() << endl;
	}
	else // 아니면.. 아무 나라나 쳐
	{
		for (uint32 enmyId : _enemyIds)
		{
			auto trgtEnmyIdItr = find(_targetEnemyIds.cbegin(), _targetEnemyIds.cend(), enmyId);
			if (trgtEnmyIdItr == _targetEnemyIds.cend())
			{
				cout << "Just attack you! - id: " << _id << ", enemy id: " << enmyId << endl;
				targetEnemy = world.GetCountry(enmyId);
				break;
			}
		}
	}

	if (targetEnemy == nullptr)
		return;

	// (2. 주변 용병 세력을 조사한다)

	// 3. 여러 상황을 감안하여 군을 꾸린다 (다른 적들, 참전할만한 용병 세력)
	size_t notTargetEnemySize = _enemyIds.size() - _targetEnemyIds.size();
	size_t soldierSize = _population / (notTargetEnemySize + 1);

	// (4. 주변 용병 세력에 서신을 보낸다)

	// 5. 출전한다
	_population -= soldierSize;
	_targetEnemyIds.push_back(targetEnemy->GetId());
	//cout << "출전 - name: " << _name << ", id: " << _id << ", enemy id: " << targetEnemy->GetId() << ", soldier: " << soldierSize << endl;

	Troop* newTroop = new Troop(this, targetEnemy, _x, _y);
	newTroop->SetId(_currTroopId++);
	if (!newTroop->Initialize(soldierSize))
	{
		SAFE_DELETE(newTroop);
		return;
	}
	_troops.push_back(newTroop);
}

void Country::UpdateTroops()
{
	for (auto itr = _troops.cbegin(); itr != _troops.cend();)
	{
		Troop* trup = *itr;
		trup->Update();

		if (trup->GetState() == Troop::State::END)
		{
			_population += trup->ReturnLiveSoldiers();
			itr = _troops.erase(itr);
			SAFE_DELETE(trup);
		}
		else
		{
			++itr;
		}
	}
}

void Country::PostUpdateTroops()
{
	for (Troop* trup : _troops)
		trup->PostUpdate();
}
