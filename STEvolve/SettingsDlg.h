#pragma once
#include "afxwin.h"


// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bRunning = true;
	ULONGLONG m_iEnergyInflow;
	int m_iShareSucc;
	int m_iCostMoveSucc;
	int m_iEatAmount;
	int m_iDecayRate;
	int m_iSpawnSucc;
	int m_iMutationRate;
	int m_iMutationAmt;
	int m_iWorldTypesIndex;
	int m_iCellTypesIndex;
	virtual BOOL OnInitDialog();
	int m_iNumThreads;
};
