#include "Settings.h"

#include <iostream>
#include <fstream>
#include <string>

int giWorldHeight = 256;// WORLD_SIZE_Y;
int giWorldWidth = 512;// WORLD_SIZE_X;
int giNumThreads = 1;
int giCostMoveSucc = 50;
int giCostShareSucc = 25;
int giEatAmount = 20000;
int giCostSpawnFail = 700;
int giCostSpawnSucc = 1000;
int giDecayRate = 10;
int iCostShareFail = giCostShareSucc * 2;// 50;
int iCostMoveFail = giCostMoveSucc;// *2;
int giCostInfo = 5;
int giMutationAmount = 64; // 8;

void ReadSettings()
{
	std::ifstream f("config.txt");
	std::string s;
	//std::stringstream ss(s);
	while (f)
	{
		std::getline(f, s);
		//f.getline(ss, ';');
	}
}