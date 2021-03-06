#include <atomic>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for min()/max()
#include <string.h>
#include <math.h>

#include "Settings.h"
#include "CellBase.h"
#include "CellNeural.h"
#include "WorldBase.h"
#include "Draw.h"

extern World *pWorld;

#define RGB(r,g,b)          ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))

#define TURNMIN -0.3
#define TURNMAX 0.3
#define LOOKMIN -0.3
#define LOOKMAX 0.3
#define ACTIONMIN -0.3
#define ACTIONMAX 0.3

// sigmoid function
//#define ACTIVATION(a) 1.0 / (1.0 + exp(-a)) 
// tanh function
#define ACTIVATION(a) 2.0 / (1.0 + exp(-a * 2)) - 1 

void NeuralBasedCell::init()
{
	int i, j;
	for (i = 0; i < NUMINPUTS; i++)
		input[i] = 0;

	for (i = 0; i < NUMNEURON1; i++)
		for (j = 0; j < NUMINPUTS; j++)
			weights1[i][j] = rand() % 24 - 12;
	for (i = 0; i < NUMNEURON2; i++)
		for (j = 0; j < NUMNEURON1; j++)
			weights2[i][j] = rand() % 24 - 12;
	for (i = 0; i < NUMOUTPUTS; i++)
		for (j = 0; j < NUMNEURON2; j++)
			weights3[i][j] = rand() % 24 - 12;
}

void NeuralBasedCell::Dump(FILE *d)
{
}

void NeuralBasedCell::DrawCell(int left, int top)
{
	int i, j, k;
	for (i = 0; i < NUMINPUTS; i++)
		SetPixelRGB(left, top + i * 2, RGB(abs(input[i]) * 250, abs(input[i]) * 250, abs(input[i]) * 250));
	for (k = 0, i = 0; i < NUMNEURON1; i++)
	{
		for (j = 0; j < NUMINPUTS; j++, k++)
			SetPixelRGB(left + 2, top + k, weights1[i][j] < 0 ? RGB(-weights1[i][j] * 90, 0, 0) : RGB(0, 0, weights1[i][j] * 90));
		//SetPixelRGB(left + 2, top + k + 1, RGB(255, 255, 255)); // separator
		k += 2;
	}
	for (k = 0, i = 0; i < NUMNEURON2; i++)
	{
		for (j = 0; j < NUMNEURON1; j++, k++)
			SetPixelRGB(left + 4, top + k, weights2[i][j] < 0 ? RGB(-weights2[i][j] * 90, 0, 0) : RGB(0, 0, weights2[i][j] * 90));
		//SetPixelRGB(left + 4, top + k + 1, RGB(255, 255, 255)); // separator
		k += 2;
	}
	for (k = 0, i = 0; i < NUMOUTPUTS; i++)
	{
		for (j = 0; j < NUMNEURON2; j++, k++)
			SetPixelRGB(left + 6, top + k, weights3[i][j] < 0 ? RGB(-weights3[i][j] * 90, 0, 0) : RGB(0, 0, weights3[i][j] * 90));
		//SetPixelRGB(left + 6, top + k + 1, RGB(255, 255, 255)); // separator
		k += 2;
	}
	if (output[0] < TURNMIN) SetPixelRGB(left + 8, top + 0 * 2, RGB(-output[0] * 250, 0, 0));				// red: turn left
	else if (output[0] > TURNMAX) SetPixelRGB(left + 8, top + 0 * 2, RGB(0, output[0] * 250, 0));			// green: turn right
	else SetPixelRGB(left + 8, top + 0 * 2, RGB(output[0] * 250, output[0] * 250, output[0] * 250));		// white: no turn
	if (output[1] < LOOKMIN) SetPixelRGB(left + 8, top + 1 * 2, RGB(-output[1] * 250, 0, 0));				// red: look
	else if (output[1] > LOOKMAX)
	{
		SetPixelRGB(left + 8, top + 1 * 2, RGB(0, output[1] * 250, 0));										// green: action
		if (output[2] < ACTIONMIN) SetPixelRGB(left + 8, top + 2 * 2, RGB(0, -output[2] * 250, 0));				// green: move
		else if (output[2] > ACTIONMAX) SetPixelRGB(left + 8, top + 2 * 2, RGB(output[2] * 250, 0, 0));			// red: share
		else SetPixelRGB(left + 8, top + 2 * 2, RGB(0, 0, abs(output[2]) * 250));								// blue: spawn
	}	
	else SetPixelRGB(left + 8, top + 1 * 2, RGB(abs(output[1]) * 250, abs(output[1]) * 250, 0));			// yellow: break
	for (i = 3; i < NUMOUTPUTS; i++)
		SetPixelRGB(left + 8, top + i * 2, output[i] < 0 ? RGB(-output[i] * 250, 0, 0) : RGB(0, 0, output[i] * 250));
}

