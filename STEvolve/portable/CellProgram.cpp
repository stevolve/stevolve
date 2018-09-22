#include <atomic>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for min()/max()
#include <string.h>

#include "Settings.h"
#include "CellBase.h"
#include "CellNeural.h"
#include "CellProgram.h"
#include "WorldBase.h"
#include "Draw.h"

#define RGB(r,g,b)          ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))

void ProgramBasedCell::init()
{
	for (int i = 0; i < GENOME_DEPTH; ++i)
	{
		genome[i] = rand();//getRandomOpCode();
		outputBuf[i] = genome[i];
		linecount[i] = 0;
	}
}

void ProgramBasedCell::Dump(FILE *d)
{
	for (int i = 0; i < GENOME_DEPTH; ++i)
	{
		uintptr_t inst = genome[instPtr] & 0xf;
		fprintf(d, "%x", inst);
		if (++instPtr >= GENOME_DEPTH)
			instPtr = 0;
	}
};

int GetColor(uint8_t i)
{
	switch (i % 22)
	{
	case MOVE: return 20; break;
	case PUSH:
	case POP: return 40; break;
	case INC:
	case DEC:
	case SHIFTL:
	case NOT:
	case XOR:
	case AND:
	case OR: return 60; break;
	case READG:
	case WRITEG:
	case READB:
	case WRITEB: return 80; break;
	case IF: return 100; break;
	case LOADP: return 120; break;
	case TURN: return 140; break;
	case LOOK: return 160; break;
	case SHARE: return 180; break;
	case SPAWN: return 200; break;
	case INFO: return 220; break;
	default: return 240;
	}
}

void ProgramBasedCell::DrawCell(int left, int top)
{
	int i, lum, color;
	left++; // makes it easier to see

	for (i = 0; i < GENOME_DEPTH; i++)
	{
		color = GetColor(genome[i]);

		if (linecount[i]) lum = __min(linecount[i] + 20, 220);
		else lum = 10;

		SetPixelHLS(left + i / giWorldHeight, top + i % giWorldHeight, color, lum);
	}

	// highlight the location of the current instruction pointer
	i = instPtr % GENOME_DEPTH;
	SetPixelRGB(left + i / giWorldHeight, top + i % giWorldHeight, RGB(255, 255, 255));

	for (i = 0; i < StackPtr; i++)
		SetPixelRGB(left + i / giWorldHeight + 2, top + i, RGB(128, 0, 0));
}

void ProgramBasedCell::Mutate()
{
	instPtr = rand() % GENOME_DEPTH;

	int x;
	for (x = 0; x < GENOME_DEPTH; x++)
	{
		if (!(rand() % giMutationAmount))
		{
			genome[x] = rand();//getRandomOpCode();
			outputBuf[x] = genome[x];
		}
	}
	// just mutate the first 8 cells since that is the most often executed code
	/*	for (x = 0; x < 8; x++)
	{
	genome[x] = rand();
	outputBuf[x] = genome[x];
	}*/
}

bool ProgramBasedCell::Spawn(int xCur, int yCur)
{
	// Copy outputBuf into neighbor if access is permitted and there
	// is energy there to make something happen. There is no need
	// to copy to a cell with no energy, since anything copied there
	// would never be executed and then would be replaced with random
	// junk eventually. See the seeding code in the main loop above. 
	ProgramBasedCell *tmpptr = (ProgramBasedCell *)getNeighborPtr(xCur, yCur, facing);
	if (!tmpptr->energy && energy >= (giCostSpawnSucc + 2)) // '+ 2' to ensure both have some energy
	{
		tmpptr->ID = ++cellIDCounter;
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
		memcpy(tmpptr->genome, outputBuf, GENOME_DEPTH);
		memset(tmpptr->iInstructionCounter, 0, OPCODE_COUNT * sizeof(uint64_t));
		memset(tmpptr->linecount, 0, GENOME_DEPTH);
		memcpy(tmpptr->outputBuf, outputBuf, GENOME_DEPTH);
		tmpptr->reg = 0;
		tmpptr->ptrPtr = 0;
		tmpptr->StackPtr = 0;
		tmpptr->instPtr = 0;
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
		tmpptr->energy = energy * .5;
		energy = energy * .5;

		energy -= __min(energy, giCostSpawnSucc);
		return true;
	}
	else
	{
		energy -= __min(energy, giCostSpawnFail);
		return false;
	}
}

