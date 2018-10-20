#pragma once

typedef struct _cellXY { int x, y; } cellXY;

//template <class T> class World
class World
{
public:
	// The world is a 2D array of cells 
	Cell ***water;
	int **land;

	std::atomic<uint64_t> cellIDCounter; // This is used to generate unique cell IDs 

	//T *getNeighborPtr(const uintptr_t x, const uintptr_t y, const uintptr_t dir);
	Cell *getNeighborPtr(const uintptr_t x, const uintptr_t y, const uintptr_t dir);
	cellXY getNeighborPos(const uintptr_t x, const uintptr_t y, const uintptr_t dir);
//	T **get(const uintptr_t x, const uintptr_t y);

	void Init();
	virtual int Start();
};

