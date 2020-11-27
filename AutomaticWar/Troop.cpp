#include "Troop.h"

#include <iostream>
#include <cassert>
#include <cmath>

#include "World.h"
#include "Country.h"
#include "Squad.h"

#include "Radar.h"

using namespace Bot;
using namespace std;

Troop::Troop(Country* myCountry, Country* targetCounty, int kmX, int kmY)
	: _myCountry(myCountry), _targetCountry(targetCounty)
{
	_location.X = (double)kmX * 1000.0;
	_location.Y = (double)kmY * 1000.0;
}

Troop::~Troop()
{
	for (Squad* sqd : _squads)
		SAFE_DELETE(sqd);
}

bool Troop::Initialize(size_t soldierSize)
{
	for (size_t i = 0; i < SQUAD_SIZE; i++)
	{
		size_t squadSoldierSize;
		if (i < SQUAD_SIZE - 1)
			squadSoldierSize = soldierSize / SQUAD_SIZE;
		else // last
			squadSoldierSize = soldierSize - (soldierSize / SQUAD_SIZE) * (SQUAD_SIZE - 1);

		Squad* newSquad = new Squad(squadSoldierSize, &_luck);
		newSquad->SetIndex(i);
		if (!newSquad->Initialize())
		{
			SAFE_DELETE(newSquad);
			return false;
		}

		_squads.push_back(newSquad);
	}

	return true;
}

void Troop::Update()
{
	if (_state == State::MARCH)
	{
		DVector goalPos = _targetCountry->GetLocationForMeter();
		DVector meToGoalDir = goalPos - _location;
		meToGoalDir.Normalize();
		_location += meToGoalDir * 1.5; // 1.5m

		int kmX = (int)(_location.X / 1000.0);
		int kmY = (int)(_location.Y / 1000.0);
		Radar::Instance().AddRectangle(kmX, kmY, 3, false);

		if(_targetTroop == nullptr)
		{
			const vector<Troop*>& enemyTroops = _targetCountry->GetTroops();
			auto enmyTrupItr = find_if(enemyTroops.cbegin(), enemyTroops.cend(), [this](Troop* enmyTrup)
			{
				return (enmyTrup->GetTargetCountry() == _myCountry);
			});

			// 우리 나라로 쳐들어 오고 있는 타깃 적국의 부대가 있다면 타깃 부대여
			if (enmyTrupItr != enemyTroops.cend())
				_targetTroop = *enmyTrupItr;
			assert(this != _targetTroop);
		}

		if(_targetTroop != nullptr)
		{
			const DVector& enmyTrupPos = _targetTroop->GetLocation();
			double meToEnmyDist = DVector::Dist(enmyTrupPos, _location);
			if (meToEnmyDist < 100) // 100m
			{
				if (_myCountry->GetUpdateOrder() < _targetCountry->GetUpdateOrder())
				{
					_state = State::FIGHT_READY;
					_targetTroop->SetState(State::FIGHT_READY);
				}
			}
		}
	}
	else if(_state == State::FIGHT)
	{
		assert(_targetTroop != nullptr);

		if (_targetTroop->GetState() == State::FIGHT)
		{
			for (size_t i = 0; i < _squads.size(); ++i)
			{
				Squad* mySquad = _squads[i];
				Squad* targetSquad = _targetTroop->GetLiveNearestSquad(i);
				assert(targetSquad != nullptr && targetSquad != mySquad);
				mySquad->Strike(targetSquad);
			}
		}
		else if (_targetTroop->GetState() == State::DESTROY)
		{
			_state = State::END;
			_targetTroop->SetState(State::END);

			cout << "Win: " << _myCountry->GetId() << ", Lose: " << _targetCountry->GetId() << endl;
			_result = Result::WIN;
			_targetTroop->SetResult(Result::LOSE);
		}
	}
	else if (_state == State::DESTROY)
	{
		if (_targetTroop->GetState() == State::FIGHT)
		{
			cout << "Lose: " << _myCountry->GetId() << ", Win: " << _targetCountry->GetId() << endl;
			_result = Result::LOSE;
			_targetTroop->SetResult(Result::WIN);
		}
		else if (_targetTroop->GetState() == State::DESTROY)
		{
			cout << "Draw: " << _myCountry->GetId() << ", Draw: " << _targetCountry->GetId() << endl;
			_result = Result::DRAW;
			_targetTroop->SetResult(Result::DRAW);
		}

		_state = State::END;
		_targetTroop->SetState(State::END);
	}
}

void Troop::PostUpdate()
{
	if (_state == State::FIGHT_READY)
	{
		_state = State::FIGHT;
		cout << "Start Fight! - " << MakeLog() << endl;
	}
	else if (_state == State::FIGHT)
	{
		for (auto itr = _squads.cbegin(); itr != _squads.cend();)
		{
			Squad* sqd = *itr;

			if (!sqd->TakeDamage())
			{
				//cout << "squad broken - id: " << _myCountry->GetId() << endl;
				itr = _squads.erase(itr);
				SAFE_DELETE(sqd);
			}
			else
			{
				++itr;
			}
		}

		if (_squads.size() == 0)
		{
			_state = State::DESTROY;
			cout << "Destroyed - " << MakeLog() << endl;
		}
	}
}

Squad* Troop::GetLiveNearestSquad(size_t index)
{
	Squad* nearestSquad = nullptr;

	size_t minDist = SQUAD_SIZE + 1;
	for (size_t i = 0; i < _squads.size(); ++i)
	{
		Squad* sqd = _squads[i];
		if (sqd->GetSoldierSize() == 0)
			continue;

		size_t dist = (size_t)abs((int)(index - i));
		if (dist < minDist)
		{
			minDist = dist;
			nearestSquad = sqd;
		}
	}

	return nearestSquad;
}

string Troop::MakeLog()
{
	string log;
	log += "My country id: " + to_string(_myCountry->GetId());
	log += ", My troop id: " + to_string(_id);
	log += ", Target country id: " + to_string(_targetCountry->GetId());

	string targetTroopInfo;
	if (_targetTroop != nullptr)
		targetTroopInfo = ", Target troop id: " + to_string(_targetTroop->GetId());
	else
		targetTroopInfo = "Annihilated";
	log += targetTroopInfo;

	return log;
}

size_t Troop::ReturnLiveSoldiers()
{
	size_t soldierSize = 0;

	for (Squad* sqd : _squads)
		soldierSize += sqd->GetSoldierSize();

	return soldierSize;
}
