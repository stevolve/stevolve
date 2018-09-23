#include <atomic>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <Windows.h>

#include "Settings.h"
#include "CellBase.h"
#include "CellNeural.h"
#include "CellProgram.h"
#include "WorldBase.h"
#include "Draw.h"

void Trace(char *szFormat, ...);

int ThreadFunc(int *);

extern bool gbRefreshStop;
extern bool gbThreadStop;
//extern int giNumThreads;
//extern int giSlider;

HANDLE ghEvent;
HANDLE aEventStart[NUMTHREADS];
HANDLE aEventDone[NUMTHREADS];
HANDLE aThread[NUMTHREADS];
CRITICAL_SECTION criticalSection;

// The world is a 2D array of cells 
CELLTYPE ***water;
int **land;
std::atomic<int> cellIDCounter = 0;

cellXY *aCheckExec;
int iCheckCurrent = 0;

uint64_t ui64Clock = 0; // Clock is incremented on each core loop 

int colorScheme = 0; // Currently selected color scheme 

//int giMutationRate2 = 50; // program-based
int giMutationRate2 = 5; // neural-based

// Base amount of energy to introduce per tick
//#define INFLOW_RATE 4000
#define INFLOW_RATE 400
uintptr_t giSunEnergy;
uintptr_t guSunVert = 0;
uintptr_t guSunHorz = giWorldHeight / 2;
int giSunHorzDir = 1;


// Get a neighbor pointer
// 
// @param x Starting X position
// @param y Starting Y position
// @param dir Direction to get neighbor from
// @return Pointer to neighboring cell
//template<class T> Cell *World<T>::getNeighborPtr(const uintptr_t x, const uintptr_t y, const uintptr_t dir)
Cell *getNeighborPtr(const uintptr_t x, const uintptr_t y, const uintptr_t dir)
{
	// Space is toroidal; it wraps at edges 
	uintptr_t x2, y2;

	switch (dir)
	{
	case D_NORTH:
		x2 = x;
		y2 = (y) ? y - 1 : giWorldHeight - 1;
		break;
	case D_NE:
		x2 = (x < (giWorldWidth - 1)) ? x + 1 : 0;
		y2 = (y) ? y - 1 : giWorldHeight - 1;
		break;
	case D_EAST:
		x2 = (x < (giWorldWidth - 1)) ? x + 1 : 0;
		y2 = y;
		break;
	case D_SE:
		x2 = (x < (giWorldWidth - 1)) ? x + 1 : 0;
		y2 = (y < (giWorldHeight - 1)) ? y + 1 : 0;
		break;
	case D_SOUTH:
		x2 = x;
		y2 = (y < (giWorldHeight - 1)) ? y + 1 : 0;
		break;
	case D_SW:
		x2 = (x) ? x - 1 : giWorldWidth - 1;
		y2 = (y < (giWorldHeight - 1)) ? y + 1 : 0;
		break;
	case D_WEST:
		x2 = (x) ? x - 1 : giWorldWidth - 1;
		y2 = y;
		break;
	case D_NW:
		x2 = (x) ? x - 1 : giWorldWidth - 1;
		y2 = (y) ? y - 1 : giWorldHeight - 1;
		break;
	default: // This should never be reached 
		x2 = x;
		y2 = y;
		break;
	}
	return water[x2][y2];
}

//template<class T> cellXY World<T>::getNeighborPos(const uintptr_t x, const uintptr_t y, const uintptr_t dir)
cellXY getNeighborPos(const uintptr_t x, const uintptr_t y, const uintptr_t dir)
{
	// Space is toroidal; it wraps at edges 
	cellXY cell;

	switch (dir)
	{
	case D_NORTH:
		cell.x = x;
		cell.y = (y ? y - 1 : giWorldHeight - 1);
		break;
	case D_NE:
		cell.x = (x < (giWorldWidth - 1) ? x + 1 : 0);
		cell.y = (y ? y - 1 : giWorldHeight - 1);
		break;
	case D_EAST:
		cell.x = (x < (giWorldWidth - 1) ? x + 1 : 0);
		cell.y = y;
		break;
	case D_SE:
		cell.x = (x < (giWorldWidth - 1) ? x + 1 : 0);
		cell.y = (y < (giWorldHeight - 1) ? y + 1 : 0);
		break;
	case D_SOUTH:
		cell.x = x;
		cell.y = (y < (giWorldHeight - 1) ? y + 1 : 0);
		break;
	case D_SW:
		cell.x = (x ? x - 1 : giWorldWidth - 1);
		cell.y = (y < (giWorldHeight - 1) ? y + 1 : 0);
		break;
	case D_WEST:
		cell.x = (x ? x - 1 : giWorldWidth - 1);
		cell.y = y;
		break;
	case D_NW:
		cell.x = (x ? x - 1 : giWorldWidth - 1);
		cell.y = (y ? y - 1 : giWorldHeight - 1);
		break;
	default: // this should never be reached
		cell.x = x;
		cell.y = y;
		break;
	}
	return cell;
}