void NeuralBasedCell::Mutate()
{
	int i, j;
	for (i = 0; i < NUMNEURON1; i++)
		for (j = 0; j < NUMINPUTS; j++)
			if (!(rand() % giMutationAmount)) weights1[i][j] = rand() % 24 - 12;
	for (i = 0; i < NUMNEURON2; i++)
		for (j = 0; j < NUMNEURON1; j++)
			if (!(rand() % giMutationAmount)) weights2[i][j] = rand() % 24 - 12;
	for (i = 0; i < NUMOUTPUTS; i++)
		for (j = 0; j < NUMNEURON2; j++)
			if (!(rand() % giMutationAmount)) weights3[i][j] = rand() % 24 - 12;
}

void NeuralBasedCell::Turn(int reg)
{
	switch (facing)
	{
	case D_NORTH: facing = (reg & 1) ? D_NE : D_NW; break;
	case D_NE:    facing = (reg & 1) ? D_EAST : D_NORTH; break;
	case D_EAST:  facing = (reg & 1) ? D_SE : D_NE; break;
	case D_SE:    facing = (reg & 1) ? D_SOUTH : D_EAST; break;
	case D_SOUTH: facing = (reg & 1) ? D_SW : D_SE; break;
	case D_SW:    facing = (reg & 1) ? D_WEST : D_SOUTH; break;
	case D_WEST:  facing = (reg & 1) ? D_NW : D_SW; break;
	case D_NW:    facing = (reg & 1) ? D_NORTH : D_WEST; break;
	}
	energy -= MIN(energy, 2);
}

bool NeuralBasedCell::Spawn(int xCur, int yCur)
{
	bool bRet = false;
//	while (af.test_and_set()) ; // spin-lock
	// Copy outputBuf into neighbor if access is permitted and there
	// is energy there to make something happen. There is no need
	// to copy to a cell with no energy, since anything copied there
	// would never be executed and then would be replaced with random
	// junk eventually. 
	NeuralBasedCell *tmpptr = (NeuralBasedCell *)pWorld->getNeighborPtr(xCur, yCur, facing);
//	while (tmpptr->af.test_and_set()) ; // spin-lock
	if (!tmpptr->energy && energy >= (giCostSpawnSucc + 10)) // '+ 10' to ensure both have some energy
	{
		tmpptr->ID = ++pWorld->cellIDCounter;
		tmpptr->bDead = false;
		tmpptr->parentID = ID;
		tmpptr->lineage = lineage; // Lineage is copied in offspring
								   //tmpptr->wMyColor = (wMyColor + wChildColor++) % 240;
								   //tmpptr->wMyColor = (wMyColor + 1) % 240;
		tmpptr->wMyColor = wMyColor;
		//tmpptr->wChildColor = 0;
		tmpptr->iCycles = 0;
		tmpptr->iExternalCycles = 0;
		tmpptr->steals = 0;
		tmpptr->generation = generation + 1;
		memset(tmpptr->iInstructionCounter, 0, OPCODE_COUNT * sizeof(uint64_t));

		memcpy(tmpptr->weights1, weights1, NUMINPUTS * NUMNEURON1 * sizeof(int));
		memcpy(tmpptr->weights2, weights2, NUMNEURON2 * NUMNEURON1 * sizeof(int));
		memcpy(tmpptr->weights3, weights3, NUMOUTPUTS * NUMNEURON2 * sizeof(int));

		tmpptr->reg = 0;
		switch (facing) // child is born "head first"
		{
		case D_NORTH: tmpptr->facing = D_SOUTH;  break;
		case D_NE:    tmpptr->facing = D_SW;  break;
		case D_EAST:  tmpptr->facing = D_WEST;  break;
		case D_SE:    tmpptr->facing = D_NW;  break;
		case D_SOUTH: tmpptr->facing = D_NORTH;    break;
		case D_SW:    tmpptr->facing = D_NE;  break;
		case D_WEST:  tmpptr->facing = D_EAST; break;
		case D_NW:    tmpptr->facing = D_SE;  break;
		}

		energy -= giCostSpawnSucc; // we already confirmed energy > giCostSpawnSucc + 10, no min() needed
		tmpptr->energy = energy * .5;
		energy = energy * .5;
		bRet = true;
	}
	else
	{
		energy -= MIN(energy, giCostSpawnFail);
		bRet = false;
	}

//	tmpptr->af.clear();
//	af.clear();
	return bRet;
}

