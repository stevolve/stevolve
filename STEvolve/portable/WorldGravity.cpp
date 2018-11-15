#include <atomic>
#include <stdint.h>
#include <Windows.h>

#include "Settings.h"
#include "CellBase.h"
#include "WorldBase.h"
#include "WorldGravity.h"

#define GRAVITYRATE 15

void WorldGravity::AdjustEnvironment()
{
	int x, y;

	// simulate gravity
	if (!(ui64Clock % GRAVITYRATE))
		for (x = 0; x < giWorldWidth; x++)
			for (y = giWorldHeight - 3; y > 0; y--) // should be 'y >= 0'?
				if (water[x][y]->energy)
					if (!water[x][y + 1]->energy)
					{
						Cell *pTmp = water[x][y];
						water[x][y] = water[x][y + 1];
						water[x][y + 1] = pTmp;
					}
}

void WorldGravity::GiveEnergy(Cell *pCell)
{
	// simulating the movement of the sun and the seasons of the year
	int v, h; // 'v' = vertical, 'h' = horizontal
	if (pCell->x < uSunVert) v = min(uSunVert - pCell->x, pCell->x + giWorldWidth - uSunVert);
	else v = min(pCell->x - uSunVert, uSunVert + giWorldWidth - pCell->x);
	//v = (v / 4) * 4 + 1;
	v = (v / 32) * 2 + 1;

	h = pCell->y; // top-down energy from the sun

	if (!pCell->bDead) pCell->energy += iSunEnergy / (v + h) - min(iSunEnergy / (v + h), pCell->iExternalCycles / 100);
}