void SetPixel(int x, int y, Cell *c)
{
	if (gbRefreshStop)
	{
		switch (colorScheme)
		{
		case 0: // energy
			if (c->bDead) SetPixelRGB(x, y, RGB(min(100, c->energy), min(100, c->energy), min(100, c->energy)));
			else SetPixelHLS(x, y, c->wMyColor, (min(c->energy, 0xFFFF) / 273)); // lineage
			break;

		case 1: // age
			if (!c->bDead) SetPixelHLS(x, y, c->wMyColor, (min(c->iExternalCycles * 7, 0xFFFF) / 273)); // age (* 7) to brighten it up
			break;

		case 2: // brains
			if (!c->bDead) SetPixelHLS(x, y, c->wMyColor, (min(c->iCycles / max(1, c->iExternalCycles), 273))); // brains
			break;

		case 3: // OPCODE
			if (c->bDead)
				SetPixelRGB(x, y, RGB(100, 100, 100)); // (grey) dead
			else if (!c->iExternalCycles)
				SetPixelRGB(x, y, RGB(255, 100, 100)); // (pink) spawned, but not yet running
			else switch (c->prevInst % OPCODE_COUNT)
			{
			case MOVE:
				SetPixelRGB(x, y, RGB(0, 255, 0)); // (green)
				break;
			case SHARE:
				SetPixelRGB(x, y, RGB(255, 0, 0)); // (red)
				break;
			case SPAWN:
				SetPixelRGB(x, y, RGB(0, 0, 255)); // (blue)
				break;
			case INFO:
				SetPixelRGB(x, y, RGB(255, 255, 0)); // (yellow)
				break;
			default:
				SetPixelRGB(x, y, RGB(255, 255, 255)); // shouldn't happen
				break;
			}
			break;

		case 4: // spawns
			if (!c->bDead) SetPixelHLS(x, y, c->wMyColor, min(c->iInstructionCounter[SPAWN] * 10, 273));
			break;

		case 5: // frequency of specific instruction
			if (!c->bDead) SetPixelHLS(x, y, c->wMyColor, min(c->iInstructionCounter[LOOK]/* / max(1, c->iExternalCycles)*/ * 10, 273));
			break;

		case 6: // eating
			if (!c->bDead) SetPixelHLS(x, y, c->wMyColor, (min(c->steals, 0xFFFF) / 273));
			break;
		}
	}
}

