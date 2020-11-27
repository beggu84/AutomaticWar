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

	// 1. �ֺ� ������ ��Ȳ�� ���ɴ� (Ÿ�� ���� �����Ѵ�)
	map<double, Country*> distToCountryMap;
	for (uint32 enmyId : _enemyIds)
	{
		// �̹� Ÿ������ �����ؼ� �δ븦 ���� ����� �ĺ����� ����
		auto trgtEnmyIdItr = find(_targetEnemyIds.cbegin(), _targetEnemyIds.cend(), enmyId);
		if (trgtEnmyIdItr != _targetEnemyIds.cend())
			continue;

		Country* enemyCountry = world.GetCountry(enmyId);
		auto myTrupItr = find_if(_troops.cbegin(), _troops.cend(), [enemyCountry](Troop* myTrup)
		{
			return (myTrup->GetTargetCountry() == enemyCountry);
		});

		// �� �������� �δ븦 ������ �ʾҴµ�
		if (myTrupItr == _troops.cend())
		{
			const vector<Troop*>& enemyTroops = enemyCountry->GetTroops();
			auto enmyTrupItr = find_if(enemyTroops.cbegin(), enemyTroops.cend(), [this](Troop* enmyTrup)
			{
				return (enmyTrup->GetTargetCountry() == this);
			});

			// ������ ������ �δ븦 ���´ٸ� Ÿ�� �ĺ�
			if (enmyTrupItr != enemyTroops.cend())
			{
				const DVector& enmyTrupPos = (*enmyTrupItr)->GetLocation();
				double hereToEnmyDist = DVector::Dist(enmyTrupPos, _locationForMeter);
				distToCountryMap.insert(make_pair(hereToEnmyDist, enemyCountry));
			}
		}
	}

	Country* targetEnemy = nullptr;
	if(distToCountryMap.size() > 0) // �� ���� ġ���ϴ� ���� �ִٸ�
	{
		// �� �߿��� ���� ����� �δ��� ���� Ÿ��
		// TODO: ���� ������ ������ ���� �������� �δ븦 ���� �ľ� �� ����?
		targetEnemy = distToCountryMap.begin()->second;
		cout << "How dare you!!! - id: " << _id << ", enemy id: " << targetEnemy->GetId() << endl;
	}
	else // �ƴϸ�.. �ƹ� ���� ��
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

	// (2. �ֺ� �뺴 ������ �����Ѵ�)

	// 3. ���� ��Ȳ�� �����Ͽ� ���� �ٸ��� (�ٸ� ����, �����Ҹ��� �뺴 ����)
	size_t notTargetEnemySize = _enemyIds.size() - _targetEnemyIds.size();
	size_t soldierSize = _population / (notTargetEnemySize + 1);

	// (4. �ֺ� �뺴 ���¿� ������ ������)

	// 5. �����Ѵ�
	_population -= soldierSize;
	_targetEnemyIds.push_back(targetEnemy->GetId());
	//cout << "���� - name: " << _name << ", id: " << _id << ", enemy id: " << targetEnemy->GetId() << ", soldier: " << soldierSize << endl;

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
