
// STEvolveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "STEvolve.h"
#include "STEvolveDlg.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

#include <atomic>
#include <stdint.h>

#include "portable\Settings.h"
#include "portable\CellBase.h"
#include "portable\CellNeural.h"
#include "portable\WorldBase.h"

#include "DisplayWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool bThreadPause = false;
bool gbRefreshStop = true;
bool gbThreadStop = false;
extern int giMagnification;
extern int colorScheme;

HDC ghCurrentDC = NULL;
HDC ghMemDC = NULL;
HBITMAP ghBitmap = NULL;

CDisplayWnd *pDisplayWnd = NULL;
DWORD dwThreadID;
HANDLE hThreadExecute = NULL;
extern HANDLE ghEvent;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSTEvolveDlg dialog



CSTEvolveDlg::CSTEvolveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STEVOLVE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSTEvolveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSTEvolveDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CSTEvolveDlg::OnNewButton)
//	ON_BN_CLICKED(IDC_BUTTON2, &CSTEvolveDlg::OnRefreshButton)
//	ON_BN_CLICKED(IDC_BUTTON3, &CSTEvolveDlg::OnPauseButton)
	ON_BN_CLICKED(IDC_CHECK1, &CSTEvolveDlg::OnRefreshButton)
	ON_BN_CLICKED(IDC_CHECK2, &CSTEvolveDlg::OnPauseButton)
	ON_BN_CLICKED(IDC_BUTTON4, &CSTEvolveDlg::OnPlusButton)
	ON_BN_CLICKED(IDC_BUTTON5, &CSTEvolveDlg::OnMinusButton)
	ON_BN_CLICKED(IDC_BUTTON6, &CSTEvolveDlg::OnSettingsButton)
	ON_COMMAND_RANGE(IDC_RADIO1, IDC_RADIO7, OnRadioChange)
END_MESSAGE_MAP()


// CSTEvolveDlg message handlers

BOOL CSTEvolveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ReadSettings();

	CRect r;
	GetClientRect(r);
	pDisplayWnd = new CDisplayWnd();
	pDisplayWnd->Create(NULL, (LPCTSTR)AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, NULL, (HBRUSH)::GetStockObject(DKGRAY_BRUSH), NULL),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL, CRect(10, 30, r.Width() - 10, r.Height() - 20), this, 123);
	
	ghCurrentDC = ::GetDC(pDisplayWnd->GetSafeHwnd());
	ghMemDC = CreateCompatibleDC(ghCurrentDC);
	ghBitmap = CreateCompatibleBitmap(ghCurrentDC, giWorldWidth + 1 + 9, giWorldHeight); // '+ 1 + 9' is for the genome legend
	::SelectObject(ghMemDC, ghBitmap);
	::FillRect(ghMemDC, CRect(giWorldWidth + 1, 0, giWorldWidth + 1 + 9, giWorldHeight), (HBRUSH)::GetStockObject(DKGRAY_BRUSH));

	::SelectObject(ghCurrentDC, ::GetStockObject(WHITE_PEN));
	::SelectObject(ghCurrentDC, ::GetStockObject(NULL_BRUSH));


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSTEvolveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSTEvolveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSTEvolveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSTEvolveDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (pDisplayWnd) pDisplayWnd->MoveWindow(10, 30, cx - 20, cy - 60);
}


BOOL CSTEvolveDlg::DestroyWindow()
{
	if (ghBitmap) ::DeleteObject(ghBitmap);
	if (ghMemDC) ::DeleteDC(ghMemDC);
	if (ghCurrentDC) ::ReleaseDC(m_hWnd, ghCurrentDC);

	// hack way to shut down the other thread for now
	gbThreadStop = true;
	WaitForSingleObject(hThreadExecute, INFINITE);

	return CDialogEx::DestroyWindow();
}


void CSTEvolveDlg::OnNewButton()
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);

	//if (TESTING)
	//	hThreadExecute = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Tester, NULL, 0, (LPDWORD)&dwThreadID);
	//else
	hThreadExecute = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, (LPDWORD)&dwThreadID);

	SetThreadPriority(hThreadExecute, THREAD_PRIORITY_LOWEST);
}

void CSTEvolveDlg::OnRefreshButton()
{
	gbRefreshStop = !gbRefreshStop;
}

void CSTEvolveDlg::OnPauseButton()
{
	bThreadPause = !bThreadPause;
	if (bThreadPause)
	{
		::ResetEvent(ghEvent);
		//UpdateDisplay();
	}
	else ::SetEvent(ghEvent);
}

void CSTEvolveDlg::OnPlusButton()
{
	giMagnification++;
}

void CSTEvolveDlg::OnMinusButton()
{
	giMagnification = max(1, giMagnification - 1);
}

void CSTEvolveDlg::OnSettingsButton()
{
	CSettingsDlg dlg;
	dlg.m_iEnergyInflow = giEnergyInflow;
	dlg.m_iShareSucc = giCostShareSucc;
	dlg.m_iCostMoveSucc = giCostMoveSucc;
	dlg.m_iEatAmount = giEatAmount;
	dlg.m_iDecayRate = giDecayRate;
	dlg.m_iSpawnSucc = giCostSpawnSucc;
	//dlg.m_iMutationRate = giMutationRate2;
	dlg.m_iMutationAmt = giMutationAmount;
	dlg.DoModal();
	giEnergyInflow = dlg.m_iEnergyInflow;
	giCostShareSucc = dlg.m_iShareSucc;
	giCostMoveSucc = dlg.m_iCostMoveSucc;
	giEatAmount = dlg.m_iEatAmount;
	giDecayRate = dlg.m_iDecayRate;
	giCostSpawnSucc = dlg.m_iSpawnSucc;
	//giMutationRate2 = dlg.m_iMutationRate;
	giMutationAmount = dlg.m_iMutationAmt;
}

void CSTEvolveDlg::OnRadioChange(UINT id)
{
	colorScheme = id - IDC_RADIO1;
}

