#pragma once

// simplest instructions
#define NOT    0x0
#define XOR    0x1
#define AND    0x2
#define OR     0x3
#define INC    0x4
#define DEC    0x5
#define SHIFTL 0x6
#define IF     0x7
#define JUMP   0x8
#define LOADP  0x9

#define PUSH   0xa
#define POP    0xb
#define READG  0xc
#define WRITEG 0xd
#define READB  0xe
#define WRITEB 0xf
#define XCHG   0x10

#define GENOME_DEPTH 256
//#define STACK_DEPTH (GENOME_DEPTH / 8)
#define STACK_DEPTH 64

class ProgramBasedCell : public Cell
{
public:
	// Memory space for cell genome (genome is stored as four
	// bit instructions packed into machine size words) 
	// high-order bit is 1 if the genome has been executed this cycle
	uint8_t genome[GENOME_DEPTH];
	// Buffer used for execution output of candidate offspring 
	uint8_t outputBuf[GENOME_DEPTH];
	// Virtual machine loop/rep stack 
	uint16_t Stack[STACK_DEPTH];
	uint16_t StackPtr = 0;
	// Virtual machine memory pointer register 
	uint16_t ptrPtr = 0;
	// current instruction pointer
	uint16_t instPtr = 0;

	uint8_t linecount[GENOME_DEPTH]; // number of times each line was executed

	void init();
	void Seed();

	void Dump(FILE *);
	void DrawCell(int, int);
	void Mutate();
	virtual bool Spawn(int, int);
	void Tick(int, int);

	void SpawnTest(int, int);
	void ResetTest();
};
