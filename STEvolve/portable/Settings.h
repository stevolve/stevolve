#pragma once

#define NUMTHREADS 1
#define CELLTYPE NeuralBasedCell
//#define CELLTYPE ProgramBasedCell

extern wchar_t *garrWorldTypes[];
extern int giWorldTypesSize;
extern int giWorldTypesIndex;

extern wchar_t *garrCellTypes[];
extern int giCellTypesSize;
extern int giCellTypesIndex;

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
