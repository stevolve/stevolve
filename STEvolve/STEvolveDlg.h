
// STEvolveDlg.h : header file
//

#pragma once


// CSTEvolveDlg dialog
class CSTEvolveDlg : public CDialogEx
{
// Construction
public:
	CSTEvolveDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STEVOLVE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL DestroyWindow();
	afx_msg void OnNewButton();
	afx_msg void OnRefreshButton();
	afx_msg void OnPauseButton();
	afx_msg void OnPlusButton();
	afx_msg void OnMinusButton();
	afx_msg void OnRadioChange(UINT id);
};
