// DisplayWnd.cpp : implementation file
//

#include "stdafx.h"
#include "STEvolve.h"
#include "DisplayWnd.h"

void MouseWatchCell(int, int);
void SelectWatchCell(int, int);

int giVScrollPos = 0;
int giHScrollPos = 0;

// CDisplayWnd

IMPLEMENT_DYNAMIC(CDisplayWnd, CWnd)

CDisplayWnd::CDisplayWnd()
{

}

CDisplayWnd::~CDisplayWnd()
{
}


BEGIN_MESSAGE_MAP(CDisplayWnd, CWnd)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CDisplayWnd message handlers




void CDisplayWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iDir = 0;

	switch (nSBCode)
	{
	case SB_LINELEFT:
		iDir = 1;
		break;
	case SB_LINERIGHT:
		iDir = -1;
		break;
	case SB_PAGELEFT:
		iDir = 10;
		break;
	case SB_PAGERIGHT:
		iDir = -10;
		break;
	}

	giHScrollPos = GetScrollPos(SB_HORZ);
	ScrollWindow(iDir, 0);
	SetScrollPos(SB_HORZ, giHScrollPos - iDir);

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDisplayWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iDir = 0;

	switch (nSBCode)
	{
	case SB_LINELEFT:
		iDir = 1;
		break;
	case SB_LINERIGHT:
		iDir = -1;
		break;
	case SB_PAGELEFT:
		iDir = 10;
		break;
	case SB_PAGERIGHT:
		iDir = -10;
		break;
	}

	giVScrollPos = GetScrollPos(SB_VERT);
	ScrollWindow(0, iDir);
	SetScrollPos(SB_VERT, giVScrollPos - iDir);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDisplayWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	SetScrollRange(SB_HORZ, 0, cx);
	SetScrollRange(SB_VERT, 0, cy);
}


void CDisplayWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	MouseWatchCell(point.x, point.y);

	CWnd::OnMouseMove(nFlags, point);
}


void CDisplayWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SelectWatchCell(point.x, point.y);

	CWnd::OnLButtonDown(nFlags, point);
}
