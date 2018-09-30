// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "STEvolve.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"


// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG1, pParent)
	, m_iEnergyInflow(0)
	, m_iShareSucc(0)
	, m_iCostMoveSucc(0)
	, m_iEatAmount(0)
	, m_iDecayRate(0)
	, m_iSpawnSucc(0)
	, m_iMutationRate(0)
	, m_iMutationAmt(0)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_iEnergyInflow);
	DDX_Text(pDX, IDC_EDIT2, m_iShareSucc);
	DDX_Text(pDX, IDC_EDIT3, m_iCostMoveSucc);
	DDX_Text(pDX, IDC_EDIT4, m_iEatAmount);
	DDX_Text(pDX, IDC_EDIT5, m_iDecayRate);
	DDX_Text(pDX, IDC_EDIT6, m_iSpawnSucc);
	DDX_Text(pDX, IDC_EDIT7, m_iMutationRate);
	DDX_Text(pDX, IDC_EDIT8, m_iMutationAmt);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
END_MESSAGE_MAP()


// CSettingsDlg message handlers
