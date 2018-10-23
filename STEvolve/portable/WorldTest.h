#pragma once

class WorldTest : public World
{
	void ResetWorld();
	void CopyAndReset(int);
	void CopyAndReset2(int, int);
	void TestInit(Cell *);
	int MainTestLoop(int *);
public:
	int Start();
};

