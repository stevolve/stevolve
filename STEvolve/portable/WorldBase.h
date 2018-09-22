#pragma once

typedef struct _cellXY { int x, y; } cellXY;

#define NUMTHREADS 1
#define CELLTYPE NeuralBasedCell
//#define CELLTYPE ProgramBasedCell
#define TESTING false
//#define GRAVITY

//template <class T> class World
//{
	extern CELLTYPE ***water;
	extern int **land;

//public:
	extern std::atomic<int> cellIDCounter;

//public:
	//T *getNeighborPtr(const uintptr_t x, const uintptr_t y, const uintptr_t dir);
	Cell *getNeighborPtr(const uintptr_t x, const uintptr_t y, const uintptr_t dir);
	cellXY getNeighborPos(const uintptr_t x, const uintptr_t y, const uintptr_t dir);
//	T **get(const uintptr_t x, const uintptr_t y);

	void Init();
	int Start();
//};

