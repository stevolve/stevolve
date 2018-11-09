#pragma once

typedef struct _cellXY { int x, y; } cellXY;

class World
{
public:
	// The world is a 2D array of cells 
	Cell ***water;
	int **land;

	std::atomic<uint64_t> cellIDCounter; // This is used to generate unique cell IDs 

	Cell *getNeighborPtr(const uint64_t x, const uint64_t y, const uint64_t dir);
	cellXY getNeighborPos(const uint64_t x, const uint64_t y, const uint64_t dir);

	void Init();
	virtual int Start();
	~World();
};

