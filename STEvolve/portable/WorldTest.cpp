#include <atomic>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <typeinfo> // for 'typeid()'

#include "CellBase.h"
#include "Settings.h"
#include "CellNeural.h"
#include "CellProgram.h"
#include "WorldBase.h"
#include "WorldTest.h"
#include "Draw.h"

extern World *pWorld;

#define RGB(r,g,b)          ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))

#define NUMTESTS 25

void SetPixel(int x, int y, Cell *c);

extern Cell *gpWatchCell;
extern bool gbRefreshStop;
extern bool gbThreadStop;
extern HANDLE ghEvent;
Cell *pCell1, *pCell2;
Cell *pCells[NUMTESTS];
int iCellBest;
int iHighScore = 0;
int iFailScore = 0;
int xPos, yPos;

#define FOODSTART 10
int foodperc = FOODSTART;

/*bool ProgramBasedCell::Spawn(int xCur, int yCur)
{
	energy -= min(energy, giCostSpawnFail);
	return true;
}*/

void ProgramBasedCell::SpawnTest(int xCur, int yCur)
{
	ProgramBasedCell *tmpptr = (ProgramBasedCell *)pWorld->water[xCur][yCur];
	tmpptr->ID = ++pWorld->cellIDCounter;
	tmpptr->bDead = false;
	tmpptr->parentID = ID;
	tmpptr->lineage = lineage; // Lineage is copied in offspring
	tmpptr->wMyColor = wMyColor;
	tmpptr->iCycles = 0;
	tmpptr->iExternalCycles = 0;
	tmpptr->steals = 0;
	tmpptr->generation = generation + 1;

	memcpy(tmpptr->genome, genome, GENOME_DEPTH);

	memset(tmpptr->iInstructionCounter, 0, OPCODE_COUNT * sizeof(uint64_t));
	memset(tmpptr->linecount, 0, GENOME_DEPTH);
	//memcpy(tmpptr->outputBuf, pCell->outputBuf, GENOME_DEPTH);
	tmpptr->reg = 0;
	tmpptr->ptrPtr = 0;
	tmpptr->StackPtr = 0;
	tmpptr->instPtr = 0;
	tmpptr->facing = facing;
}

void ProgramBasedCell::ResetTest()
{
	instPtr = 0;
	StackPtr = 0;
}

/*bool NeuralBasedCell::Spawn(int xCur, int yCur)
{
	energy -= min(energy, giCostSpawnFail);
	return true;
}*/

void NeuralBasedCell::SpawnTest(int xCur, int yCur)
{
	NeuralBasedCell *tmpptr = (NeuralBasedCell *)pWorld->water[xCur][yCur];
	tmpptr->ID = ++pWorld->cellIDCounter;
	tmpptr->bDead = false;
	tmpptr->parentID = ID;
	tmpptr->lineage = lineage; // Lineage is copied in offspring
	tmpptr->wMyColor = wMyColor;
	tmpptr->iCycles = 0;
	tmpptr->iExternalCycles = 0;
	tmpptr->steals = 0;
	tmpptr->generation = generation + 1;

	memcpy(tmpptr->weights1, weights1, NUMINPUTS * NUMNEURON1 * sizeof(double));
	memcpy(tmpptr->weights2, weights2, NUMNEURON2 * NUMNEURON1 * sizeof(double));
	memcpy(tmpptr->weights3, weights3, NUMOUTPUTS * NUMNEURON2 * sizeof(double));

	memset(tmpptr->iInstructionCounter, 0, OPCODE_COUNT * sizeof(uint64_t));

	tmpptr->reg = 0;
	tmpptr->facing = facing;
}

void NeuralBasedCell::ResetTest()
{
//		for (int i = 0; i < NUMNEURON1; i++)
//			pCell2->weights1[i][0] = 0.0; // start out ignoring any energy
	memset(input, 0, NUMINPUTS * sizeof(double));
}