//template<class T> void World<T>::Init()
void Init()
{
	water = new CELLTYPE **[giWorldWidth];
	land = new int *[giWorldWidth];

	for (int x = 0; x < giWorldWidth; x++)
	{
		water[x] = new CELLTYPE *[giWorldHeight];
		land[x] = new int[giWorldHeight];
		for (int y = 0; y < giWorldHeight; y++)
		{
			water[x][y] = new CELLTYPE;
			water[x][y]->x = x; water[x][y]->y = y;
			water[x][y]->bDead = true;
			water[x][y]->iCycles = 0;
			water[x][y]->iExternalCycles = 0;
			water[x][y]->steals = 0;
			water[x][y]->generation = 0;
			water[x][y]->reg = 0;

			land[x][y] = 0;
		}
	}

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

/*template<class T> T **World<T>::get(const uintptr_t x, const uintptr_t y)
{
	return &water[x][y];
}

template<class T> void World<T>::execute()
{

}*/

int Start()
{
	uintptr_t i, x, y;

	int iAdd = 1; // or -1 depending on amount of living cells

	// total energy currently in the system
	uint64_t iTotalEnergy = 0;

	ghEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	static int foo2[NUMTHREADS];
	for (i = 0; i < NUMTHREADS; i++)
	{
		DWORD foo;
		foo2[i] = i;
		aEventStart[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		aEventDone[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		aThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, &(foo2[i]), 0, &foo);
	}
	InitializeCriticalSection(&criticalSection);

	// Seed the random number generator 
	srand((unsigned int)time(NULL));

	// Reset per-report stat counters 
	//	for (x = 0; x < sizeof(statCounters); ++x)
	//		((uint8_t *)&statCounters)[x] = (uint8_t)0;

	aCheckExec = new cellXY[giWorldWidth * giWorldHeight];

	Init();

	for (x = 0; x < giWorldWidth; x++)
		for (y = 0; y < giWorldHeight; y++)
			if (!(rand() % 9)) // initially create life every 9 cells
			{
				water[x][y]->init();
				for (i = 0; i < OPCODE_COUNT; i++)
					water[x][y]->iInstructionCounter[i] = 0;

				water[x][y]->ID = cellIDCounter++;
				water[x][y]->bDead = false;
				water[x][y]->facing = rand() % 8;
				water[x][y]->parentID = water[x][y]->ID;
				water[x][y]->lineage = water[x][y]->ID; // Lineage is copied in offspring
				water[x][y]->wMyColor = rand() % 240;
				water[x][y]->wChildColor = 0;
				water[x][y]->energy = INFLOW_RATE * 1000;
			}

	// Main loop 
	while (true)
	{
		// Increment clock and run reports periodically 
		// Clock is incremented at the start, so it starts at 1
		ui64Clock++;
		//    if (!(clock % giReportFreq)) doReport(clock);

		// Periodically dump the viable population if defined 
#ifdef DUMP_FREQUENCY
		if (!(clock % DUMP_FREQUENCY)) doDump(clock);
#endif // DUMP_FREQUENCY 

		if (gbThreadStop) break; // breaks the 'for (;;)' loop, frees memory and ends thread
		if (gbRefreshStop) ClearPixels();

#ifdef GRAVITY
		// simulate gravity
		if (!(ui64Clock % 15) && y < giWorldHeight - 1)
			for (x = 0; x < giWorldWidth; x++)
				for (y = giWorldHeight - 3; y > 0; y--) // should be 'y >= 0'?
					if (water[x][y]->energy)
						if (!water[x][y + 1]->energy)
						{
							CELLTYPE *pTmp = water[x][y];
							water[x][y] = water[x][y + 1];
							water[x][y + 1] = pTmp;
						}
#endif GRAVITY

		iTotalEnergy = 0;
		for (x = 0; x < giWorldWidth; x++)
			for (y = 0; y < giWorldHeight; y++)
			{
				if (land[x][y] && gbRefreshStop) SetPixelRGB(x, y, RGB(land[x][y] / 10, land[x][y] / 10, 0));
				if (water[x][y]->energy)
				{
					if (water[x][y]->bDead) water[x][y]->energy -= min(water[x][y]->energy, giDecayRate);
					else
					{
						aCheckExec[iCheckCurrent].x = x;
						aCheckExec[iCheckCurrent].y = y;
						iCheckCurrent++;
						iTotalEnergy += water[x][y]->energy;
						water[x][y]->x = x; water[x][y]->y = y;
					}
					SetPixel(x, y, water[x][y]);
				}
			}
		if (gbRefreshStop) UpdateDisplay();
		WaitForSingleObject(ghEvent, INFINITE);

		// this code adds or subtracts energy based on overgrowth or undergrowth of cells
		// 61440 = 80% of giWorldWidth * giWorldHeight
		// 15360 = 20 % of giWorldWidth * giWorldHeight
		if (iCheckCurrent > giWorldWidth * giWorldHeight * .25) iAdd = -200;
		//if (iCheckCurrent > giWorldWidth * giWorldHeight * .12) iAdd = -200;
		//if (iCheckCurrent > giWorldWidth * giWorldHeight * .06) iAdd = -200;
		else iAdd = 0;
		//giSunEnergy = 80000 / iCheckCurrent;
		//giSunEnergy = (80000000 - min(iTotalEnergy, 80000000)) / iCheckCurrent;
		//giSunEnergy = (giInFlowFreq * 200000 - min(iTotalEnergy, giInFlowFreq * 200000)) / iCheckCurrent;
		giSunEnergy = (iCheckCurrent < (giWorldWidth * giWorldHeight / 4)) ? (giWorldWidth * giWorldHeight / 262) : 0;
		//giSunEnergy = (iCheckCurrent < (giWorldWidth * giWorldHeight / 4)) ? 500 : 0;
		giSunEnergy = (INFLOW_RATE + iAdd) * 10 + (rand() % 100); // rand() to simulate cloudy days
#ifdef GRAVITY
		giSunEnergy = 1850 + (rand() % 100); // '1850' for gravity simulation
#endif // GRAVITY
		guSunVert += 8;
		if (guSunVert >= giWorldWidth)
		{
			guSunVert = guSunVert % giWorldWidth;
			guSunHorz = (guSunHorz + giSunHorzDir);
			if (guSunHorz >= (giWorldHeight * 0.75)) giSunHorzDir = -1;
			else if (guSunHorz <= (giWorldHeight * 0.25)) giSunHorzDir = 1;
			Trace("SunHorz=%d, %d\n", (int)guSunHorz, (int)giSunHorzDir);
		}

		// randomize the order of execution of living cells
		cellXY tmp;
		x = iCheckCurrent;
		while (x > 1)
		{
			y = rand() % x;
			x--;
			tmp = aCheckExec[x];
			aCheckExec[x] = aCheckExec[y];
			aCheckExec[y] = tmp;
		}

		// mutate the current instruction pointer to a random cell
		Cell *pptr;
		if (!(rand() % giMutationRate2))
		{
			pptr = water[aCheckExec[0].x][aCheckExec[0].y];
			pptr->Mutate();
			pptr->wMyColor = (pptr->wMyColor + 20) % 240;
			pptr->lineage = pptr->ID;
			pptr->energy += INFLOW_RATE * 100; // give the new mutation an enormous amount of energy
		}

		for (i = 0; i < NUMTHREADS; i++)
		{
			ResetEvent(aEventDone[i]);
			SetEvent(aEventStart[i]);
		}
		//ThreadFunc();
		WaitForMultipleObjects(NUMTHREADS, aEventDone, TRUE, INFINITE);
	}

	for (x = 0; x < giWorldWidth; ++x)
	{
		for (y = 0; y < giWorldHeight; ++y)
			delete water[x][y];
		delete[] water[x];
		delete[] land[x];
	}
	delete[] water;
	delete[] land;
	delete[] aCheckExec;

	for (i = 0; i < NUMTHREADS; i++)
	{
		CloseHandle(aThread[i]);
		CloseHandle(aEventStart[i]);
		CloseHandle(aEventDone[i]);
	}
	CloseHandle(ghEvent);
	DeleteCriticalSection(&criticalSection);

	exit(0);
	return 0;
}

int ThreadFunc(int* pID)
{
	// Miscellaneous variables used in the loop 
	int xCur, yCur;
	Cell *pCell;

	while (true)
	{
		WaitForSingleObject(aEventStart[*pID], INFINITE);
		ResetEvent(aEventStart[*pID]);

		while (true)
		{
			EnterCriticalSection(&criticalSection);
			if (iCheckCurrent <= 0)
			{
				LeaveCriticalSection(&criticalSection);
				break;
			}
			iCheckCurrent--;
			// Keep track of how many cells have been executed 
			//    statCounters.cellExecutions += 1.0;
			LeaveCriticalSection(&criticalSection);
			xCur = aCheckExec[iCheckCurrent].x;
			yCur = aCheckExec[iCheckCurrent].y;
			pCell = water[xCur][yCur];

			// Core execution loop 
			pCell->iExternalCycles++;
//			int iExecuteAmount = 1;//4 * ((pptr->energy / 100) + 1);//4;//rand() % GENOME_DEPTH;

			// simulating the movement of the sun and the seasons of the year
			int v, h; // 'v' = vertical, 'h' = horizontal
			if (pCell->x < guSunVert) v = min(guSunVert - pCell->x, pCell->x + giWorldWidth - guSunVert);
			else v = min(pCell->x - guSunVert, guSunVert + giWorldWidth - pCell->x);
			//v = (v / 4) * 4 + 1;
			v = (v / 32) * 2 + 1;

#ifdef GRAVITY
			h = pCell->y / 2; // top-down energy from the sun
#else // GRAVITY
			h = abs((int)guSunHorz - pCell->y) / 64; // sun is above the entire grid
#endif // GRAVITY

			if (!pCell->bDead) pCell->energy += giSunEnergy / (v + h) - min(giSunEnergy / (v + h), pCell->iExternalCycles / 100);

			if (!pCell->bDead)
				pCell->Tick(xCur, yCur);
		}

		SetEvent(aEventDone[*pID]);
	}

	return 0;
}
