#include "stdafx.h"
#include "portable\Settings.h"
#include "portable\Draw.h"

#include <atomic>
#include <stdint.h>
#include "portable\CellBase.h"
#include "portable\CellNeural.h"
#include "portable\CellProgram.h"

extern HDC ghMemDC;
extern HDC ghCurrentDC;
extern HWND ghStatusbar;
extern Cell *gpWatchCell;
extern int giVScrollPos;
extern int giHScrollPos;
int giMagnification = 2;

void SetPixelHLS(int x, int y, unsigned short hue, unsigned short lum)
{
	//	::SetPixel(ghCurrentDC, x, y, RGB(color & 0x30, color & 0xC0, color & 0x03));
	//	::SetPixelV(ghCurrentDC, x, y, RGB((color & 0xC0), (color & 0x38) << 2, (color & 0x07) << 5));
	//	::SetPixelV(ghMemDC, x, y, RGB((color & 0xC0), (color & 0x38) << 2, (color & 0x07) << 5));
	::SetPixelV(ghMemDC, x, y, ColorHLSToRGB(hue, lum, 240));
}

void SetPixelRGB(int x, int y, unsigned long rgb)
{
	::SetPixelV(ghMemDC, x, y, rgb);
}

void ClearPixels()
{
	::FillRect(ghMemDC, CRect(0, 0, giWorldWidth, giWorldHeight), (HBRUSH)::GetStockObject(BLACK_BRUSH));
	//::FillRect(ghMemDC, CRect(giWorldWidth + 1, 0, giWorldWidth + 1 + 9, giWorldHeight), (HBRUSH)::GetStockObject(DKGRAY_BRUSH));
}

void UpdateDisplay()
{
	::StretchBlt(ghCurrentDC, 0, 0, (giWorldWidth + 1 + 9) * giMagnification, giWorldHeight * giMagnification, ghMemDC, giHScrollPos, giVScrollPos, giWorldWidth + 1 + 9, giWorldHeight, SRCCOPY);
	if (gpWatchCell) gpWatchCell->DrawCell(giWorldWidth + 1, 0);
}

void UpdateStatusbar(CString &szText)
{
	SetWindowText(ghStatusbar, szText);
}

void Trace(char *szFormat, ...)
{
	CString temp;
	va_list args;
	va_start(args, szFormat);
#ifdef _DEBUG
	ATL::CTrace::TraceV(NULL, -1, atlTraceGeneral, 0, szFormat, args);
	//TRACE(szFormat, args);
#endif
	temp.FormatV(CStringW(szFormat), args);
	va_end(args);

	UpdateStatusbar(temp);
}