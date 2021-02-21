#pragma once

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Constants representing neighbors in the 2D grid. 
#define D_NORTH 0
#define D_NE 1
#define D_EAST 2
#define D_SE 3
#define D_SOUTH 4
#define D_SW 5
#define D_WEST 6
#define D_NW 7

#define TURN   0x11
#define LOOK   0x12

// these instructions cause the cell to break execution
#define MOVE   0x13
#define SHARE  0x14
#define SPAWN  0x15
#define INFO   0x16

#define OPCODE_COUNT 0x17

class Cell
{
public:
//	std::atomic_flag af = ATOMIC_FLAG_INIT;

	// Globally unique cell ID 
	uint64_t ID;
	// ID of the cell's parent 
	uint64_t parentID;
	// Counter for original lineages -- equal to the cell ID of the first cell in the line. 
	uint64_t lineage;
	// Generations start at 0 and are incremented from there. 
	uint64_t generation;
	bool bDead;

	uint8_t wMyColor;
	uint8_t wChildColor;
	uint16_t x, y; // current position of this cell

	uint64_t iCycles; // number of cycles executed for the current cell
	uint64_t iExternalCycles;
	uintptr_t steals;
	uint64_t iInstructionCounter[OPCODE_COUNT]; // number of times each instruction was executed
	uint8_t prevInst; // the previous instruction executed, used in MySetPixel()


	// pointers to attached cells
	//	uint64_t multiCellID; // set to the first cell to spawn

	// Energy level of this cell 
	uint64_t energy = 0;

	// local VM settings for running cell
	////////////
	// The main "register" 
	uintptr_t reg;
	// Which way is the cell facing? 
	uintptr_t facing;

	virtual void init() = 0;
	virtual void Seed() = 0;
	virtual void DrawCell(int, int) = 0;
	virtual void Dump(FILE *d) = 0;
	virtual void Mutate() = 0;
	virtual void Tick(int, int) = 0;

	virtual bool Spawn(int, int) = 0;
	void Look(int, int);
	bool Move(int, int);
	bool Share(int, int);

	virtual void SpawnTest(int, int) = 0;
	virtual void ResetTest() = 0;
};
