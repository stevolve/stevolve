#include "portable\Settings.h"
#include "portable\CellBase.h"
#include "portable\CellNeural.h"
#include "portable\CellProgram.h"
#include "portable\WorldBase.h"


void DrawOpcode(Cell *c, int *rgb)
{
	if (c->bDead)
		{ rgb[0] = 100; rgb[1] = 100; rgb[2] = 100; }  // (grey) dead 
	else if (!c->iExternalCycles)
		{ rgb[0] = 255; rgb[1] = 100; rgb[2] = 100; }  // (pink) spawned, but not yet running
	else switch (c->prevInst % OPCODE_COUNT)
	{
	case MOVE:
		rgb[0] = 0; rgb[1] = 255; rgb[2] = 0;  // (green)
		break;
	case SHARE:
		rgb[0] = 255; rgb[1] = 0; rgb[2] = 0;  // (red)
		break;
	case SPAWN:
		rgb[0] = 0; rgb[1] = 0; rgb[2] = 255; // (blue)
		break;
	case INFO:
		rgb[0] = 255; rgb[1] = 255; rgb[2] = 0;  // (yellow)
		break;
	default:
		rgb[0] = 255; rgb[1] = 255; rgb[2] = 255;  // shouldn't happen
//				Trace("BAD OPCODE: prevInst=%d\n", (int)c->prevInst);
		break;
	}
}

void SetPixelHLS(int x, int y, unsigned short hue, unsigned short lum)
{
}

void SetPixelRGB(int x, int y, unsigned long rgb)
{
}

void ClearPixels()
{
}

void UpdateDisplay()
{
}

