#include "stdafx.h"
#include "portable\Settings.h"
#include "portable\Draw.h"

extern HDC ghMemDC;
extern HDC ghCurrentDC;
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
}

void Trace(char *szFormat, ...)
{

}