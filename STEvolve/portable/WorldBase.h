#pragma once

typedef struct _cellXY { int x, y; } cellXY;

class World
{
public:
	// The world is a 2D array of cells 
	Cell ***water;
	int **land;

	std::atomic<uint64_t> cellIDCounter; // This is used to generate unique cell IDs 
	uint64_t ui64Clock = 0; // Clock is incremented on each core loop 

	uint64_t iSunEnergy;
	uint64_t uSunVert = 0;
	uint64_t uSunHorz;
	int iSunHorzDir = 1; // '1' means it's moving south, '-1' mean it's moving north

	Cell *getNeighborPtr(const uint64_t x, const uint64_t y, const uint64_t dir);
	cellXY getNeighborPos(const uint64_t x, const uint64_t y, const uint64_t dir);

	virtual void Init();
	virtual void AdjustEnvironment() = 0;
	virtual void GiveEnergy(Cell *pCell) = 0;
	virtual int Start();
	~World();
};