void WorldTest::ResetWorld()
{
	int x, y;
	for (x = 0; x < giWorldWidth / 2; x++)
		for (y = 0; y < giWorldHeight; y++)
			if (!(rand() % foodperc))// && water[x][y]->bDead) // bDead shouldn't matter as bDead and energy will both be reset later, as long as I don't wipe out the weights
			{
				//water[x][y]->init();
				//for (int i = 0; i < OPCODE_COUNT; i++)
				//	water[x][y]->iInstructionCounter[i] = 0;

				//water[x][y]->ID = cellIdCounter++;
				pWorld->water[x][y]->bDead = true;
pWorld->water[giWorldWidth / 2 + x][y]->bDead = true;
				//water[x][y]->facing = rand() % 8;
				//water[x][y]->parentID = water[x][y]->ID;
				//water[x][y]->lineage = water[x][y]->ID; // Lineage is copied in offspring
				//water[x][y]->wMyColor = rand() % 240;
				//water[x][y]->wChildColor = 0;
				pWorld->water[x][y]->energy = 500;
pWorld->water[giWorldWidth / 2 + x][y]->energy = 500;
			}
			else //if (water[x][y]->bDead)
			{
				pWorld->water[x][y]->energy = 0;
pWorld->water[giWorldWidth / 2 + x][y]->energy = 0;
				pWorld->water[x][y]->bDead = true;
pWorld->water[giWorldWidth / 2 + x][y]->bDead = true;
			}

	// make sure no food it touching the cell when starting
	x = xPos; y = yPos;
	pWorld->water[x - 1][y - 1]->energy = 0;
	pWorld->water[x][y - 1]->energy = 0;
	pWorld->water[x + 1][y - 1]->energy = 0;
	pWorld->water[x - 1][y]->energy = 0;
	pWorld->water[x + 1][y]->energy = 0;
	pWorld->water[x - 1][y + 1]->energy = 0;
	pWorld->water[x][y + 1]->energy = 0;
	pWorld->water[x + 1][y + 1]->energy = 0;
	x = xPos * 3;
	pWorld->water[x - 1][y - 1]->energy = 0;
	pWorld->water[x][y - 1]->energy = 0;
	pWorld->water[x + 1][y - 1]->energy = 0;
	pWorld->water[x - 1][y]->energy = 0;
	pWorld->water[x + 1][y]->energy = 0;
	pWorld->water[x - 1][y + 1]->energy = 0;
	pWorld->water[x][y + 1]->energy = 0;
	pWorld->water[x + 1][y + 1]->energy = 0;
}

void WorldTest::CopyAndReset(int winner)
{
	ResetWorld();
	if (winner == 1) // cell 1 wins
	{
		pCell1->SpawnTest(xPos * 3, yPos);
		pCell2 = pWorld->water[xPos * 3][yPos];
		pCell2->SpawnTest(xPos, yPos);
		pCell1 = pWorld->water[xPos][yPos];
		iCellBest = 1;
		pCell2->Mutate();
	}
	else // cell 2 wins
	{
		pCell2->SpawnTest(xPos, yPos);
		pCell1 = pWorld->water[xPos][yPos];
		pCell1->SpawnTest(xPos * 3, yPos);
		pCell2 = pWorld->water[xPos * 3][yPos];
		iCellBest = 2;
		pCell1->Mutate();
	}

	pCell1->ResetTest();
	pCell2->ResetTest();

	pCell1->reg = pCell2->reg = 0;
	// todo: initialize other variables?

	pCell1->facing = pCell2->facing = 0;
	pCell1->energy = pCell2->energy = giEnergyInflow;
	pCell1->bDead = pCell2->bDead = false;
	pCell1->x = xPos; pCell1->y = yPos;
	pCell2->x = xPos * 3; pCell2->y = yPos;
}

void WorldTest::CopyAndReset2(int winner, int loser)
{
	ResetWorld();
	int xFoo = loser * giWorldWidth / NUMTESTS;
	pCells[winner]->SpawnTest(xFoo, yPos);
	pCells[loser] = pWorld->water[xFoo][yPos];
//	pCells[loser]->Mutate();

	pCells[loser]->ResetTest();
	pCells[loser]->reg = 0;
	pCells[loser]->energy = giEnergyInflow;
	pCells[loser]->bDead = false;
	pCells[loser]->x = xFoo; pCells[loser]->y = yPos;
}

