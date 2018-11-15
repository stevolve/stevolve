#pragma once


class WorldGravity : public World
{
public:
	void AdjustEnvironment();
	void GiveEnergy(Cell *pCell);
};

