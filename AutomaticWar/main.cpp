#include "stdafx.h"

#include <vector>
#include <iostream>
#include <windows.h>

#include "World.h"
#include "Radar.h"

using namespace Bot;

int main()
{
	World& world = World::Instance();
	if (!world.Initialize())
		return 0;

	while (true)
		world.Update();

    return 0;
}