void NeuralBasedCell::Tick(int xCur, int yCur)
{
	// INPUT
	//		LOOK-RESULT
	//		ENERGY
	// OUTPUT-CONTINUES
	//		LOOK
	//		TURN
	// OUTPUT-BREAKS
	//		EAT
	//		SPAWN
	//		MOVE
	while (energy)
	{
		int i, j;
		double a;

		//input[0] = energy / 10000.0f;
		input[0] = MIN(energy, 10000) / 10000.0f;
		//Look(xCur, yCur);
		//input[1] = reg / 10.0f;
		//input[1] = reg ? (11 - reg) / 10.0f : 0.0f;


		iCycles++;

		for (i = 0; i < NUMNEURON1; i++)
		{
			a = weights1[i][0] * input[0];
			a += weights1[i][1] * input[1];
			a += weights1[i][2] * input[2];
			a += weights1[i][3] * input[3];
			a += weights1[i][4] * input[4];
			a = ACTIVATION(a);
			neuron1[i] = a;
		}

		for (i = 0; i < NUMNEURON2; i++)
		{
/*			a = 0;
			for (j = 0; j < NUMNEURON1; j++)
				a += weights2[i][j] * neuron1[j];
			a = ACTIVATION(a);
			//output[i] = a;
			neuron2[i] = a;
*/
neuron2[i] = neuron1[i]; // this just copies the neuron, effectively skipping the layer. comment out to allow layer
		}

		for (i = 0; i < NUMOUTPUTS; i++)
		{
			a = 0;
			for (j = 0; j < NUMNEURON2; j++)
				a += weights3[i][j] * neuron2[j];
			a = ACTIVATION(a);
			output[i] = a;
		}

		input[2] = output[3];
		input[3] = output[4];
		input[4] = output[5];

		if (output[0] < TURNMIN) Turn(0);
		else if (output[0] > TURNMAX) Turn(1);

		if (output[1] < LOOKMIN)
		{
			Look(xCur, yCur);
			input[1] = reg / 10.0f;
			//input[1] = reg ? (11 - reg) / 10.0f : 0.0f;
			iInstructionCounter[LOOK]++;
			prevInst = LOOK;
		}
		else if (output[1] > LOOKMAX)
		{
			if (output[2] < ACTIONMIN)
			{
				if (Move(xCur, yCur))
				{
					iInstructionCounter[MOVE]++;
					prevInst = MOVE;
					break;
				}
			}
			else if (output[2] > ACTIONMAX)
			{
				if (Share(xCur, yCur))
				{
					// 'SHARE' is also captured in pCell->steals
					//iInstructionCounter[SHARE]++; 
					prevInst = SHARE;
					break;
				}
			}
			else // output[2] == 0
			{
				if (Spawn(xCur, yCur))
				{
					iInstructionCounter[SPAWN]++;
					prevInst = SPAWN;
					break;
				}
			}
		}
		else
		{
			energy -= MIN(energy, giCostInfo); // just to make sure it doesn't infinite loop
			prevInst = INFO;
			break;
		}
	}
}
