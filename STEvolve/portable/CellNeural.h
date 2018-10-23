#pragma once

#define NUMINPUTS 5
#define NUMNEURON1 8
#define NUMNEURON2 8
#define NUMOUTPUTS 6
class NeuralBasedCell : public Cell
{
public:
	double input[NUMINPUTS];
	int weights1[NUMNEURON1][NUMINPUTS];
	double neuron1[NUMNEURON1];
	int weights2[NUMNEURON2][NUMNEURON1];
	double neuron2[NUMNEURON2];
	int weights3[NUMOUTPUTS][NUMNEURON2];
	double output[NUMOUTPUTS];

public:
	void init();
	void Dump(FILE *);
	void DrawCell(int, int);
	void Mutate();
	void Turn(int);
	virtual bool Spawn(int, int);
	void Tick(int, int);

	void SpawnTest(int, int);
	void ResetTest();
};

