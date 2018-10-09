#pragma once

#define NUMTHREADS 1
#define CELLTYPE NeuralBasedCell
//#define CELLTYPE ProgramBasedCell
#define TESTING false
//#define GRAVITY

extern int giWorldHeight;
extern int giWorldWidth;
extern int giNumThreads;
extern int giEnergyInflow;
extern int giCostMoveSucc;
extern int giCostShareSucc;
extern int giEatAmount;
extern int giCostSpawnFail;
extern int giCostSpawnSucc;
extern int giDecayRate;
extern int iCostShareFail;
extern int iCostMoveFail;
extern int giCostInfo;
extern int giMutationAmount;

void ReadSettings();
