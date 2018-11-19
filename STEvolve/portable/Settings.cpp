#include "Settings.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <thread>

int giWorldHeight = 256;// WORLD_SIZE_Y;
int giWorldWidth = 512;// WORLD_SIZE_X;
int giNumThreads = 1;
unsigned int guMaxThreads = 1;
int giEnergyInflow = 300; // amount of energy to introduce per tick
int giCostMoveSucc = 50;
int giCostShareSucc = 25;
int giEatAmount = 4000;
int giCostSpawnFail = 700;
int giCostSpawnSucc = 6000;
int giDecayRate = 10;
int iCostShareFail = giCostShareSucc * 2;// 50;
int iCostMoveFail = giCostMoveSucc;// *2;
int giCostInfo = 5;
int giMutationAmount = 64; 
int giChokePerc = 10; // limit cells to this percentage of the available space

wchar_t *garrWorldTypes[] = { L"toroidal", L"gravity", L"testing" };
int giWorldTypesSize = sizeof(garrWorldTypes) / sizeof(garrWorldTypes[0]);
int giWorldTypesIndex = 0; // toroidal

wchar_t *garrCellTypes[] = { L"neural-net", L"program" };
int giCellTypesSize = sizeof(garrCellTypes) / sizeof(garrCellTypes[0]);
int giCellTypesIndex = 0; // neural-net

void ReadSettings()
{
	guMaxThreads = std::thread::hardware_concurrency();

	std::ifstream f("config.txt");
	std::string s;
	//std::stringstream ss(s);
	while (f)
	{
		std::getline(f, s);
		s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
		s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
		std::istringstream line(s);
		std::string key;
		std::string value;
		std::getline(line, key, ',');
		if (key[0] == ';') continue;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		std::getline(line, value);
		if (key == "width")
			giWorldWidth = std::stoi(value);
		else if (key == "height")
			giWorldHeight = std::stoi(value);
		else if (key == "costmovesucc")
			giCostMoveSucc = std::stoi(value);
		else if (key == "costsharesucc")
			giCostShareSucc = std::stoi(value);
	}
}