#pragma once

class WorldToroidal : public World
{
public:
	//int **land;

	void AdjustEnvironment();
	void GiveEnergy(Cell *pCell);
	void Init();
};

