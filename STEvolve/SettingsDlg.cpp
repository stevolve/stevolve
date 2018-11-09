// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "STEvolve.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

#include "portable\Settings.h"


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
	, m_iNumThreads(0)
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
	DDX_CBIndex(pDX, IDC_COMBO1, m_iWorldTypesIndex);
	DDX_CBIndex(pDX, IDC_COMBO2, m_iCellTypesIndex);
	DDX_Text(pDX, IDC_EDIT9, m_iNumThreads);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
END_MESSAGE_MAP()


// CSettingsDlg message handlers


BOOL CSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (m_bRunning)
	{
		GetDlgItem(IDC_COMBO1)->EnableWindow(false);
		GetDlgItem(IDC_COMBO2)->EnableWindow(false);
	}

	int i;

	CComboBox *world = (CComboBox *)GetDlgItem(IDC_COMBO1);
	for (i = 0; i < giWorldTypesSize; i++)
		world->AddString(CString(garrWorldTypes[i]));
	world->SetCurSel(m_iWorldTypesIndex);

	CComboBox *cell = (CComboBox *)GetDlgItem(IDC_COMBO2);
	for (i = 0; i < giCellTypesSize; i++)
		cell->AddString(CString(garrCellTypes[i]));
	cell->SetCurSel(m_iCellTypesIndex);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
