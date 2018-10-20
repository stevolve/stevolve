#include <atomic>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for min()/max()

#include "Settings.h"
#include "CellBase.h"
#include "CellNeural.h"
#include "CellProgram.h"
#include "WorldBase.h"

extern World *pWorld;

extern int iCostMoveFail;

void Cell::Look(int xCur, int yCur)
{
	uintptr_t tempx, tempy;
	tempx = xCur; tempy = yCur;
	reg = 0; // nothing was found if it remains 0
	for (int i = 0; i < 10; i++) // only "look" max 10 cells away
	{
		Cell *tmpptr = pWorld->getNeighborPtr(tempx, tempy, facing);
		if (tmpptr->energy)
		{
			reg = i + 1;
			break;
		}

		switch (facing)
		{
		case D_NORTH:
			(tempy) ? tempy-- : tempy = giWorldHeight - 1;
			break;
		case D_NE:
			(tempx == giWorldWidth - 1) ? tempx = 0 : tempx++;
			(tempy) ? tempy-- : tempy = giWorldHeight - 1;
			break;
		case D_EAST:
			(tempx == giWorldWidth - 1) ? tempx = 0 : tempx++;
			break;
		case D_SE:
			(tempx == giWorldWidth - 1) ? tempx = 0 : tempx++;
			(tempy == giWorldHeight - 1) ? tempy = 0 : tempy++;
			break;
		case D_SOUTH:
			(tempy == giWorldHeight - 1) ? tempy = 0 : tempy++;
			break;
		case D_SW:
			(tempx) ? tempx-- : tempx = giWorldWidth - 1;
			(tempy == giWorldHeight - 1) ? tempy = 0 : tempy++;
			break;
		case D_WEST:
			(tempx) ? tempx-- : tempx = giWorldWidth - 1;
			break;
		case D_NW:
			(tempx) ? tempx-- : tempx = giWorldWidth - 1;
			(tempy) ? tempy-- : tempy = giWorldHeight - 1;
			break;
		default: // this should never be reached
			tempx = xCur;
			tempy = yCur;
			break;
		}
	}
	energy -= __min(energy, 2);
}

bool Cell::Move(int xCur, int yCur)
{
	Cell *tmpptr = pWorld->getNeighborPtr(xCur, yCur, facing);
	if (!tmpptr->energy)
	{
		*((Cell **)&pWorld->water[xCur][yCur]) = tmpptr;
		cellXY cell = pWorld->getNeighborPos(xCur, yCur, facing);
		*((Cell **)&pWorld->water[cell.x][cell.y]) = this;
tmpptr->x = xCur; tmpptr->y = yCur;
		this->x = cell.x; this->y = cell.y;

		energy -= __min(energy, giCostMoveSucc + pWorld->land[x][y]);
		return true;
	}
	else
	{
		energy -= __min(energy, iCostMoveFail);
		return false;
	}
};

bool Cell::Share(int xCur, int yCur)
{
	Cell *tmpptr = pWorld->getNeighborPtr(xCur, yCur, facing);
	if (tmpptr->energy)
	{
		tmpptr->bDead = true;
		uintptr_t tmp = __min(tmpptr->energy, __max(0, giEatAmount - (int)(iExternalCycles / 10000)));
		tmpptr->energy -= tmp;
		energy += tmp;
		steals += tmp;

		energy -= __min(energy, giCostShareSucc);
		return true;
	}
	else
	{
		energy -= __min(energy, giCostShareSucc);// iCostShareFail);
		return false;
	}
}