void ProgramBasedCell::Tick(int xCur, int yCur)
{
	uintptr_t inst, tmp;

	int iExecuteAmount = 1;//4 * ((pptr->energy / 100) + 1);//4;//rand() % GENOME_DEPTH;

	while (energy && iExecuteAmount)
	{
		// Get the next instruction 
		//		inst = (pCell->genome[pCell->instPtr]) % OPCODE_COUNT;
		if (genome[instPtr] < 192)
			inst = genome[instPtr] / 19.2;
		else if (genome[instPtr] < 247)
			inst = (genome[instPtr] - 192) / 6.1111 + 0xa; // '6.1111' = (247 - 192) / 9
		else
			inst = (genome[instPtr] - 247) / 2 + 0x12;
		prevInst = inst;
		iCycles++;

		// this is an unlikely changing mutation
		if (!(rand() % 1000))
		{
			tmp = rand() % GENOME_DEPTH;
			outputBuf[tmp] = rand();
			wMyColor = (wMyColor + 1) % 240;
			lineage = ID;
		}

		// Execute the instruction 
		// Keep track of execution frequencies for each instruction 
		//        statCounters.instructionExecutions[inst] += 1.0;
		iInstructionCounter[inst]++;
		linecount[instPtr]++;
		if (linecount[instPtr] == 255)
			linecount[instPtr] = 128; // so that the high order bit is always set

		switch (inst)
		{
		case MOVE: // MOVE: 
		{
			if (Move(xCur, yCur)) iExecuteAmount = 0;
			instPtr = (instPtr + 1) % GENOME_DEPTH;
		}
		break;
		case PUSH: // PUSH: reg onto stack
			if (StackPtr < STACK_DEPTH - 1)
				Stack[StackPtr++] = reg;
			else energy -= __min(energy, 400);
			energy -= __min(energy, (StackPtr / 32) + 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case POP: // POP: reg from stack
			if (StackPtr)
				reg = Stack[--StackPtr];
			else energy -= __min(energy, 5);
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case INC: // INC: Increment the register 
			reg = (reg + 1) & 0xffff;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case DEC: // DEC: Decrement the register 
			if (reg) reg--;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case READG: // READG: Read into the register from genome 
			reg = genome[ptrPtr];// & 0xf;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case WRITEG: // WRITEG: Write out from the register to genome 
					 //			if (!bTesting) // we don't want to overwrite while testing
					 //				genome[ptrPtr] = reg;// & 0xf;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case READB: // READB: Read into the register from buffer 
			reg = outputBuf[ptrPtr];// & 0xf;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case WRITEB: // WRITEB: Write out from the register to buffer 
			outputBuf[ptrPtr] = reg;// & 0xf;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case IF: // IF/GOTO:
			if (reg)
			{
				//if (instPtr < (GENOME_DEPTH - 1)) 
				//{
				instPtr = genome[instPtr + 1] % GENOME_DEPTH;
				//instPtr--; // because it will be added to at the end of this switch statement
				//}
				//else 
				//{
				//instPtr = genome[EXEC_START_WORD];
				// the above statement it not necessary,
				// the next statement will cause the instPtr increment at the end of this
				// switch statement to set it to EXEC_START_WORD
				//	instPtr = GENOME_DEPTH - 1;
				//}
			}
			else
				instPtr = (instPtr + 1) % GENOME_DEPTH;
			//{
			//	if (instPtr < (GENOME_DEPTH - 1)) instPtr++; 
			//	else instPtr = EXEC_START_WORD; // will be incremented at the end of this switch statement
			//}
			energy -= __min(energy, 1);
			break;
		case JUMP:
			instPtr = reg % GENOME_DEPTH;
			energy -= __min(energy, 1);
			//instPtr--; // because it will be added to at the end of this switch statement
			//instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
			//		  case DJNZ:
			//			  if (!reg)
			//			  {
			//				  instPtr = Stack[StackPtr];
			//
			//			  }
			//			  energy -= __min(energy) % GENOME_DEPTH;
			//		  break;
		case LOADP: // Load with current value of register: 
			ptrPtr = reg % GENOME_DEPTH;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case TURN: // TURN: Turn in the direction specified by register 
				   //facing = reg & 3;
			reg >>= 1; // SHIFTR: Shift right
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case XCHG: // XCHG: Load the reg with the current instruction pointer
				   //			tmp = reg;
			if (instPtr < (GENOME_DEPTH - 1)) instPtr++;
			else instPtr = 0;
			reg = genome[instPtr];
			//			genome[instPtr] = tmp;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case LOOK: // LOOK: 
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
			Look(xCur, yCur);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case SHARE: // SHARE: Equalize energy between self and neighbor if allowed 
			if (Share(xCur, yCur)) iExecuteAmount = 0;
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;
		case SPAWN: // SPAWN
			if (Spawn(xCur, yCur)) iExecuteAmount = 0;
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		case SHIFTL: // SHIFTL: Shift left
			reg <<= 1;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		case NOT:
			reg = ~reg;
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		case XOR:
			reg ^= Stack[StackPtr];
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		case AND:
			reg &= Stack[StackPtr];
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		case OR:
			reg |= Stack[StackPtr];
			energy -= __min(energy, 1);
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		case INFO: // get information about yourself
			reg = energy;
			energy -= __min(energy, giCostInfo);
			iExecuteAmount = 0;
			instPtr = (instPtr + 1) % GENOME_DEPTH;
			break;

		default: // this should never be reached
			energy = 0;
			//Trace("!!!invalid OpCode\n");
			break;
		}

		// Advance the shift and word pointers, and loop around
		// to the beginning at the end of the genome. 
		//if (++instPtr >= GENOME_DEPTH)
		//	  instPtr = EXEC_START_WORD;
	}
}