int WorldTest::Start()
{
	// hard-code the height/width because a larger world won't matter for the test
	giWorldHeight = 256;
	giWorldWidth = 512;

	Init();
	giCostSpawnFail = 50;
	giCostInfo = 100;
	giEnergyInflow = 3000;

// remove the 'land' (should probably do this another way)
for (int x = 0; x < giWorldWidth; x++)
	for (int y = 0; y < giWorldHeight; y++)
		pWorld->land[x][y] = 0;

	xPos = giWorldWidth / 4;
	yPos = giWorldHeight / 2;
	ResetWorld();

	MainTestLoop(0);

	return 0;
}

void WorldTest::TestInit(Cell *pCell)
{
	pCell->init();
	for (int i = 0; i < OPCODE_COUNT; i++)
		pCell->iInstructionCounter[i] = 0;

	pCell->ID = pWorld->cellIDCounter++;
	pCell->bDead = false;
	pCell->facing = rand() % 8;
	pCell->parentID = pCell->ID;
	pCell->lineage = pCell->ID; // Lineage is copied in offspring
	pCell->wMyColor = rand() % 240;
	pCell->wChildColor = 0;
	pCell->energy = giEnergyInflow;
}

int WorldTest::MainTestLoop(int* pID)
{
	yPos = giWorldHeight / 2;
	int i;

	xPos = -1;
	for (i = 0; i < NUMTESTS; i++)
	{
		xPos += giWorldWidth / NUMTESTS;
		pCells[i] = pWorld->water[xPos][yPos];
		pCells[i]->x = xPos; pCells[i]->y = yPos;
		TestInit(pCells[i]);
	}

	xPos = giWorldWidth / 4;

	iCellBest = 1;

int iAllCycles = 0;

	pCell1 = pCells[0];
	pCell2 = pCells[1];
pCell1->Seed();
//pCell2->Seed();

while (!gbThreadStop)
	{
/*		for (i = 0; i < NUMTESTS; i++)
		{
			pCells[i]->Execute(pCells[i]->x, pCells[i]->y);
			// these should probably be moved into Cell::Execute()
			pCells[i]->iExternalCycles++;
		}*/
		pCell1->Tick(pCell1->x, pCell1->y);
		pCell2->Tick(pCell2->x, pCell2->y);

		// here I could either try to subclass the Cell to change Cell::Spawn() to not actually spawn
		// or have a flag for test-mode in the Cell class to not spawn
		// or just remove the child after the spawn:
		// if the instruction was SPAWN, just kill the child (I know, sad) to it doesn't try to eat it
		if (pCell1->prevInst == SPAWN)
		{
			Cell *pTemp = pWorld->getNeighborPtr(pCell1->x, pCell1->y, pCell1->facing);
			pTemp->bDead = true;
			pTemp->energy = 0;
		}
		if (pCell2->prevInst == SPAWN)
		{
			Cell *pTemp = pWorld->getNeighborPtr(pCell2->x, pCell2->y, pCell2->facing);
			pTemp->bDead = true;
			pTemp->energy = 0;
		}

		// these should probably be moved into Cell::Execute()
		pCell1->iExternalCycles++;
		pCell2->iExternalCycles++;

		ClearPixels();
		for (int x = 0; x < giWorldWidth; x++)
			for (int y = 0; y < giWorldHeight; y++)
			{
				if (pWorld->water[x][y]->energy)
				{
					SetPixel(x, y, pWorld->water[x][y]);
					//if (water[x][y]->bDead)
					//	SetPixelRGB(x, y, RGB(min(100, water[x][y]->energy), min(100, water[x][y]->energy), min(100, water[x][y]->energy)));
					//else
					//	SetPixelHLS(x, y, water[x][y]->wMyColor, (min(water[x][y]->energy, 0xFFFF) / 273)); // lineage
				}
			}
		gpWatchCell = iCellBest == 1 ? pCell1 : pCell2;
		if (gbRefreshStop) UpdateDisplay();
/*if (iAllCycles++ > 10)
{
	int least = MAXINT;
	int iLeast = 0;
	int most = 0;
	int iMost = 0;
	for (i = 0; i < NUMTESTS; i++)
	{
		if (pCells[i]->steals < least)
		{
			least = pCells[i]->steals;
			iLeast = i;
		}
		if (pCells[i]->steals > most)
		{
			most = pCells[i]->steals;
			iMost = i;
		}
	}
	Trace("%d steals=%d\n", iMost, pCells[iMost]->steals);
	int foo = 0;
	for (i = 0; i < NUMTESTS; i++)
	{
		if (i != iMost)
		{
			CopyAndReset2(iMost, i);
			pCells[i]->Mutate();
			foo = i; // a non-iMost cell
		}
	}
	CopyAndReset2(foo, iMost);
	iAllCycles = 0;
}*/

//		if (pCell1->energy && pCell2->energy)
		if (pCell1->energy || pCell2->energy)
		{
			if (pCell1->steals > pCell2->steals) // pCell1 steals first
			{
				if (iCellBest != 1) 
					Trace("Cell1 eats, generation=%u\n", (unsigned int)pCell1->ID);
				foodperc++;
				CopyAndReset(1);
				iFailScore = 0;
			}
			else if (pCell1->steals < pCell2->steals) // pCell2 steals first
			{
				if (iCellBest != 2) 
					Trace("Cell2 eats, generation=%u\n", (unsigned int)pCell2->ID);
				foodperc++;
				CopyAndReset(2);
				iFailScore = 0;
			}
			else if (pCell1->steals || pCell2->steals)
			{
				foodperc++;
				iFailScore = 0;
			}
		}
		/*else if (pCell1->energy > pCell2->energy)
		{
			CopyAndReset(1);
		}
		else if (pCell1->energy < pCell2->energy)
		{
			CopyAndReset(2);
		}*/
		else if (!pCell1->energy && !pCell2->energy)
		/*{
			if (pCell1->iCycles > pCell2->iCycles)
			{
				CopyAndReset(1);
			}
			else if (pCell1->iCycles < pCell2->iCycles)
			{
				CopyAndReset(2);
			}
			else if (pCell1->iInstructionCounter[MOVE] > pCell2->iInstructionCounter[MOVE])
			{
				CopyAndReset(1);
			}
			else if (pCell1->iInstructionCounter[MOVE] < pCell2->iInstructionCounter[MOVE])
			{
				CopyAndReset(2);
			}
			else if (pCell1->iInstructionCounter[LOOK] > pCell2->iInstructionCounter[LOOK])
			{
				CopyAndReset(1);
			}
			else if (pCell1->iInstructionCounter[LOOK] < pCell2->iInstructionCounter[LOOK])
			{
				CopyAndReset(2);
			}
			else*/ // both are without energy
			{
bool foo = gbRefreshStop;
gbRefreshStop = true;
				if (foodperc > iHighScore) Trace("highscore=%d foodperc=%d best=%d externalcycles=%d looks=%d moves=%d generation=%u\n", 
												iHighScore, foodperc, iCellBest, (int)pCell1->iExternalCycles, 
												(int)pCell1->iInstructionCounter[LOOK], (int)pCell1->iInstructionCounter[MOVE], (unsigned int)pCell1->ID);
				iHighScore = max(foodperc, iHighScore);
				iFailScore++;
				if (iFailScore > 10)
				{
					foodperc = max(FOODSTART, foodperc - (int)(iFailScore / 10)); // increase food if it keeps failing
					Trace("increasing food to:%d due to many failures:%d, highscore=%d\n", foodperc, (int)(iFailScore / 10), iHighScore);
				}
gbRefreshStop = foo;
				//foodperc = FOODSTART;
				//foodperc = max(FOODSTART, foodperc - 1);
				CopyAndReset(iCellBest);

				// just an experiment to ensure the mutation is significant:
				/*if (typeid(pCell1).name() == "ProgramBasedCell")
				{
					Cell *pNotBest = (iCellBest == 1) ? pCell2 : pCell1;
					((ProgramBasedCell *)pNotBest)->genome[0] = rand();
				}*/
			}
		//}

		//WaitForSingleObject(ghEvent, INFINITE);
	}

	return 0;
}