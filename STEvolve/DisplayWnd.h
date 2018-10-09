#pragma once


// CDisplayWnd

class CDisplayWnd : public CWnd
{
	DECLARE_DYNAMIC(CDisplayWnd)

public:
	CDisplayWnd();
	virtual ~CDisplayWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


