#include <atomic>
#include <stdint.h>
#include <Windows.h>

#include "Settings.h"
#include "CellBase.h"
#include "WorldBase.h"
#include "WorldToroidal.h"

void WorldToroidal::Init()
{
	World::Init(); // call base-class World::Init() first

	// create land masses
	for (int x = 0; x < giWorldWidth; x++)
		for (int y = 0; y < giWorldHeight; y++)
		{
			if (x > 0 && y > 0 && x < giWorldWidth - 1 && y < giWorldHeight - 1)
			{
				if (!(rand() % 4000))
				{
					int h = rand() % 100;
					for (int i = 0; i < h; i++)
						for (int j = 0; j < h; j++)
							if (x - h > 0 && y - h > 0)
								land[x - h + i][y - h + j] = __max(land[x - h + i][y - h + j], h < 70 ? 1000 : 2000);// h * 100);
				}
				else land[x][y] = land[x][y] > 0 ? land[x][y] : 0;
			}
			//else land[x][y] = 0;
		}
	int landcount = 0;
	for (int x = 0; x < giWorldWidth; x++)
		for (int y = 0; y < giWorldHeight; y++)
			if (land[x][y] > 0) landcount++;
	//Trace("land percentage=%d\n", (int)(landcount / (float)(giWorldWidth * giWorldHeight) * 100));
}

void WorldToroidal::AdjustEnvironment()
{
	// no adjustments needed for the toroidal world
}

void WorldToroidal::GiveEnergy(Cell *pCell)
{
	// simulating the movement of the sun and the seasons of the year
	int v, h; // 'v' = vertical, 'h' = horizontal
	if (pCell->x < uSunVert) v = min(uSunVert - pCell->x, pCell->x + giWorldWidth - uSunVert);
	else v = min(pCell->x - uSunVert, uSunVert + giWorldWidth - pCell->x);
	//v = (v / 4) * 4 + 1;
	v = (v / 32) * 2 + 1;

	h = abs((int)uSunHorz - pCell->y) / 64; // sun is above the entire grid

	if (!pCell->bDead) pCell->energy += iSunEnergy / (v + h) - min(iSunEnergy / (v + h), pCell->iExternalCycles / 100);
}
